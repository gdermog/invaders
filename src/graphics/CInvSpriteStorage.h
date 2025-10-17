//****************************************************************************************************
//! \file CInvSpriteStorage.h
//! Module declares class CInvSpriteStorage, which represent a storage of sprites used in the game.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvSpriteStorage
#define H_CInvSpriteStorage

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvSprite.h>


namespace Inv
{

  /*! \brief The class represent a storage of sprites used in the game. CInvSpriteStorage manages
      a collection of CInvSprite objects, allowing for easy addition and retrieval of sprites by
      their unique IDs. It handles the loading of sprite images from specified file paths and
      ensures that each sprite is properly initialized for use in the game. */
  class CInvSpriteStorage
  {
    public:

    CInvSpriteStorage(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice);

    CInvSpriteStorage( const CInvSpriteStorage & ) = delete;
    CInvSpriteStorage & operator=( const CInvSpriteStorage & ) = delete;
    ~CInvSpriteStorage();

    std::shared_ptr<CInvSprite> AddSprite(
      const std::string & spriteId,
      const std::string & spriteRelPath );
    /*!< \brief Adds a new sprite to the storage, loading images from given relative path.
         Returns reference to object representing the sprite stored in the CInvSpriteStorage
         class (so some additional adjustments are possible).

         \param[in] spriteId      ID of the sprite, used to retrieve it later
         \param[in] spriteRelPath Relative path to directory containing sprite images,
                                  relative to settings image path. Images are expected
                                  to be named as 001.png, 002.png, ... up to the first
                                  missing number. Example: "alien1" will load
                                  images "sprites/alien1/001.png", "sprites/alien1/002.png", ... */

    std::unique_ptr<CInvSprite> GetSprite( const std::string & spriteId ) const;
    /*!< \brief Returns copy of sprite with given ID, or nullptr if no such sprite exists.

         \param[in] spriteId ID of the sprite to be retrieved.
         \returns Copy of sprite with given ID, or nullptr if no such sprite exists.
                  Only CInvSprite is copied, textures in device have same references. */

  private:

    const CInvSettings & mSettings;
    //<! Reference to settings object, all parameters are taken from here

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //<! Direct3D device, used to set and get scissor rectangle

    std::map<std::string, std::shared_ptr<CInvSprite>> mSpriteMap;
    //<! Map of sprites stored in this class

  };

} // namespace Inv

#endif
