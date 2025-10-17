//****************************************************************************************************
//! \file CInvSoundsStorage.cpp
//! Module defines class CInvSoundsStorage, which represents storage of in-game sounds.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <filesystem>

#include <CInvLogger.h>
#include <InvStringTools.h>
#include <CInvSoundsStorage.h>

namespace Inv
{

  static const std::string lModLogId( "SNDSTORE" );

  CInvSoundsStorage::CInvSoundsStorage(
    const CInvSettings & settings,
    const CInvAudio & audio ):

    mSettings( settings ),
    mAudio( audio ),
    mSoundMap()
  {}

  //----------------------------------------------------------------------------------------------

  CInvSoundsStorage::~CInvSoundsStorage() = default;

  //----------------------------------------------------------------------------------------------

  std::shared_ptr<CInvSound> CInvSoundsStorage::AddSound(
    const std::string & soundId,
    const std::string & soundRelPath )
  {
    auto findIt = mSoundMap.find( soundId );
    if( findIt != mSoundMap.end() )
    {
      LOG << "Warning: Sound with ID '" << soundId << "' already exists, returning existing one.";
      return findIt->second;
    } // if

    std::string err;
    std::string fnam = mSettings.GetImagePath() + "/sounds/" + soundRelPath;

    auto newSound = std::make_shared<CInvSound>();
    mAudio.Load( fnam, *newSound );

    mSoundMap[soundId] = newSound;
    return newSound;

  } // CInvSoundsStorage::AddSound

  //----------------------------------------------------------------------------------------------

  std::shared_ptr<CInvSound> CInvSoundsStorage::GetSound( const std::string & soundId ) const
  {
    auto findIt = mSoundMap.find( soundId );
    if( findIt == mSoundMap.end() )
      return nullptr;

    return findIt->second;

  } // CInvSoundsStorage::GetSprite

  //----------------------------------------------------------------------------------------------

  void CInvSoundsStorage::PlaySound( const std::string & soundId ) const
  {
    auto findIt = mSoundMap.find( soundId );
    if( findIt == mSoundMap.end() )
      return;

    mAudio.PlayOneShot( *(findIt->second), 1.0f );

  } // CInvSoundsStorage::PlaySound

  //----------------------------------------------------------------------------------------------

  void CInvSoundsStorage::PlaySoundLoop( const std::string & soundId ) const
  {
    auto findIt = mSoundMap.find( soundId );
    if( findIt == mSoundMap.end() )
      return;

    mAudio.PlayLoop( *( findIt->second ), 1.0f, false );
  } // CInvSoundsStorage::PlaySoundLoop

  //----------------------------------------------------------------------------------------------

  void CInvSoundsStorage::StopSound( const std::string & soundId ) const
  {
    auto findIt = mSoundMap.find( soundId );
    if( findIt == mSoundMap.end() )
      return;

    mAudio.Stop( *( findIt->second ) );
  } // CInvSoundsStorage::StopSound

  //----------------------------------------------------------------------------------------------

} // namespace Inv
