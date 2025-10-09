//****************************************************************************************************
//! \file CInvGameScene.h
//! Module defines class CInvGameScene, which implements ...
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvGameScene
#define H_CInvGameScene

#include <d3d9.h>

#include <entity/registry.hpp>

#include <InvGlobals.h>

#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>
#include <graphics/CInvSprite.h>
#include <graphics/CInvSpriteStorage.h>

#include <engine/CInvEntityFactory.h>
#include <engine/InvENTTProcessors.h>

namespace Inv
{
  /*! \brief ... */
  class CInvGameScene
  {
  public:

    CInvGameScene(
      const CInvSettings & settings,
      const CInvText & textCreator,
      const CInvSpriteStorage & spriteStorage,
      CInvPrimitive & primitives,
      LPDIRECT3D9 pD3D,
      LPDIRECT3DDEVICE9 pd3dDevice,
      LPDIRECT3DVERTEXBUFFER9 pVB,
      LARGE_INTEGER tickReferencePoint );

    CInvGameScene( const CInvGameScene & ) = delete;
    CInvGameScene & operator=( const CInvGameScene & ) = delete;
    ~CInvGameScene();

    bool GenerateNewScene(
      float sceneTopLeftX, float sceneTopLeftY,
      float sceneBottomRightX, float sceneBottomRightY );

    bool RenderActualScene( LARGE_INTEGER actualTickPoint );
    /*!< \brief Renders the actual game scene.

         \param[in] actualTickPoint Current tick point, used to calculate game situation */


    void Reset( LARGE_INTEGER newTickRefPoint );
    /*!< \brief Resets the game state to initial conditions, ready for a new game.

         \param[in] newTickRefPoint New reference tick point, usually current time */

  private:


    LARGE_INTEGER mTickReferencePoint;
    //!< \brief Reference tick point, used to calculate elapsed time during the game.

    LARGE_INTEGER mDiffTickPoint;

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings

    const CInvText & mTextCreator;
    //!< \brief Reference to text creator, used to draw text on screen

    const CInvSpriteStorage & mSpriteStorage;
    //!< \brief Reference to sprite storage object, used to access sprites.

    CInvPrimitive & mPrimitives;
    //!< \brief Reference to primitive drawer, used to draw basic shapes on screen

    entt::registry mEnTTRegistry;
    //!< EnTT registry containing all entities and components of the current game scene

    CInvEntityFactory mEntityFactory;
    //<! \brief Entity factory, used to create game actors

    LPDIRECT3D9             mPD3D;
    //<! Direct3D interface, used to create device

    LPDIRECT3DDEVICE9       mPd3dDevice;
    //<! Direct3D device, used to draw on screen

    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //<! Vertex buffer, used to draw primitives

    float mSceneWidth;
    //!< \brief Width of the whole game scene in pixels.
    float mSceneHeight;
    //!< \brief Height of the whole game scene in pixels.

    float mSceneTopLeftX;
    //!< \brief X coordinate of top left corner of the game scene in pixels.
    float mSceneTopLeftY;
    //!< \brief Y coordinate of top left corner of the game scene in pixels.
    float mSceneBottomRightX;
    //!< \brief X coordinate of bottom right corner of the game scene in pixels.
    float mSceneBottomRightY;
    //!< \brief Y coordinate of bottom right corner of the game scene in pixels.

    float mAlienStartingAreaCoefficient;
    //!< \brief Coefficient defining the area at the top of the scene where aliens can start

    procActorStateSelector mProcActorStateSelector;
    procEntitySpawner mProcEntitySpawner;
    procActorMover mProcActorMover;
    procActorOutOfSceneCheck mProcActorOutOfSceneCheck;
    procGarbageCollector mProcGarbageCollector;
    procActorRender mProcActorRender;


  };

} // namespace Inv

#endif
