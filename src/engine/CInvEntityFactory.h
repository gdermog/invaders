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

#include <graphics/CInvSprite.h>
#include <graphics/CInvSpriteStorage.h>

namespace Inv
{

  /*! \brief The class implements ... */
  class CInvEntityFactory
  {
  public:

    CInvEntityFactory(
      const CInvSettings & settings,
      const CInvSpriteStorage & spriteStorage,
      entt::registry & enttRegistry,
      LPDIRECT3D9 pD3D,
      LPDIRECT3DDEVICE9 pd3dDevice,
      LPDIRECT3DVERTEXBUFFER9 pVB );

    CInvEntityFactory( const CInvEntityFactory & ) = delete;
    CInvEntityFactory & operator=( const CInvEntityFactory & ) = delete;
    ~CInvEntityFactory();

    void AddAlienEntity(
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

    void AddMissileEntity(
      const std::string & entityType,
      float posX, float posY,
      float missileSizeX,
      float velocityX, float velocityY );

  private:

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings object, used to access configuration parameters.

    const CInvSpriteStorage & mSpriteStorage;
    //!< \brief Reference to sprite storage object, used to access sprites.

    entt::registry & mEnTTRegistry;
    //!< EnTT registry containing all entities and components of the current simulation

    LPDIRECT3D9             mPD3D;
    //!< \brief Pointer to Direct3D interface, used for graphics operations.
    LPDIRECT3DDEVICE9       mPd3dDevice;
    //!< \brief Pointer to Direct3D device, used for rendering.
    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //!< \brief Pointer to Direct3D vertex buffer, used for rendering primitives.

    uint64_t mNextEntityId;

  };

} // namespace Inv

#endif
