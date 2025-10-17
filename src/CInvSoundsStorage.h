//****************************************************************************************************
//! \file CInvSoundsStorage.h
//! Module declares class CInvSoundsStorage, which represents storage of in-game sounds.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvSoundsStorage
#define H_CInvSoundsStorage

#include <InvGlobals.h>
#include <CInvSettings.h>
#include <CInvAudio.h>

namespace Inv
{

  /*! \brief The class represents storage of in-game sounds. It allows adding sounds from files,
      retrieving them by ID, and playing/stopping them as needed. The class uses CInvAudio
      for audio playback and takes configuration parameters from CInvSettings. */
  class CInvSoundsStorage
  {
    public:

    CInvSoundsStorage(
      const CInvSettings & settings,
      const CInvAudio & audio);

    CInvSoundsStorage( const CInvSoundsStorage & ) = delete;
    CInvSoundsStorage & operator=( const CInvSoundsStorage & ) = delete;
    ~CInvSoundsStorage();

    std::shared_ptr<CInvSound> AddSound(
      const std::string & soundId,
      const std::string & soundRelPath );
    /*!< \brief Adds sound to the storage by loading it from file.

         \param[in] soundId        Unique ID of the sound within the storage
         \param[in] soundRelPath   Relative path to the sound file, relative to
                                   mSettings.GetSoundPath()
         \return Shared pointer to the loaded sound, or nullptr if loading failed. */

    std::shared_ptr<CInvSound> GetSound( const std::string & soundId ) const;
    /*!< \brief Retrieves sound from the storage by its ID.

         \param[in] soundId   Unique ID of the sound within the storage
         \return Shared pointer to the sound, or nullptr if not found. */

    void PlaySound( const std::string & soundId ) const;
    /*!< \brief Plays the sound with the given ID once.

         \param[in] soundId   Unique ID of the sound within the storage */

    void PlaySoundLoop( const std::string & soundId ) const;
    /*!< \brief Plays the sound with the given ID in a loop.

         \param[in] soundId   Unique ID of the sound within the storage */

    void StopSound( const std::string & soundId ) const;
    /*!< \brief Stops the sound with the given ID if it is currently playing.

         \param[in] soundId   Unique ID of the sound within the storage */

    const CInvAudio & GetAudio() const { return mAudio; }
    /*!< \brief Returns reference to audio engine used by this storage. */

  private:

    const CInvSettings & mSettings;
    //<! Reference to settings object, all parameters are taken from here

    const CInvAudio & mAudio;
    //<! Reference to audio engine, used to load sounds

    std::map<std::string, std::shared_ptr<CInvSound>> mSoundMap;
    //<! Map of sounds stored in this class

  };

} // namespace Inv

#endif
