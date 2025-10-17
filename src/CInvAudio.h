//****************************************************************************************************
//! \file CInvAudio.h
//! Module declares class CInvAudio, which implements minimalistic wrapper for XAudio2.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvAudio
#define H_CInvAudio

#include <xaudio2.h>

#include <InvGlobals.h>

namespace Inv
{
  /**************************************************************************************************/

  /*! \brief Sound data container. CInvSound holds raw PCM audio data along with its format information.
      It supports both standard WAVEFORMATEX and WAVEFORMATEXTENSIBLE formats. The structure also keeps
      track of the currently playing voice for looping sounds. */
  struct CInvSound
  {

    std::vector<uint8_t> data;
    //!< Raw interleaved PCM data

    WAVEFORMATEX         wfex{};
    //!< Basic WAVEFORMATEX for quick access (frequencies, channels…)

    std::vector<uint8_t> wfraw;
    //!< Complete payload of 'fmt ' chunk (due to WAVEFORMATEXTENSIBLE, but we always store it)

    bool                 isExtensible = false;
    //!< true if the format was WAVE_FORMAT_EXTENSIBLE

    IXAudio2SourceVoice * actPlaying = nullptr;
    //!< Currently playing voice (for looping sounds)
  };

  /**************************************************************************************************/

  /*! \brief Voice callback: one-shot voice self-destruction after buffer is finished. */
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

  /**************************************************************************************************/

  /*! \brief Class implements minimalistic wrapper for XAudio2. CInvAudio provides functionality to load
      audio files (WAV via custom loader, MP3/AAC/WMA via Media Foundation) and play sounds using XAudio2.
      It supports one-shot playback and looping playback of sounds. The class handles initialization and
      cleanup of COM, Media Foundation, and XAudio2 engine. */
  class CInvAudio
  {

  public:

    CInvAudio();
    ~CInvAudio();

    CInvAudio( const CInvAudio & ) = delete;
    CInvAudio & operator=( const CInvAudio & ) = delete;
    CInvAudio( CInvAudio && ) = default;
    CInvAudio & operator=( CInvAudio && ) = default;

    bool LoadWav( const std::string & path,  CInvSound & outSound ) const;
    /*!< \brief WAV loader (RIFF WAVE; supports 16B PCM 'fmt', WAVEFORMATEX and WAVEFORMATEXTENSIBLE)

         \param[in]  path      Path to WAV file
         \param[out] outSound  Loaded sound data
         \return true if loading was successful */

    bool LoadViaMediaFoundation( const std::string & path, CInvSound & outSound ) const;
    /*!< \brief Media Foundation loader (MP3/AAC/WMA/… → PCM)

         \param[in]  path      Path to audio file
         \param[out] outSound  Loaded sound data
         \return true if loading was successful */

    bool Load( const std::string & path, CInvSound & outSound ) const;
    /*!< \brief General loader: chooses between WAV loader and Media Foundation loader based on file extension

         \param[in]  path      Path to audio file
         \param[out] outSound  Loaded sound data
         \return true if loading was successful */


    IXAudio2SourceVoice * PlayOneShot( const CInvSound & snd, float volume = 1.0f ) const;
    /*!< \brief One-shot playback: creates SourceVoice, submits buffer, starts playback.
         Voice self-destructs after playback ends.

         \param[in] snd     Sound to play
         \param[in] volume  Playback volume (0.0f - silent, 1.0f - full)
         \return Pointer to created IXAudio2SourceVoice, or nullptr on failure */

    void PlayLoop( CInvSound & snd, float volume = 1.0f, bool restart = false ) const;
    /*!< \brief Looping playback: creates SourceVoice, submits buffer, starts playback in loop.
         If the sound is already playing in loop, it can be restarted.

         \param[in,out] snd      Sound to play in loop (its actPlaying will be updated)
         \param[in]     volume   Playback volume (0.0f - silent, 1.0f - full)
         \param[in]     restart  If true and sound is already playing, it will be restarted */

    void Stop( CInvSound & snd ) const;
    /*!< \brief Stops playback of a looping sound and releases its voice.

         \param[in,out] snd  Sound to stop (its actPlaying will be set to nullptr) */

  private:

    static uint32_t BytesToSamples( uint32_t bytes, const WAVEFORMATEX & wf );
    /*!< \brief Convenient function which convert bytes to number of "sample-frames"
         ( 1 frame = sample of all channels )

          \param[in] bytes  Number of bytes
          \param[in] wf     Wave format
          \return Number of sample-frames */

    static VoiceCallback mVoiceCallback;
    /*!< \brief Voice callback instance for one-shot playback self-destruction */

    static bool CreateSourceAndSubmit(
      const CInvSound & snd,
      IXAudio2 * xa,
      IXAudio2SourceVoice ** outVoice,
      XAUDIO2_BUFFER & outBuf,
      bool loop,
      float volume );
    /*!< \brief Creates source voice, submits buffer, prepares for playback.

         \param[in]  snd        Sound to play
         \param[in]  xa         XAudio2 engine
         \param[out] outVoice   Created source voice
         \param[out] outBuf     Submitted buffer
         \param[in]  loop       true for looping playback, false for one-shot
         \param[in]  volume     Playback volume (0.0f - silent, 1.0f - full)
         \return true if successful */

    bool mComInit = false;
    //!< true if COM was successfully initialized

    IXAudio2 * mXA = nullptr;
    //!< XAudio2 engine instance

    IXAudio2MasteringVoice * mMaster = nullptr;
    //!< Mastering voice instance

  };

} // namespace Inv

#endif
