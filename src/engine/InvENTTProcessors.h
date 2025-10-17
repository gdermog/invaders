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
#include <CInvSoundsStorage.h>
#include <engine/InvENTTComponents.h>

namespace Inv
{

  using FnEventCallback_t = std::function<void( uint32_t )>;
  //!< Type definition for event callback without entity id

  using FnEventCallbackEithEntityId_t = std::function<void( entt::entity, uint32_t )>;
  //!< Type definition for event callback with entity id

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
      CInvEntityFactory & entityFactory,
      const CInvSoundsStorage & soundStorage );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    CInvEntityFactory & mEntityFactory;

    const CInvSoundsStorage & mSoundStorage;

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
      const CInvSoundsStorage & soundStorage,
      uint32_t & ammoLeft );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue );

    CInvEntityFactory & mEntityFactory;
    //<! \brief Reference to entity factory, used to create new missile entities

    const CInvSoundsStorage & mSoundStorage;
    //<! \brief Reference to sound storage, used to play shooting sound

    uint32_t & mAmmoLeft;
    //<! \brief Reference to variable containing amount of ammo left for the player ship

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
      float sceneBottomRightY,
      float & playerActX,
      float & playerActY );

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

    float & mPlayerActX;
    //<! \brief Actual X coordinate of the player ship, reference for update

    float & mPlayerActY;
    //<! \brief Actual Y coordinate of the player ship, reference for update

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
    //!< \brief Reference to current velocity of alien formation in X axis
    float & mVYGroup;
    //!< \brief Reference to current velocity of alien formation in Y axis

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
    //<! \brief Reference to collision test object, used to detect collisions between sprites

    std::set<entt::entity> mCanDamage;
    //!< List of entities that can deal damage, working variable|

    std::set<entt::entity> mCanBeDamagedAlien;
    //!< List of alien entities that can be , working variable|
    std::set<entt::entity> mCanBeDamagedPlayer;
    //!< List of player entities that can be damaged, working variable|

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

    float mSceneTopLeftX;
    //<! \brief X coordinate of top left corner of the game scene in pixels.
    float mSceneTopLeftY;
    //<! \brief Y coordinate of top left corner of the game scene in pixels.
    float mSceneBottomRightX;
    //<! \brief X coordinate of bottom right corner of the game scene in pixels.
    float mSceneBottomRightY;
    //<! \brief Y coordinate of bottom right corner of the game scene in pixels.

  }; // procActorOutOfSceneCheck


  //****** processor: garbage collector ***************************************************************


  struct procGarbageCollector: public procEnTTBase
  {
    procGarbageCollector(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      FnEventCallbackEithEntityId_t pruneCallback );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      bool allowCallbacks = true );

    FnEventCallbackEithEntityId_t mPruneCallback;
    //<! \brief Callback called when entity is pruned

    std::vector<entt::entity> mEntities;
    //<! \brief Working vector containing entities to be removed from registry, working variable

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
    //<! \brief Working structure for sorting sprites according to Z axis level

  }; // procActorRender

  //****** processor: check if the player actor is in dangerous area **********************************


  struct procPlayerInDanger: public procEnTTBase
  {
    procPlayerInDanger(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      bool & isInDangerousArea );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick );

    bool & mIsInDangerousArea;
    //<! \brief Reference to variable indicating whether the player is in dangerous area

  }; // procPlayerSpeedUpdater

} // namespace Inv

#endif
