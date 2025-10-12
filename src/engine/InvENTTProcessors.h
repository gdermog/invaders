//****************************************************************************************************
//! \file InvENTTProcessors.h
//! Module contains EnTT processors declarations
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_InvENTTProcessors
#define H_InvENTTProcessors

#include <entity/registry.hpp>

#include <InvGlobals.h>

#include <engine/InvENTTComponents.h>

namespace Inv
{
  using FnEventCallback_t = std::function<void( uint32_t )>;

  using FnEventCallbackEithEntityId_t = std::function<void( entt::entity, uint32_t )>;

#define BIND_MEMBER_EVENT_CALLBACK( ref, fnName )  std::bind( &fnName, (ref), std::placeholders::_1, std::placeholders::_2 )
  //!< Macro to simplify binding of member function as event callback

#define BIND_MEMBER_EVENT_CALLBACK_ON( ref, fnName, ent )  std::bind( &fnName, (ref), (ent), std::placeholders::_1 )
  //!< Macro to simplify binding of member function as event callback, with expects entity passed as first argument


  class CInvEntityFactory;
  class CInvSettings;
  class CInvSettingsRuntime;

  //****** processor: setting of actors to specific states *******************************************

  struct procActorStateSelector
  {
    procActorStateSelector( LARGE_INTEGER refTick );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;
  };

  //****** processor: adding entities on request of special events ***********************************

  struct procEntitySpawner
  {
    procEntitySpawner(
      LARGE_INTEGER refTick,
      CInvEntityFactory & entityFactory,
      CInvSettingsRuntime & settingsRuntime );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;
    CInvEntityFactory & mEntityFactory;
    CInvSettingsRuntime & mSettingsRuntime;
  };

  //****** processor: updating speed of player actor ************************************************

  struct procPlayerSpeedUpdater
  {
    procPlayerSpeedUpdater(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void reset( LARGE_INTEGER refTick );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue );

    LARGE_INTEGER mRefTick;
    const CInvSettings & mSettings;
    CInvSettingsRuntime & mSettingsRuntime;
  };

  //****** processor: updating demads for offensive actions of player actor ****************

  struct procPlayerFireUpdater
  {
    procPlayerFireUpdater(
      LARGE_INTEGER refTick,
      CInvEntityFactory & entityFactory,
      CInvSettingsRuntime & settingsRuntime );

    void reset( LARGE_INTEGER refTick );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue );

    LARGE_INTEGER mRefTick;
    CInvEntityFactory & mEntityFactory;
    CInvSettingsRuntime & mSettingsRuntime;

    bool mShootCommenced;
    //!< \brief True if shoot command was given in the last update. If the player holds fire button,
    //!  the ship must not fire again until the button is released and pressed again. This flag is
    //!  set to false when fire button is released.

    bool mCanShoot;
    //!< \brief True if player can shoot now, false if not. It is set to true by game scene
    //!  according to available rockets (ammo).

  }; // procPlayerFireUpdater

  //****** processor: bounds guard - player ************************************************

  struct procPlayerBoundsGuard
  {
    procPlayerBoundsGuard(
      LARGE_INTEGER refTick,
      float sceneTopLeftX,
      float sceneTopLeftY,
      float sceneBottomRightX,
      float sceneBottomRightY );

    void reset(
      LARGE_INTEGER refTick,
      float sceneTopLeftX,
      float sceneTopLeftY,
      float sceneBottomRightX,
      float sceneBottomRightY );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

    float mSceneTopLeftX;
    //!< \brief X coordinate of top left corner of the game scene in pixels.
    float mSceneTopLeftY;
    //!< \brief Y coordinate of top left corner of the game scene in pixels.
    float mSceneBottomRightX;
    //!< \brief X coordinate of bottom right corner of the game scene in pixels.
    float mSceneBottomRightY;
    //!< \brief Y coordinate of bottom right corner of the game scene in pixels.
  };

  //****** processor: bounds guard - aliens ************************************************

  struct procAlienBoundsGuard
  {
    procAlienBoundsGuard(
      LARGE_INTEGER refTick,
      float & vXGroup,
      float & vYGroup,
      float sceneTopLeftX,
      float sceneTopLeftY,
      float sceneBottomRightX,
      float sceneBottomRightY,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void reset(
      LARGE_INTEGER refTick,
      float sceneTopLeftX,
      float sceneTopLeftY,
      float sceneBottomRightX,
      float sceneBottomRightY );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      float bottomGuardedArea );

    LARGE_INTEGER mRefTick;

    float & mVXGroup;
    float & mVYGroup;

    uint32_t mYGroupTranslationCounter;

    bool mTranslatingDown;
    float mNextVXGroup;

    float mSceneTopLeftX;
    //!< \brief X coordinate of top left corner of the game scene in pixels.
    float mSceneTopLeftY;
    //!< \brief Y coordinate of top left corner of the game scene in pixels.
    float mSceneBottomRightX;
    //!< \brief X coordinate of bottom right corner of the game scene in pixels.
    float mSceneBottomRightY;
    //!< \brief Y coordinate of bottom right corner of the game scene in pixels.

    const CInvSettings & mSettings;
    CInvSettingsRuntime & mSettingsRuntime;
  };


  //****** processor: moving of actors ***************************************************************

  struct procActorMover
  {
    procActorMover(
      LARGE_INTEGER refTick,
      float & vXGroup,
      float & vYGroup,
      CInvSettingsRuntime & settingsRuntime );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

    float & mVXGroup;
    float & mVYGroup;

    CInvSettingsRuntime & mSettingsRuntime;

  };

  //****** processor: colliding of actors ***************************************************************

  class CInvCollisionTest;

  struct procCollisionDetector
  {
    procCollisionDetector( LARGE_INTEGER refTick, CInvCollisionTest &cTest );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

    CInvCollisionTest & mCTest;

    std::set<entt::entity> mCanDamage;

    std::set<entt::entity> mCanBeDamagedAlien;
    std::set<entt::entity> mCanBeDamagedPlayer;

    std::vector<std::pair<entt::entity, entt::entity>> mCollidedPairs;
                        //!< List of pairs of entities that collided in the last update. First is dangerous
                        //!  entity, second is entity that can be damaged.

  };

  //****** processor: searching for actor that are out of scene **************************************

  struct procActorOutOfSceneCheck
  {
    procActorOutOfSceneCheck(
      LARGE_INTEGER refTick,
      float sceneTopLeftX,
      float sceneTopLeftY,
      float sceneBottomRightX,
      float sceneBottomRightY );

    void reset(
      LARGE_INTEGER refTick,
      float sceneTopLeftX,
      float sceneTopLeftY,
      float sceneBottomRightX,
      float sceneBottomRightY );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

    float mSceneTopLeftX;
    float mSceneTopLeftY;
    float mSceneBottomRightX;
    float mSceneBottomRightY;

  };

  //****** processor: garbage collector ***************************************************************

  struct procGarbageCollector
  {
    procGarbageCollector( LARGE_INTEGER refTick, FnEventCallbackEithEntityId_t pruneCallback );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

    FnEventCallbackEithEntityId_t mPruneCallback;

  };

  //****** processor: rendering of actors ************************************************************

  class CInvSprite;

  struct procActorRender
  {
    procActorRender( LARGE_INTEGER refTick );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

    using SpriteInfo_t = struct
    {
      std::shared_ptr<CInvSprite> sprite;
      cpGraphics & gph;
      const cpPosition & pos;
      const cpGeometry & geo;
    };

    std::map<float, std::vector<SpriteInfo_t>> mZAxisSorting;

  };


} // namespace Inv

#endif
