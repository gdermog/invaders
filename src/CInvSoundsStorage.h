//****************************************************************************************************
//! \file CInvSoundsStorage.h
//! Module declares class CInvSoundsStorage, which...
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

  /*! \brief The class represents ...*/
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

    std::shared_ptr<CInvSound> GetSound( const std::string & soundId ) const;

    void PlaySound( const std::string & soundId ) const;

    void PlaySoundLoop( const std::string & soundId ) const;

    void StopSound( const std::string & soundId ) const;

    const CInvAudio & GetAudio() const { return mAudio; }

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
