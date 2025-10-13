//****************************************************************************************************
//! \file InvENTTProcessors.h
//! Module contains basic EnTT processors declarations
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

  class CInvSprite;
  class CInvEntityFactory;
  class CInvSettings;
  class CInvSettingsRuntime;


  //****** processor: base struct for other processors ***************************************************


  struct procEnTTBase
  {
    procEnTTBase(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void reset( LARGE_INTEGER refTick );

    const CInvSettings & mSettings;

    CInvSettingsRuntime & mSettingsRuntime;

    LARGE_INTEGER mRefTick;

    bool mIsSuspended;
    //!< \brief If true, the processor does not perform any action in update() method.

  };


  //****** processor: adding entities on request of special events ***********************************


  struct procEntitySpawner: public procEnTTBase
  {
    procEntitySpawner(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      CInvEntityFactory & entityFactory );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    CInvEntityFactory & mEntityFactory;

  }; // procEntitySpawner


  //****** processor: updating speed of player actor ************************************************


  struct procPlayerSpeedUpdater: public procEnTTBase
  {
    procPlayerSpeedUpdater(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue );

  }; // procPlayerSpeedUpdater


  //****** processor: updating demads for offensive actions of player actor ****************


  struct procPlayerFireUpdater: public procEnTTBase
  {
    procPlayerFireUpdater(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      CInvEntityFactory & entityFactory,
      uint32_t & ammoLeft );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue );

    CInvEntityFactory & mEntityFactory;

    uint32_t & mAmmoLeft;

    bool mShootCommenced;
    //!< \brief True if shoot command was given in the last update. If the player holds fire button,
    //!  the ship must not fire again until the button is released and pressed again. This flag is
    //!  set to false when fire button is released.


  }; // procPlayerFireUpdater


  //****** processor: bounds guard - player ************************************************


  struct procPlayerBoundsGuard: public procEnTTBase
  {
    procPlayerBoundsGuard(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
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

    float mSceneTopLeftX;
    //!< \brief X coordinate of top left corner of the game scene in pixels.
    float mSceneTopLeftY;
    //!< \brief Y coordinate of top left corner of the game scene in pixels.
    float mSceneBottomRightX;
    //!< \brief X coordinate of bottom right corner of the game scene in pixels.
    float mSceneBottomRightY;
    //!< \brief Y coordinate of bottom right corner of the game scene in pixels.

  }; // procPlayerBoundsGuard


  //****** processor: moving of actors ***************************************************************


  struct procActorMover: public procEnTTBase
  {
    procActorMover(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      float & vXGroup,
      float & vYGroup );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    float & mVXGroup;
    float & mVYGroup;

    bool mFormationFreeze;
    //!< \brief If true, aliens in formation do not move. It is used when player is respawned.
    //!  Aliens on raid, however, returns to its position in formation freely.

  }; // procActorMover


  //****** processor: colliding of actors ***************************************************************


  class CInvCollisionTest;

  struct procCollisionDetector: public procEnTTBase
  {
    procCollisionDetector(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      CInvCollisionTest & cTest );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    CInvCollisionTest & mCTest;

    std::set<entt::entity> mCanDamage;

    std::set<entt::entity> mCanBeDamagedAlien;
    std::set<entt::entity> mCanBeDamagedPlayer;

    std::vector<std::pair<entt::entity, entt::entity>> mCollidedPairs;
                        //!< List of pairs of entities that collided in the last update. First is dangerous
                        //!  entity, second is entity that can be damaged.

  }; // procCollisionDetector


  //****** processor: searching for actor that are out of scene **************************************


  struct procActorOutOfSceneCheck: public procEnTTBase
  {
    procActorOutOfSceneCheck(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
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

  }; // procActorOutOfSceneCheck


  //****** processor: garbage collector ***************************************************************


  struct procGarbageCollector: public procEnTTBase
  {
    procGarbageCollector(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      FnEventCallbackEithEntityId_t pruneCallback );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    FnEventCallbackEithEntityId_t mPruneCallback;

    std::vector<entt::entity> mEntities;

  }; // procGarbageCollector


  //****** processor: rendering of actors ************************************************************


  struct procActorRender: public procEnTTBase
  {
    procActorRender(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    using SpriteInfo_t = struct
    {
      std::shared_ptr<CInvSprite> sprite;
      cpGraphics & gph;
      const cpPosition & pos;
      const cpGeometry & geo;
    };

    std::map<float, std::vector<SpriteInfo_t>> mZAxisSorting;

  }; // procActorRender


} // namespace Inv

#endif
