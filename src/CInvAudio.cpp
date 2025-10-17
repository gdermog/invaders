#include "CInvAudio.h"

#include <windows.h>
#include <objbase.h>        // CoInitializeEx, CoUninitialize
#include <mmreg.h>          // WAVEFORMATEX / WAVEFORMATEXTENSIBLE

// Media Foundation – důležité je POŘADÍ:
#include <mfapi.h>
#include <mfidl.h>        // musí být před mfreadwrite.h (definuje IMFMediaSource, IMFAttributes, …)
#include <mfreadwrite.h>  // až potom
#include <mfobjects.h>    // (volitelné, většinou už zatažené přes mfidl.h)
#include <mferror.h>      // (volitelné pro MF_* HRESULTy)

#include <propvarutil.h>  // klidně až po MF hlavičkách


#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cwctype>

#include <InvStringTools.h>

namespace Inv
{

  // ====== interní RIFF struktury ======
#pragma pack(push, 1)
  struct RiffHeader { uint32_t id; uint32_t size; uint32_t wave; };
  struct ChunkHeader { uint32_t id; uint32_t size; };
#pragma pack(pop)

  static constexpr uint32_t FOURCCc( char a, char b, char c, char d )
  {
    return (uint32_t)(uint8_t)a
      | ( (uint32_t)(uint8_t)b << 8 )
      | ( (uint32_t)(uint8_t)c << 16 )
      | ( (uint32_t)(uint8_t)d << 24 );
  }

  // ====== VoiceCallback ======
  void CInvAudio::VoiceCallback::OnBufferEnd( void * pContext )
  {
    // Pro one-shot přehrání si v pContext předáme ukazatel na voice
    if( auto * v = reinterpret_cast<IXAudio2SourceVoice *>( pContext ) )
    {
      v->Stop( 0 );
      v->DestroyVoice();
    }
  }

  // ====== Pomocné funkce ======
  static inline const WAVEFORMATEX * GetWaveFormatPtr( const CInvSound & snd )
  {
    if( snd.isExtensible && !snd.wfraw.empty() )
      return reinterpret_cast<const WAVEFORMATEX *>( snd.wfraw.data() );
    return &snd.wfex;
  }

  static inline void setError( std::string * e, const char * msg, HRESULT hr = S_OK )
  {
    if( !e ) return;
    if( SUCCEEDED( hr ) ) { *e = msg; return; }
    char buf[256];
    snprintf( buf, 256, "%s (hr=0x%08X)", msg, (unsigned)hr );
    *e = buf;
  }

  // case-insensitive “ends with”
  static bool ends_with_icase( const std::string & s, const char * suf )
  {
    size_t n = s.size(), m = strlen( suf );
    if( m > n ) return false;
    for( size_t i = 0; i < m; ++i ) {
      char a = (char)towlower( s[n - m + i] );
      char b = (char)towlower( suf[i] );
      if( a != b ) return false;
    }
    return true;
  }

  // ====== Audio ======
  CInvAudio::CInvAudio()
  {
    // --- COM init (nutné pro XAudio2/MMDevice & Media Foundation) ---
    HRESULT hrCI = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    if( SUCCEEDED( hrCI ) || hrCI == S_FALSE ) {
      m_comInit = true;
    }
    else if( hrCI == RPC_E_CHANGED_MODE ) {
      // COM už běží v jiném modelu; pokračujeme (doporučeno ale držet MTA v entry-pointu).
    }
    else {
      throw std::runtime_error( "CoInitializeEx failed" );
    }

    // --- Media Foundation (pro MF dekodéry: MP3/AAC/WMA → PCM) ---
    HRESULT hr = MFStartup( MF_VERSION, MFSTARTUP_NOSOCKET );
    if( FAILED( hr ) ) throw std::runtime_error( "MFStartup failed" );

    // --- XAudio2 ---
    if( FAILED( XAudio2Create( &m_xa, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
      throw std::runtime_error( "XAudio2Create failed" );

    if( FAILED( m_xa->CreateMasteringVoice( &m_master ) ) )
      throw std::runtime_error( "CreateMasteringVoice failed" );
  }

  CInvAudio::~CInvAudio()
  {
    if( m_master ) { m_master->DestroyVoice(); m_master = nullptr; }
    if( m_xa ) { m_xa->Release();          m_xa = nullptr; }
    MFShutdown();

    if( m_comInit ) {
      CoUninitialize();
      m_comInit = false;
    }
  }

  // ====== WAV loader (RIFF WAVE; PCM / IEEE float / WAVEFORMATEXTENSIBLE) ======
  bool CInvAudio::LoadWav( const std::string & path, CInvSound & outSound, std::string * error )
  {
    outSound = CInvSound{};

    std::ifstream f( path, std::ios::binary );
    if( !f )
    {
      setError( error, "Failed to open file" );
      return false;
    }

    RiffHeader rh{};
    f.read( reinterpret_cast<char *>( &rh ), sizeof( rh ) );
    if( !f || rh.id != FOURCCc( 'R', 'I', 'F', 'F' ) || rh.wave != FOURCCc( 'W', 'A', 'V', 'E' ) )
    {
      setError( error, "Not a RIFF/WAVE file" );
      return false;
    }

    bool haveFmt = false;
    bool haveData = false;

    std::vector<uint8_t> fmtRaw;
    WAVEFORMATEX wfex{};
    std::vector<uint8_t> pcm;

    // Projdi chunky do chvíle, než máme fmt i data
    while( f && ( !haveFmt || !haveData ) )
    {
      ChunkHeader ch{};
      f.read( reinterpret_cast<char *>( &ch ), sizeof( ch ) );
      if( !f ) break;

      if( ch.id == FOURCCc( 'f', 'm', 't', ' ' ) )
      {
        fmtRaw.resize( ch.size );
        if( ch.size )
        {
          f.read( reinterpret_cast<char *>( fmtRaw.data() ), ch.size );
          if( !f ) { setError( error, "Read 'fmt ' chunk failed" ); return false; }
        }

        // Minimální validní fmt je 16 B (PCMWAVEFORMAT/PCM)
        if( ch.size < 16 ) {
          setError( error, "Invalid 'fmt ' chunk (too small)" );
          return false;
        }

        // PCMWAVEFORMAT 'min' pohled (16 B)
        struct PCMWAVEFORMAT_MIN {
          WAVEFORMAT wfx; // wFormatTag, nChannels, nSamplesPerSec, nAvgBytesPerSec, nBlockAlign
          WORD       wBitsPerSample;
        };

        std::memset( &wfex, 0, sizeof( wfex ) );
        auto * ppcm = reinterpret_cast<const PCMWAVEFORMAT_MIN *>( fmtRaw.data() );

        // kopie společných polí
        wfex.wFormatTag = ppcm->wfx.wFormatTag;
        wfex.nChannels = ppcm->wfx.nChannels;
        wfex.nSamplesPerSec = ppcm->wfx.nSamplesPerSec;
        wfex.nAvgBytesPerSec = ppcm->wfx.nAvgBytesPerSec;
        wfex.nBlockAlign = ppcm->wfx.nBlockAlign;

        if( ch.size == 16 && wfex.wFormatTag == WAVE_FORMAT_PCM ) {
          // Čisté PCM bez cbSize (PCMWAVEFORMAT)
          wfex.wBitsPerSample = ppcm->wBitsPerSample;
          wfex.cbSize = 0;
          outSound.isExtensible = false;
        }
        else if( wfex.wFormatTag == WAVE_FORMAT_EXTENSIBLE ) {
          // WAVEFORMATEXTENSIBLE vyžaduje 40 B (WAVEFORMATEX + 22)
          if( ch.size < ( sizeof( WAVEFORMATEX ) + 22 ) ) {
            setError( error, "Invalid WAVE_FORMAT_EXTENSIBLE (fmt too small)" );
            return false;
          }
          outSound.isExtensible = true;
        }
        else {
          // Ostatní (ne-PCM) očekávají WAVEFORMATEX (≥ 18 B) s cbSize
          if( ch.size < sizeof( WAVEFORMATEX ) ) {
            setError( error, "Invalid non-PCM WAVEFORMATEX (fmt too small)" );
            return false;
          }
          std::memcpy( &wfex, fmtRaw.data(), sizeof( WAVEFORMATEX ) );
          outSound.isExtensible = false;
        }

        // pad na sudý byte (RIFF)
        if( ch.size & 1 ) f.seekg( 1, std::ios::cur );

        haveFmt = true;
      }
      else if( ch.id == FOURCCc( 'd', 'a', 't', 'a' ) )
      {
        pcm.resize( ch.size );
        if( ch.size )
        {
          f.read( reinterpret_cast<char *>( pcm.data() ), ch.size );
          if( !f ) { setError( error, "Read 'data' chunk failed" ); return false; }
        }
        haveData = true;

        if( ch.size & 1 ) f.seekg( 1, std::ios::cur );
      }
      else
      {
        // přeskoč neznámý chunk + pad
        f.seekg( ch.size + ( ch.size & 1 ), std::ios::cur );
      }
    }

    if( !haveFmt || !haveData )
    {
      setError( error, "Missing 'fmt ' or 'data' chunk" );
      return false;
    }

    // Výstup
    outSound.data = std::move( pcm );
    outSound.wfex = wfex;
    outSound.wfraw = std::move( fmtRaw ); // vždy uchovejme „raw“ fmt payload
    return true;
  }

  // ====== Media Foundation loader (MP3/AAC/WMA/...) → PCM ======
  bool CInvAudio::LoadViaMediaFoundation( const std::string & path, CInvSound & outSound, std::string * error )
  {
    outSound = CInvSound{};

    IMFSourceReader * reader = nullptr;
    HRESULT hr = MFCreateSourceReaderFromURL( StringToWString( path ).c_str(), nullptr, &reader );
    if( FAILED( hr ) ) { setError( error, "MFCreateSourceReaderFromURL failed", hr ); return false; }

    // Cíl: PCM
    IMFMediaType * outType = nullptr;
    hr = MFCreateMediaType( &outType );
    if( FAILED( hr ) ) { reader->Release(); setError( error, "MFCreateMediaType failed", hr ); return false; }

    outType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );
    outType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM );
    hr = reader->SetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, outType );
    outType->Release();
    if( FAILED( hr ) ) { reader->Release(); setError( error, "SetCurrentMediaType PCM failed", hr ); return false; }

    // získej WAVEFORMATEX
    IMFMediaType * actual = nullptr;
    hr = reader->GetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, &actual );
    if( FAILED( hr ) ) { reader->Release(); setError( error, "GetCurrentMediaType failed", hr ); return false; }

    WAVEFORMATEX * pwf = nullptr; UINT32 wfSize = 0;
    hr = MFCreateWaveFormatExFromMFMediaType( actual, &pwf, &wfSize );
    if( FAILED( hr ) ) { actual->Release(); reader->Release(); setError( error, "MFCreateWaveFormatExFromMFMediaType failed", hr ); return false; }

    // čtení dekódovaných vzorků
    std::vector<uint8_t> pcm;
    for( ;;)
    {
      DWORD flags = 0;
      IMFSample * sample = nullptr;
      hr = reader->ReadSample( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample );
      if( FAILED( hr ) ) { CoTaskMemFree( pwf ); actual->Release(); reader->Release(); setError( error, "ReadSample failed", hr ); return false; }
      if( flags & MF_SOURCE_READERF_ENDOFSTREAM ) { if( sample ) sample->Release(); break; }
      if( !sample ) continue;

      IMFMediaBuffer * buf = nullptr;
      hr = sample->ConvertToContiguousBuffer( &buf );
      if( SUCCEEDED( hr ) )
      {
        BYTE * pData = nullptr; DWORD cb = 0;
        hr = buf->Lock( &pData, nullptr, &cb );
        if( SUCCEEDED( hr ) && cb )
        {
          size_t old = pcm.size(); pcm.resize( old + cb );
          memcpy( pcm.data() + old, pData, cb );
          buf->Unlock();
        }
        buf->Release();
      }
      sample->Release();

      if( FAILED( hr ) ) { CoTaskMemFree( pwf ); actual->Release(); reader->Release(); setError( error, "Buffer lock failed", hr ); return false; }
    }

    actual->Release();
    reader->Release();

    if( pcm.empty() ) { CoTaskMemFree( pwf ); setError( error, "Decoder produced no audio" ); return false; }

    // Naplň Sound
    outSound.data = std::move( pcm );
    outSound.wfex = *pwf; // základní WFEX
    outSound.wfraw.assign( (uint8_t *)pwf, (uint8_t *)pwf + wfSize ); // celý payload (může být extensible)
    outSound.isExtensible = ( pwf->wFormatTag == WAVE_FORMAT_EXTENSIBLE );
    CoTaskMemFree( pwf );

    return true;
  }

  bool CInvAudio::Load( const std::string & path, CInvSound & outSound, std::string * error )
  {
    if( ends_with_icase( path, ".wav" ) )
      return LoadWav( path, outSound, error );
    return LoadViaMediaFoundation( path, outSound, error );
  }

  uint32_t CInvAudio::BytesToSamples( uint32_t bytes, const WAVEFORMATEX & wf )
  {
    const uint32_t ba = wf.nBlockAlign;       // bajtů na 1 frame (všechny kanály)
    return ba ? ( bytes / ba ) : 0;
  }

  // ====== Vytvoření voice, submit bufferu, spuštění ======
  bool CInvAudio::CreateSourceAndSubmit(
    const CInvSound & snd,
    IXAudio2 * xa,
    IXAudio2SourceVoice ** outVoice,
    XAUDIO2_BUFFER & outBuf,
    bool loop,
    float volume,
    IXAudio2VoiceCallback * cb )
  {
    if( !xa ) return false;

    const WAVEFORMATEX * pwf = GetWaveFormatPtr( snd );
    if( FAILED( xa->CreateSourceVoice( outVoice, pwf,
      0, XAUDIO2_DEFAULT_FREQ_RATIO,
      cb, nullptr, nullptr ) ) )
      return false;

    std::memset( &outBuf, 0, sizeof( outBuf ) );
    outBuf.AudioBytes = static_cast<UINT32>( snd.data.size() );
    outBuf.pAudioData = snd.data.data();

    if( loop )
    {
      outBuf.LoopBegin = 0;
      outBuf.LoopLength = BytesToSamples( outBuf.AudioBytes, *pwf ); // v SAMPLech, ne v bajtech
      outBuf.LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    else
    {
      outBuf.Flags = XAUDIO2_END_OF_STREAM;
      outBuf.pContext = *outVoice; // one-shot autodestrukce v OnBufferEnd
    }

    if( FAILED( ( *outVoice )->SubmitSourceBuffer( &outBuf ) ) )
    {
      ( *outVoice )->DestroyVoice();
      *outVoice = nullptr;
      return false;
    }

    ( *outVoice )->SetVolume( volume );
    if( FAILED( ( *outVoice )->Start( 0 ) ) )
    {
      ( *outVoice )->DestroyVoice();
      *outVoice = nullptr;
      return false;
    }
    return true;
  }

  bool CInvAudio::PlayOneShot( const CInvSound & snd, float volume )
  {
    IXAudio2SourceVoice * voice = nullptr;
    XAUDIO2_BUFFER buf{};
    return CreateSourceAndSubmit( snd, m_xa, &voice, buf, /*loop*/false, volume, &m_callback );
  }

  void CInvAudio::PlayLoop( CInvSound & snd, float volume, bool restart )
  {
    if( snd.actPlaying != nullptr )
    {
      if( restart )
        Stop( snd );
      else
        return;
    }

    XAUDIO2_BUFFER buf{};
    if( !CreateSourceAndSubmit( snd, m_xa, &snd.actPlaying, buf, /*loop*/true, volume, &m_callback ) )
      snd.actPlaying = nullptr;
  }

  void CInvAudio::Stop( CInvSound & snd )
  {
    if( nullptr == snd.actPlaying ) return;
    snd.actPlaying->Stop( 0 );
    snd.actPlaying->FlushSourceBuffers();
    snd.actPlaying->DestroyVoice();
    snd.actPlaying = nullptr;
  }

} // namespace Inv
