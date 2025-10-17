#pragma once
// Minimalistický XAudio2 wrapper pro DX9 projekt (C++17, MSVC/VS2022).
// Funkce: LoadWav(), LoadViaMediaFoundation(), LoadAudioAuto(), PlayOneShot(), PlayLoop(), Stop().

#include <cstdint>
#include <string>
#include <vector>
#include <xaudio2.h>


namespace Inv
{

  // -----------------------------
  // Datová struktura s načteným zvukem
  // -----------------------------
  struct CInvSound
  {
    // Surová interleavovaná PCM data
    std::vector<uint8_t> data;

    // Základní WAVEFORMATEX pro rychlý přístup (frekvence, kanály…)
    WAVEFORMATEX         wfex{};

    // Kompletní payload 'fmt ' chunku (kvůli WAVEFORMATEXTENSIBLE, ale ukládáme vždy)
    std::vector<uint8_t> wfraw;

    // true, pokud formát byl WAVE_FORMAT_EXTENSIBLE
    bool                 isExtensible = false;

    IXAudio2SourceVoice * actPlaying = nullptr;
  };


  // --- Voice callback: autodestrukce one-shot voice po dohrání bufferu ---
  struct VoiceCallback: public IXAudio2VoiceCallback
  {
    void STDMETHODCALLTYPE OnStreamEnd() override {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart( UINT32 ) override {}
    void STDMETHODCALLTYPE OnBufferStart( void * ) override {}
    void STDMETHODCALLTYPE OnLoopEnd( void * ) override {}
    void STDMETHODCALLTYPE OnVoiceError( void *, HRESULT ) override {}
    void STDMETHODCALLTYPE OnBufferEnd( void * pContext ) override; // zničí voice předanou v pContext
  };

  // -----------------------------
  // Audio engine (XAudio2 + MF dekodéry)
  // -----------------------------
  class CInvAudio
  {
  public:
    CInvAudio();             // Inicializuje COM (MTA), Media Foundation a XAudio2 (mastering voice)
    ~CInvAudio();

    CInvAudio( const CInvAudio & ) = delete;
    CInvAudio & operator=( const CInvAudio & ) = delete;
    CInvAudio( CInvAudio && ) = default;
    CInvAudio & operator=( CInvAudio && ) = default;

    // WAV loader (RIFF WAVE; podporuje 16B PCM 'fmt ', WAVEFORMATEX i WAVEFORMATEXTENSIBLE)
    bool LoadWav( const std::string & path, CInvSound & outSound, std::string * error = nullptr ) const;

    // Loader přes Media Foundation (MP3/AAC/WMA/… -> PCM)
    bool LoadViaMediaFoundation( const std::string & path, CInvSound & outSound, std::string * error = nullptr ) const;

    // Pohodlná obálka: pokud je .wav, použije LoadWav, jinak Media Foundation
    bool Load( const std::string & path, CInvSound & outSound, std::string * error = nullptr ) const;

    // Jednorázové přehrání (neblokující). Vytvoří dočasnou voice, po dohrání se sama zničí.
    IXAudio2SourceVoice * PlayOneShot( const CInvSound & snd, float volume = 1.0f ) const;

    // Smyčka: vrací handle (IXAudio2SourceVoice*). Ukonči přes Stop(handle).
    void PlayLoop( CInvSound & snd, float volume = 1.0f, bool restart = false ) const;

    // Zastaví a zlikviduje voice vrácenou z PlayLoop
    void Stop( CInvSound & snd ) const;

  private:

    // Pomůcka: převod bajtů na počet "sample-frames" (1 frame = vzorek všech kanálů)
    static uint32_t BytesToSamples( uint32_t bytes, const WAVEFORMATEX & wf );

    static VoiceCallback mVoiceCallback;

    // --- Interní helper: vytvoří SourceVoice, nasype buffer, spustí přehrávání ---
    static bool CreateSourceAndSubmit(
      const CInvSound & snd,
      IXAudio2 * xa,
      IXAudio2SourceVoice ** outVoice,
      XAUDIO2_BUFFER & outBuf,
      bool loop,
      float volume
    );

    // --- Stav enginu ---
    bool                    mComInit = false;            // jestli jsme v tomto vlákně volali CoInitializeEx
    IXAudio2 * mXA = nullptr;          // XAudio2 engine
    IXAudio2MasteringVoice * mMaster = nullptr;          // výstupní mastering voice


  };

} // namespace Inv
