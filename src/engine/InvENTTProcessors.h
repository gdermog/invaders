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


  //****** processor: setting of actors to specific states *******************************************

  struct procActorStateSelector
  {
    procActorStateSelector( LARGE_INTEGER refTick );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;
  };

  //****** processor: adding entities on request of special events ***********************************

  class CInvEntityFactory;

  struct procEntitySpawner
  {
    procEntitySpawner( LARGE_INTEGER refTick, CInvEntityFactory & entityFactory );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;
    CInvEntityFactory & mEntityFactory;
  };

  //****** processor: moving of actors ***************************************************************

  struct procActorMover
  {
    procActorMover( LARGE_INTEGER refTick );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;
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
