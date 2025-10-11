//****************************************************************************************************
//! \file InvENTTProcessors.h
//! Module contains EnTT processors definitions
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/InvENTTProcessors.h>
#include <engine/InvENTTComponents.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvCollisionTest.h>
#include <engine/CInvEntityFactory.h>
#include <CInvSettingsRuntime.h>

namespace Inv
{


  //****** processor: setting of actors to specific states *******************************************

  procActorStateSelector::procActorStateSelector( LARGE_INTEGER refTick ):
    mRefTick( refTick )
  {
  } // procActorStateSelector::procActorStateSelector

  //--------------------------------------------------------------------------------------------------

  void procActorStateSelector::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procActorStateSelector::reset

  //--------------------------------------------------------------------------------------------------

  void procActorStateSelector::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    auto view = reg.view<const cpAlienBehave, cpAlienStatus, cpGraphics>();
    view.each( [=]( const cpAlienBehave & behave, cpAlienStatus & status, cpGraphics &gph )
    {                   // Updating status for alien actors

        if( status.isAnimating || status.isFiring || status.isDying )
          return;       // Previous status must be resolved before any other is set

        auto probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
        if( probRoll < behave.animationProbability )
        {
          status.isAnimating = true;
          gph.diffTick.QuadPart = 0ul;
          gph.standardAnimationEffect->Restore();
          return;       // Animation is started on random event. Effect is restored, runs once (as it is not
                        // continuous) and then suspends itself, sending event message by appropriate callback,
                        // which sets isAnimating flag to false again.
        } // if

        probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
        if( probRoll < behave.shootProbability )
        {
          status.isFiring = true;
          gph.diffTick.QuadPart = 0ul;
          gph.specificAnimationEffect->Restore();
          return;       // Fire animation is started on random event. Effect is restored, runs once (as it is not
                        // continuous) and then suspends itself, sending event message by appropriate callback,
                        // which sets isFiring flag to false again.
        } // if

    } );

  } // procActorStateSelector::update

  //****** processor: adding entities on request of special events ***********************************


  procEntitySpawner::procEntitySpawner(
    LARGE_INTEGER refTick,
    CInvEntityFactory & entityFactory,
    CInvSettingsRuntime & settingsRuntime ):
    mRefTick( refTick ),
    mEntityFactory( entityFactory ),
    mSettingsRuntime( settingsRuntime )
  {}

  //--------------------------------------------------------------------------------------------------

  void procEntitySpawner::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procEntitySpawner::reset

  //--------------------------------------------------------------------------------------------------

  void procEntitySpawner::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    float xTopLeft, yTopLeft;
    float xBottomRight, yBottomRight;
    float xSize, ySize;
    size_t imageIndex;

    auto view = reg.view<cpAlienStatus, const cpPosition, const cpGeometry, const cpGraphics>();
    view.each( [&]( cpAlienStatus & stat, const cpPosition & pos, const cpGeometry & geo, const cpGraphics &gph )
    {                   // Check for aliens shooting requests

        if( ! stat.isShootRequested )
          return;       // Alien did not request to shoot in this tick

        gph.standardSprite->GetResultingPosition(
          xTopLeft, yTopLeft, xBottomRight, yBottomRight, xSize, ySize, imageIndex );

        mEntityFactory.AddMissileEntity(
          "SPIT", false,
          0.5f * ( xTopLeft + xBottomRight ),
          yBottomRight - 0.15f * ySize,
          0.33f * xSize );

        stat.isShootRequested = false;
                        // Shoot request is processed
    } );

  } // procEntitySpawner::update

  //****** processor: updating speed of player actor ************************************************

  procPlayerSpeedUpdater::procPlayerSpeedUpdater(
    LARGE_INTEGER refTick,
    CInvSettingsRuntime & settingsRuntime ):
    mRefTick( refTick ),
    mSettingsRuntime( settingsRuntime )
  {
  }

  //--------------------------------------------------------------------------------------------------

  void procPlayerSpeedUpdater::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procActorMover::reset

  //--------------------------------------------------------------------------------------------------

  void procPlayerSpeedUpdater::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    ControlStateFlags_t controlState,
    ControlValue_t controlValue )
  {

    auto view = reg.view<cpPlayBehave, cpPlayStatus, cpVelocity>();
    view.each( [=]( cpPlayBehave & pos, cpPlayStatus & pstat, cpVelocity & vel )
    {
        if( pstat.isDying )
          return;       // Player is dying, no control possible

        vel.vX = 0.0f;
        vel.vY = 0.0f;
        vel.vZ = 0.0f;

        if( ControlStateHave( controlState, ControlState_t::kLeft  ) )
          vel.vX = -mSettingsRuntime.mPlayerVelocity;

        if( ControlStateHave( controlState, ControlState_t::kRight ) )
          vel.vX = mSettingsRuntime.mPlayerVelocity;

        if( ControlStateHave( controlState, ControlState_t::kUp) )
          vel.vY = -mSettingsRuntime.mPlayerVelocity;

        if( ControlStateHave( controlState, ControlState_t::kDown ) )
          vel.vY = mSettingsRuntime.mPlayerVelocity;

    } );
  } // procPlayerSpeedUpdater::update


  //****** processor: moving of actors ************************************************************

  procActorMover::procActorMover( LARGE_INTEGER refTick ):
    mRefTick( refTick )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorMover::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procActorMover::reset

  //--------------------------------------------------------------------------------------------------

  void procActorMover::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    auto view = reg.view<cpPosition, const cpVelocity>();
    view.each( [=]( cpPosition & pos, const cpVelocity & vel )
    {
        pos.X += vel.vX;
        pos.Y += vel.vY;
        pos.Z += vel.vZ;
    } );

  } // procActorMover::update


  //****** processor: colliding of actors ***************************************************************

  procCollisionDetector::procCollisionDetector( LARGE_INTEGER refTick, CInvCollisionTest & cTest ):
    mRefTick( refTick ),
    mCTest( cTest )
  {
  }

  //--------------------------------------------------------------------------------------------------

  void procCollisionDetector::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procActorMover::reset

  //--------------------------------------------------------------------------------------------------

  void procCollisionDetector::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    mCollidedPairs.clear();
    mCanDamage.clear();
    mCanBeDamagedAlien.clear();
    mCanBeDamagedPlayer.clear();

    auto viewDmg = reg.view<cpDamage, cpGraphics>();
    viewDmg.each( [&]( entt::entity entity, const auto & dmg, const auto & gph )
    {
        mCanDamage.insert( entity );
    } );

    auto viewHealth = reg.view<cpId, cpHealth, cpGraphics>();
    viewHealth.each( [&]( entt::entity entity, const auto & id, const auto & hlt, const auto & )
    {
      auto [ bAlien, sAlien ] = reg.try_get<cpAlienBehave, cpAlienStatus>( entity );
      if( nullptr != bAlien && nullptr != sAlien && ! sAlien->isDying )
        mCanBeDamagedAlien.insert( entity );

      auto [ bPlayer, sPlayer ] = reg.try_get<cpPlayBehave, cpPlayStatus>( entity );
      if( nullptr != bPlayer && nullptr != sPlayer && ! sPlayer->isDying && ! sPlayer->isInvulnerable )
        mCanBeDamagedPlayer.insert( entity );
    } );

    for( auto dangerousEntity : mCanDamage )
    {
      auto [dmgDanger, gphDanger] = reg.try_get<cpDamage, cpGraphics>( dangerousEntity );

      if( nullptr == dmgDanger || nullptr == gphDanger )
        continue;

      if( dmgDanger->dangerToAliens )
      {
        for( auto vulnerableEntity : mCanBeDamagedAlien )
        {
          if( dangerousEntity == vulnerableEntity )
            continue;
          auto [hltVulner, gphVulner] = reg.try_get<cpHealth, cpGraphics>( vulnerableEntity );
          if( nullptr == hltVulner || nullptr == gphVulner )
            continue;
          if( nullptr == gphDanger->standardSprite || nullptr == gphVulner->standardSprite )
            continue;
          if( mCTest.AreInCollision( *(gphDanger->standardSprite), *(gphVulner->standardSprite) ) )
            mCollidedPairs.push_back( { dangerousEntity, vulnerableEntity } );
        } // for
      }

      if( dmgDanger->dangerToPlayer )
      {
        for( auto vulnerableEntity : mCanBeDamagedPlayer )
        {
          if( dangerousEntity == vulnerableEntity )
            continue;
          auto [hltVulner, gphVulner] = reg.try_get<cpHealth, cpGraphics>( vulnerableEntity );
          if( nullptr == hltVulner || nullptr == gphVulner )
            continue;
          if( nullptr == gphDanger->standardSprite || nullptr == gphVulner->standardSprite )
            continue;
          if( mCTest.AreInCollision( *( gphDanger->standardSprite ), *( gphVulner->standardSprite ) ) )
            mCollidedPairs.push_back( { dangerousEntity, vulnerableEntity } );
        } // for
      } // if

    } // for


  } // procCollisionDetector::update

  //****** processor: searching for actor that are out of scene **************************************

  procActorOutOfSceneCheck::procActorOutOfSceneCheck(
    LARGE_INTEGER refTick,
    float sceneTopLeftX,
    float sceneTopLeftY,
    float sceneBottomRightX,
    float sceneBottomRightY ):
    mRefTick( refTick ),
    mSceneTopLeftX( sceneTopLeftX ),
    mSceneTopLeftY( sceneTopLeftY ),
    mSceneBottomRightX( sceneBottomRightX ),
    mSceneBottomRightY( sceneBottomRightY )
  {
  }

  //--------------------------------------------------------------------------------------------------

  void procActorOutOfSceneCheck::reset(
    LARGE_INTEGER refTick,
    float sceneTopLeftX,
    float sceneTopLeftY,
    float sceneBottomRightX,
    float sceneBottomRightY )
  {
    mRefTick = refTick;
    mSceneTopLeftX = sceneTopLeftX;
    mSceneTopLeftY = sceneTopLeftY;
    mSceneBottomRightX = sceneBottomRightX;
    mSceneBottomRightY = sceneBottomRightY;
  } // procActorOutOfSceneCheck::reset

  //--------------------------------------------------------------------------------------------------

  void procActorOutOfSceneCheck::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    auto view = reg.view<cpId, const cpPosition, const cpGeometry>();
    view.each( [=]( cpId & id, const cpPosition & pos, const cpGeometry & geo )
    {
       if( ( pos.X + 0.5f * geo.width < mSceneTopLeftX )     ||
           ( mSceneBottomRightX < pos.X - 0.5f * geo.width ) ||
           ( pos.Y + 0.5f * geo.height < mSceneTopLeftY )    ||
           ( mSceneBottomRightY < pos.Y - 0.5f * geo.height ) )
       {
         id.active = false;
                      // Entity is out of scene, remove it from registry later
       } // if

    } );
  } // procActorOutOfSceneCheck::update

  //****** processor: garbage collector ***************************************************************

  procGarbageCollector::procGarbageCollector( LARGE_INTEGER refTick, FnEventCallbackEithEntityId_t pruneCallback ):
    mPruneCallback( pruneCallback ),
    mRefTick( refTick )
  {
  }

  //--------------------------------------------------------------------------------------------------

  void procGarbageCollector::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procGarbageCollector::reset

  //--------------------------------------------------------------------------------------------------

  void procGarbageCollector::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    std::vector<entt::entity> entities;

    auto view = reg.view<cpId>();
    for( auto entity : view )
    {
      auto & entId = view.get<cpId>( entity );
      if( !entId.active )
      {                 // Entity is marked as inactive and it will be remove from registry in short time.
                        // If it should send notification on pruning, it is done now.
        if( entId.noticeOnPruning && nullptr != mPruneCallback )
          mPruneCallback( entity, entId.id );
        entities.push_back( entity );
      } // if
    }  // for

    for( auto entity : entities )
      reg.destroy( entity );
                        // Remove all entities marked as inactive

  } // procGarbageCollector::update

  //****** processor: rendering of actors ************************************************************

  procActorRender::procActorRender( LARGE_INTEGER refTick ):
    mRefTick( refTick )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorRender::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procActorRender::reset

  //--------------------------------------------------------------------------------------------------

  void procActorRender::update(
    entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {

    auto view = reg.view< cpGraphics, const cpPosition, const cpGeometry>();

    view.each( [=]( cpGraphics & gph, const cpPosition & pos, const cpGeometry & geo )
    {
        mZAxisSorting[gph.standardSprite->GetLevel()].emplace_back( gph.standardSprite, gph, pos, geo );
        gph.diffTick.QuadPart++;
    } );                // Sprite animations are driven by tick count stored in cpGraphics component.
                        // It must not be dependent on global tick counter, because each entity starts
                        // its animations independently at random time.

    for( auto & item : mZAxisSorting )
    {
      for( auto & item2 : item.second )
      {
        item2.sprite->Draw(
          item2.pos.X, item2.pos.Y,
          item2.geo.width, item2.geo.height,
          actTick, actTick, item2.gph.diffTick,
          item2.gph.staticStandardImageIndex );
      } // for

      item.second.clear();

    } // for

  } // procActorRender::update

} // namespace Inv
