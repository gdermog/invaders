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
  // Datová struktura s naèteným zvukem
  // -----------------------------
  struct CInvSound
  {
    // Surová interleavovaná PCM data
    std::vector<uint8_t> data;

    // Základní WAVEFORMATEX pro rychlý pøístup (frekvence, kanály…)
    WAVEFORMATEX         wfex{};

    // Kompletní payload 'fmt ' chunku (kvùli WAVEFORMATEXTENSIBLE, ale ukládáme vždy)
    std::vector<uint8_t> wfraw;

    // true, pokud formát byl WAVE_FORMAT_EXTENSIBLE
    bool                 isExtensible = false;

    IXAudio2SourceVoice * actPlaying = nullptr;
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
    bool LoadWav( const std::string & path, CInvSound & outSound, std::string * error = nullptr );

    // Loader pøes Media Foundation (MP3/AAC/WMA/… -> PCM)
    bool LoadViaMediaFoundation( const std::string & path, CInvSound & outSound, std::string * error = nullptr );

    // Pohodlná obálka: pokud je .wav, použije LoadWav, jinak Media Foundation
    bool Load( const std::string & path, CInvSound & outSound, std::string * error = nullptr );

    // Jednorázové pøehrání (neblokující). Vytvoøí doèasnou voice, po dohrání se sama znièí.
    bool PlayOneShot( const CInvSound & snd, float volume = 1.0f );

    // Smyèka: vrací handle (IXAudio2SourceVoice*). Ukonèi pøes Stop(handle).
    void PlayLoop( CInvSound & snd, float volume = 1.0f, bool restart = false );

    // Zastaví a zlikviduje voice vrácenou z PlayLoop
    void Stop( CInvSound & snd );

  private:

    // Pomùcka: pøevod bajtù na poèet "sample-frames" (1 frame = vzorek všech kanálù)
    static uint32_t BytesToSamples( uint32_t bytes, const WAVEFORMATEX & wf );

    // --- Interní helper: vytvoøí SourceVoice, nasype buffer, spustí pøehrávání ---
    static bool CreateSourceAndSubmit(
      const CInvSound & snd,
      IXAudio2 * xa,
      IXAudio2SourceVoice ** outVoice,
      XAUDIO2_BUFFER & outBuf,
      bool loop,
      float volume,
      IXAudio2VoiceCallback * cb
    );

    // --- Stav enginu ---
    bool                    m_comInit = false;            // jestli jsme v tomto vláknì volali CoInitializeEx
    IXAudio2 * m_xa = nullptr;          // XAudio2 engine
    IXAudio2MasteringVoice * m_master = nullptr;          // výstupní mastering voice

    // --- Voice callback: autodestrukce one-shot voice po dohrání bufferu ---
    struct VoiceCallback: public IXAudio2VoiceCallback
    {
      void STDMETHODCALLTYPE OnStreamEnd() override {}
      void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
      void STDMETHODCALLTYPE OnVoiceProcessingPassStart( UINT32 ) override {}
      void STDMETHODCALLTYPE OnBufferStart( void * ) override {}
      void STDMETHODCALLTYPE OnLoopEnd( void * ) override {}
      void STDMETHODCALLTYPE OnVoiceError( void *, HRESULT ) override {}
      void STDMETHODCALLTYPE OnBufferEnd( void * pContext ) override; // znièí voice pøedanou v pContext
    } m_callback;


  };

} // namespace Inv
