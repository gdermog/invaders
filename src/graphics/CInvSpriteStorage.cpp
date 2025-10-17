//****************************************************************************************************
//! \file CInvSpriteStorage.cpp
//! Module defines class CInvSpriteStorage, which represent a storage of sprites used in the game.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <filesystem>

#include <CInvLogger.h>
#include <InvStringTools.h>
#include <graphics/CInvSpriteStorage.h>

namespace Inv
{

  static const std::string lModLogId( "Text" );

  CInvSpriteStorage::CInvSpriteStorage(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice ):
    mSettings( settings ),
    mPd3dDevice( pd3dDevice )
  {
  } // CInvSpriteStorage::CInvSpriteStorage

  //----------------------------------------------------------------------------------------------

  CInvSpriteStorage::~CInvSpriteStorage() = default;

  //----------------------------------------------------------------------------------------------

  std::shared_ptr<CInvSprite> CInvSpriteStorage::AddSprite(
    const std::string & spriteId,
    const std::string & spriteRelPath )
  {
    auto findIt = mSpriteMap.find( spriteId );
    if( findIt != mSpriteMap.end() )
    {
      LOG << "Warning: Sprite with ID '" << spriteId << "' already exists, returning existing one.";
      return findIt->second;
    } // if

    auto newSprite = std::make_shared<CInvSprite>( mSettings, mPd3dDevice );
    newSprite->AddMultipleSpriteImages( "sprites/" + spriteRelPath + "/%03u.png");

    mSpriteMap[spriteId] = newSprite;
    return newSprite;

  } // CInvSpriteStorage::AddSprite

  //----------------------------------------------------------------------------------------------

  std::unique_ptr<CInvSprite> CInvSpriteStorage::GetSprite( const std::string & spriteId ) const
  {
    auto findIt = mSpriteMap.find( spriteId );
    if( findIt == mSpriteMap.end() )
      return nullptr;

    return std::make_unique<CInvSprite>( *findIt->second );
                        // Shallow copy is returned, textures in device have same references

  } // CInvSpriteStorage::GetSprite

  //----------------------------------------------------------------------------------------------


} // namespace Inv
