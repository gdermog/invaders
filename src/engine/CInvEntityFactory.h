//****************************************************************************************************
//! \file CInvEntityFactory.h
//! Module declares class CInvEntityFactory, which ...
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvEntityFactory
#define H_CInvEntityFactory

#include <entity/registry.hpp>

#include <InvGlobals.h>
#include <CInvSettingsRuntime.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvSpriteStorage.h>

#define DEBUG_ID_FIGHTER  50
#define DEBUG_ID_FIGHTER_EXPLODE 100

namespace Inv
{

  class CInvGameScene;

  /*! \brief The class implements ... */
  class CInvEntityFactory
  {
  public:

    CInvEntityFactory(
      const CInvSettings & settings,
      const CInvSpriteStorage & spriteStorage,
      entt::registry & enttRegistry,
      CInvGameScene & gScene,
      CInvSettingsRuntime & settingsRuntime,
      LPDIRECT3D9 pD3D,
      LPDIRECT3DDEVICE9 pd3dDevice,
      LPDIRECT3DVERTEXBUFFER9 pVB );

    CInvEntityFactory( const CInvEntityFactory & ) = delete;
    CInvEntityFactory & operator=( const CInvEntityFactory & ) = delete;
    ~CInvEntityFactory();

    CInvSettingsRuntime & GetCInvSettingsRuntime() const { return mSettingsRuntime; }

    entt::entity AddAlienEntity(
      const std::string & entityType,
      float posX, float posY,
      float alienSizeX );
    /*!< \brief Adds a new alien entity of given type at given position.

         \param[in] entityType  Type of alien entity to be created, must correspond to a sprite ID
                                in sprite storage.
         \param[in] posX        X position of the alien entity (of centre of object) [px]
         \param[in] posY        Y position of the alien entity (of centre of object) [px]
         \param[in] alienSizeX  Width of the alien entity [px], height will be calculated according
                                to sprite aspect ratio. */

    entt::entity AddPlayerEntity(
      const std::string & entityType,
      float posX, float posY,
      float playerSizeX );

    entt::entity AddMissileEntity(
      const std::string & entityType,
      bool fromPlayer,
      float posX, float posY,
      float missileSizeX,
      float directionX = 0.0f, float directionY = 1.0f );

    entt::entity AddExplosionEntity(
      const std::string & entityType,
      float posX, float posY,
      float explosionSizeX,
      float velocityX = 0.0f, float velocityY = 0.0f );

  private:

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings object, used to access configuration parameters.

    const CInvSpriteStorage & mSpriteStorage;
    //!< \brief Reference to sprite storage object, used to access sprites.

    entt::registry & mEnTTRegistry;
    //!< EnTT registry containing all entities and components of the current simulation

    CInvSettingsRuntime & mSettingsRuntime;

    LPDIRECT3D9             mPD3D;
    //!< \brief Pointer to Direct3D interface, used for graphics operations.
    LPDIRECT3DDEVICE9       mPd3dDevice;
    //!< \brief Pointer to Direct3D device, used for rendering.
    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //!< \brief Pointer to Direct3D vertex buffer, used for rendering primitives.

    CInvGameScene &mGameScene;
    //!< \brief Reference to game scene, used to access game-wide callback functions.

  };

} // namespace Inv

#endif
