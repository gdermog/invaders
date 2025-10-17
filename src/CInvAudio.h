#pragma once
// Minimalistick� XAudio2 wrapper pro DX9 projekt (C++17, MSVC/VS2022).
// Funkce: LoadWav(), LoadViaMediaFoundation(), LoadAudioAuto(), PlayOneShot(), PlayLoop(), Stop().

#include <cstdint>
#include <string>
#include <vector>
#include <xaudio2.h>


namespace Inv
{

  // -----------------------------
  // Datov� struktura s na�ten�m zvukem
  // -----------------------------
  struct CInvSound
  {
    // Surov� interleavovan� PCM data
    std::vector<uint8_t> data;

    // Z�kladn� WAVEFORMATEX pro rychl� p��stup (frekvence, kan�ly�)
    WAVEFORMATEX         wfex{};

    // Kompletn� payload 'fmt ' chunku (kv�li WAVEFORMATEXTENSIBLE, ale ukl�d�me v�dy)
    std::vector<uint8_t> wfraw;

    // true, pokud form�t byl WAVE_FORMAT_EXTENSIBLE
    bool                 isExtensible = false;

    IXAudio2SourceVoice * actPlaying = nullptr;
  };



  // -----------------------------
  // Audio engine (XAudio2 + MF dekod�ry)
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

    // Loader p�es Media Foundation (MP3/AAC/WMA/� -> PCM)
    bool LoadViaMediaFoundation( const std::string & path, CInvSound & outSound, std::string * error = nullptr );

    // Pohodln� ob�lka: pokud je .wav, pou�ije LoadWav, jinak Media Foundation
    bool Load( const std::string & path, CInvSound & outSound, std::string * error = nullptr );

    // Jednor�zov� p�ehr�n� (neblokuj�c�). Vytvo�� do�asnou voice, po dohr�n� se sama zni��.
    bool PlayOneShot( const CInvSound & snd, float volume = 1.0f );

    // Smy�ka: vrac� handle (IXAudio2SourceVoice*). Ukon�i p�es Stop(handle).
    void PlayLoop( CInvSound & snd, float volume = 1.0f, bool restart = false );

    // Zastav� a zlikviduje voice vr�cenou z PlayLoop
    void Stop( CInvSound & snd );

  private:

    // Pom�cka: p�evod bajt� na po�et "sample-frames" (1 frame = vzorek v�ech kan�l�)
    static uint32_t BytesToSamples( uint32_t bytes, const WAVEFORMATEX & wf );

    // --- Intern� helper: vytvo�� SourceVoice, nasype buffer, spust� p�ehr�v�n� ---
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
    bool                    m_comInit = false;            // jestli jsme v tomto vl�kn� volali CoInitializeEx
    IXAudio2 * m_xa = nullptr;          // XAudio2 engine
    IXAudio2MasteringVoice * m_master = nullptr;          // v�stupn� mastering voice

    // --- Voice callback: autodestrukce one-shot voice po dohr�n� bufferu ---
    struct VoiceCallback: public IXAudio2VoiceCallback
    {
      void STDMETHODCALLTYPE OnStreamEnd() override {}
      void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
      void STDMETHODCALLTYPE OnVoiceProcessingPassStart( UINT32 ) override {}
      void STDMETHODCALLTYPE OnBufferStart( void * ) override {}
      void STDMETHODCALLTYPE OnLoopEnd( void * ) override {}
      void STDMETHODCALLTYPE OnVoiceError( void *, HRESULT ) override {}
      void STDMETHODCALLTYPE OnBufferEnd( void * pContext ) override; // zni�� voice p�edanou v pContext
    } m_callback;


  };

} // namespace Inv
