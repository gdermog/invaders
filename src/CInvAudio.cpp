#include "CInvAudio.h"
//****************************************************************************************************
//! \file CInvAudio.h
//! Module defines class CInvAudio, which implements minimalistic wrapper for XAudio2.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <windows.h>
#include <objbase.h>
#include <mmreg.h>

// Media Foundation – order is important:
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>

#include <fstream>
#include <cstring>

#include <InvStringTools.h>
#include <CInvLogger.h>

namespace Inv
{

  static const std::string lModLogId( "AUDIO" );

/****** Internal RIFF structures ********************************************************************/

#pragma pack(push, 1)
  struct RiffHeader { uint32_t id; uint32_t size; uint32_t wave; };
  struct ChunkHeader { uint32_t id; uint32_t size; };
#pragma pack(pop)

/****** Convenient functions ************************************************************************/

  /*! \brief Creates FOURCC code from four characters.

       \param[in] a  First character
       \param[in] b  Second character
       \param[in] c  Third character
       \param[in] d  Fourth character
       \return FOURCC code */
  static constexpr uint32_t FOURCCc( char a, char b, char c, char d )
  {
    return (uint32_t)(uint8_t)a
      | ( (uint32_t)(uint8_t)b << 8 )
      | ( (uint32_t)(uint8_t)c << 16 )
      | ( (uint32_t)(uint8_t)d << 24 );
  } // FOURCCc

  //------------------------------------------------------------------------------------------------

  /*! \brief Returns pointer to WAVEFORMATEX structure inside CInvSound.
       If the sound uses WAVEFORMATEXTENSIBLE, the pointer to the full structure is returned,
       otherwise pointer to the basic WAVEFORMATEX member is returned.

       \param[in] snd  Sound object
       \return Pointer to WAVEFORMATEX structure */
  inline const WAVEFORMATEX * GetWaveFormatPtr( const CInvSound & snd )
  {
    if( snd.isExtensible && !snd.wfraw.empty() )
      return reinterpret_cast<const WAVEFORMATEX *>( snd.wfraw.data() );
    return &snd.wfex;
  }

/****** Voice callback ******************************************************************************/

  void VoiceCallback::OnBufferEnd( void * pContext )
  {
    // Pro one-shot přehrání si v pContext předáme ukazatel na voice
    if( auto * v = reinterpret_cast<IXAudio2SourceVoice *>( pContext ) )
    {
      v->Stop( 0 );
      v->DestroyVoice();
    }
  }

/****** CInvAudio implementation ********************************************************************/

  CInvAudio::CInvAudio():
    mComInit( false ),
    mXA( nullptr ),
    mMaster( nullptr )
  {

    HRESULT hrCI = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
                        // COM init (required for XAudio2/MMDevice & Media Foundation)

    if( SUCCEEDED( hrCI ) || hrCI == S_FALSE )
      mComInit = true;
    else if( hrCI == RPC_E_CHANGED_MODE )
    {                   // COM is already running in a different model; continue
                        // (but it is recommended to keep the MTA in the entry-point).
    } // else if
    else
    {
      LOG << "CoInitializeEx failed";
      exit( EXIT_FAILURE );
    } // else

    HRESULT hr = MFStartup( MF_VERSION, MFSTARTUP_NOSOCKET );
    if( FAILED( hr ) )
    {                   // Media Foundation( for MF decoders : MP3 / AAC / WMA → PCM )
      LOG << "MFStartup failed";
      exit( EXIT_FAILURE );
    } // if

    if( FAILED( XAudio2Create( &mXA, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
    {
      LOG << "XAudio2Create failed";
      exit( EXIT_FAILURE );
    } // if

    if( FAILED( mXA->CreateMasteringVoice( &mMaster ) ) )
    {
      LOG << "CreateMasteringVoice failed";
      exit( EXIT_FAILURE );
    } // if

  } // CInvAudio::CInvAudio

  //------------------------------------------------------------------------------------------------

  CInvAudio::~CInvAudio()
  {
    if( nullptr != mMaster )
      mMaster->DestroyVoice();

    if( nullptr != mXA )
      mXA->Release();

    MFShutdown();

    if( mComInit )
      CoUninitialize();

  } // CInvAudio::~CInvAudio

  //------------------------------------------------------------------------------------------------

  bool CInvAudio::LoadWav( const std::string & path, CInvSound & outSound ) const
  {
    outSound = CInvSound{};

    std::ifstream f( path, std::ios::binary );
    if( ! f.is_open() )
    {
      LOG << "Failed to open file " << path;
      return false;
    }

    RiffHeader rh{};
    f.read( reinterpret_cast<char *>( &rh ), sizeof( rh ) );
    if( !f || rh.id != FOURCCc( 'R', 'I', 'F', 'F' ) || rh.wave != FOURCCc( 'W', 'A', 'V', 'E' ) )
    {
      LOG << path << " is not a RIFF/WAVE file";
      return false;
    }

    bool haveFmt = false;
    bool haveData = false;

    std::vector<uint8_t> fmtRaw;
    WAVEFORMATEX wfex{};
    std::vector<uint8_t> pcm;

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
          if( !f )
          {
            LOG << "Read 'fmt' chunk from " << path << " failed";
            return false;
          } // if
        } // if

        if( ch.size < 16 )
        {
          LOG << "Invalid 'fmt ' chunk (too small) in " << path;
          return false;
        } // if

        struct PCMWAVEFORMAT_MIN
        {
          WAVEFORMAT wfx; // wFormatTag, nChannels, nSamplesPerSec, nAvgBytesPerSec, nBlockAlign
          WORD       wBitsPerSample;
        };

        std::memset( &wfex, 0, sizeof( wfex ) );
        auto * ppcm = reinterpret_cast<const PCMWAVEFORMAT_MIN *>( fmtRaw.data() );

        wfex.wFormatTag = ppcm->wfx.wFormatTag;
        wfex.nChannels = ppcm->wfx.nChannels;
        wfex.nSamplesPerSec = ppcm->wfx.nSamplesPerSec;
        wfex.nAvgBytesPerSec = ppcm->wfx.nAvgBytesPerSec;
        wfex.nBlockAlign = ppcm->wfx.nBlockAlign;

        if( ch.size == 16 && wfex.wFormatTag == WAVE_FORMAT_PCM )
        {
          wfex.wBitsPerSample = ppcm->wBitsPerSample;
          wfex.cbSize = 0;
          outSound.isExtensible = false;
        }
        else if( wfex.wFormatTag == WAVE_FORMAT_EXTENSIBLE )
        {
          if( ch.size < ( sizeof( WAVEFORMATEX ) + 22u ) )
          {
            LOG << "Invalid WAVE_FORMAT_EXTENSIBLE (fmt too small) in " << path;
            return false;
          } // if
          outSound.isExtensible = true;
        } // if
        else
        {
          if( ch.size < sizeof( WAVEFORMATEX ) )
          {
            LOG << "Invalid non-PCM WAVEFORMATEX (fmt too small) in " << path;
            return false;
          } // if
          std::memcpy( &wfex, fmtRaw.data(), sizeof( WAVEFORMATEX ) );
          outSound.isExtensible = false;
        } // else

        if( ch.size & 1 )
          f.seekg( 1, std::ios::cur );

        haveFmt = true;
      }
      else if( ch.id == FOURCCc( 'd', 'a', 't', 'a' ) )
      {
        pcm.resize( ch.size );
        if( ch.size )
        {
          f.read( reinterpret_cast<char *>( pcm.data() ), ch.size );
          if( !f )
          {
            LOG << "Read 'data' chunk from " << path << " failed";
            return false;
          } // if
        } // if

        haveData = true;

        if( ch.size & 1 )
          f.seekg( 1, std::ios::cur );
      } // else if
      else
        f.seekg( ch.size + ( ch.size & 1 ), std::ios::cur );
    } // while

    if( ! haveFmt || ! haveData )
    {
      LOG << "Missing 'fmt ' or 'data' chunk in " << path;
      return false;
    } // if

    outSound.data = std::move( pcm );
    outSound.wfex = wfex;
    outSound.wfraw = std::move( fmtRaw );
    return true;
  } // CInvAudio::LoadWav

  //------------------------------------------------------------------------------------------------

  bool CInvAudio::LoadViaMediaFoundation( const std::string & path, CInvSound & outSound ) const
  {
    outSound = CInvSound{};

    IMFSourceReader * reader = nullptr;
    HRESULT hr = MFCreateSourceReaderFromURL( StringToWString( path ).c_str(), nullptr, &reader );
    if( FAILED( hr ) )
    {
      LOG << "MFCreateSourceReaderFromURL failed reading " << path;
      return false;
    } // if

    IMFMediaType * outType = nullptr;
    hr = MFCreateMediaType( &outType );
    if( FAILED( hr ) )
    {
      reader->Release();
      LOG << "MFCreateMediaType failed reading " << path;
      return false;
    } // if

    outType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );
    outType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM );
    hr = reader->SetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, outType );
    outType->Release();
    if( FAILED( hr ) )
    {
      reader->Release();
      LOG << "SetCurrentMediaType PCM failed reading " << path;
      return false;
    } // if

    IMFMediaType * actual = nullptr;
    hr = reader->GetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, &actual );
    if( FAILED( hr ) )
    {
      reader->Release();
      LOG << "GetCurrentMediaType failed reading " << path;
      return false;
    } // if

    WAVEFORMATEX * pwf = nullptr; UINT32 wfSize = 0;
    hr = MFCreateWaveFormatExFromMFMediaType( actual, &pwf, &wfSize );
    if( FAILED( hr ) )
    {
      actual->Release();
      reader->Release();
      LOG << "MFCreateWaveFormatExFromMFMediaType failed reading " << path;
      return false;
    } // if

    std::vector<uint8_t> pcm;
    for( ;;)
    {
      DWORD flags = 0;
      IMFSample * sample = nullptr;
      hr = reader->ReadSample( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample );
      if( FAILED( hr ) )
      {
        CoTaskMemFree( pwf );
        actual->Release();
        reader->Release();
        LOG << "ReadSample failed reading " << path;
        return false;
      } // if

      if( flags & MF_SOURCE_READERF_ENDOFSTREAM )
      {
        if( sample ) sample->Release();
        break;
      } // if
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
        } // if
        buf->Release();
      } // if
      sample->Release();

      if( FAILED( hr ) )
      {
        CoTaskMemFree( pwf );
        actual->Release();
        reader->Release();
        LOG << "Buffer lock failed reading " << path;
        return false;
      } // if
    } // for

    actual->Release();
    reader->Release();

    if( pcm.empty() )
    {
      CoTaskMemFree( pwf );
      LOG << "Decoder produced no audio reading " << path;
      return false;
    } // if

    outSound.data = std::move( pcm );
    outSound.wfex = *pwf;
    outSound.wfraw.assign( (uint8_t *)pwf, (uint8_t *)pwf + wfSize );
    outSound.isExtensible = ( pwf->wFormatTag == WAVE_FORMAT_EXTENSIBLE );
    CoTaskMemFree( pwf );

    return true;

  } // CInvAudio::LoadViaMediaFoundation

  //------------------------------------------------------------------------------------------------

  bool CInvAudio::Load( const std::string & path, CInvSound & outSound ) const
  {
    if( EndsWithICase( path, ".wav" ) )
      return LoadWav( path, outSound );
    return LoadViaMediaFoundation( path, outSound );
  } // CInvAudio::Load

  //------------------------------------------------------------------------------------------------

  uint32_t CInvAudio::BytesToSamples( uint32_t bytes, const WAVEFORMATEX & wf )
  {
    const uint32_t ba = wf.nBlockAlign;       // bajtů na 1 frame (všechny kanály)
    return ba ? ( bytes / ba ) : 0;
  } // CInvAudio::BytesToSamples

  //------------------------------------------------------------------------------------------------

  VoiceCallback CInvAudio::mVoiceCallback;

  //------------------------------------------------------------------------------------------------

  bool CInvAudio::CreateSourceAndSubmit(
    const CInvSound & snd,
    IXAudio2 * xa,
    IXAudio2SourceVoice ** outVoice,
    XAUDIO2_BUFFER & outBuf,
    bool loop,
    float volume )
  {
    if( !xa )
      return false;

    const WAVEFORMATEX * pwf = GetWaveFormatPtr( snd );
    if( FAILED( xa->CreateSourceVoice( outVoice, pwf,
      0, XAUDIO2_DEFAULT_FREQ_RATIO,
      &mVoiceCallback, nullptr, nullptr ) ) )
      return false;

    std::memset( &outBuf, 0, sizeof( outBuf ) );
    outBuf.AudioBytes = static_cast<UINT32>( snd.data.size() );
    outBuf.pAudioData = snd.data.data();

    if( loop )
    {
      outBuf.LoopBegin = 0;
      outBuf.LoopLength = BytesToSamples( outBuf.AudioBytes, *pwf );
      outBuf.LoopCount = XAUDIO2_LOOP_INFINITE;
    } // if
    else
    {
      outBuf.Flags = XAUDIO2_END_OF_STREAM;
      outBuf.pContext = *outVoice;
    } // else

    if( FAILED( ( *outVoice )->SubmitSourceBuffer( &outBuf ) ) )
    {
      ( *outVoice )->DestroyVoice();
      *outVoice = nullptr;
      return false;
    } // if

    ( *outVoice )->SetVolume( volume );
    if( FAILED( ( *outVoice )->Start( 0 ) ) )
    {
      ( *outVoice )->DestroyVoice();
      *outVoice = nullptr;
      return false;
    } // if

    return true;

  } // CInvAudio::CreateSourceAndSubmit

  //------------------------------------------------------------------------------------------------

  IXAudio2SourceVoice * CInvAudio::PlayOneShot( const CInvSound & snd, float volume ) const
  {
    IXAudio2SourceVoice * voice = nullptr;
    XAUDIO2_BUFFER buf{};
    auto retVal = CreateSourceAndSubmit( snd, mXA, &voice, buf, false, volume );
    return retVal ? voice : nullptr;
  } // CInvAudio::PlayOneShot

  //------------------------------------------------------------------------------------------------

  void CInvAudio::PlayLoop( CInvSound & snd, float volume, bool restart ) const
  {
    if( snd.actPlaying != nullptr )
    {
      if( restart )
        Stop( snd );
      else
        return;
    } // if

    XAUDIO2_BUFFER buf{};
    if( !CreateSourceAndSubmit( snd, mXA, &snd.actPlaying, buf, true, volume ) )
      snd.actPlaying = nullptr;

  } // CInvAudio::PlayLoop

  //------------------------------------------------------------------------------------------------

  void CInvAudio::Stop( CInvSound & snd ) const
  {
    if( nullptr == snd.actPlaying ) return;
    snd.actPlaying->Stop( 0 );
    snd.actPlaying->FlushSourceBuffers();
    snd.actPlaying->DestroyVoice();
    snd.actPlaying = nullptr;
  } // CInvAudio::Stop

  //------------------------------------------------------------------------------------------------

} // namespace Inv
