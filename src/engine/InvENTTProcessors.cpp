//****************************************************************************************************
//! \file InvENTTProcessors.h
//! Module contains EnTT processors definitions
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/InvENTTProcessors.h>

#include <graphics/CInvPrimitive.h>
#include <graphics/CInvSprite.h>
#include <graphics/CInvSpriteStorage.h>

#include <engine/CInvEntityFactory.h>

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
    {
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
          gph.firingAnimationEffect->Restore();
          return;       // Fire animation is started on random event. Effect is restored, runs once (as it is not
                        // continuous) and then suspends itself, sending event message by appropriate callback,
                        // which sets isFiring flag to false again.
        } // if

    } );

  } // procActorStateSelector::update

  //****** processor: adding entities on request of special events ***********************************


  procEntitySpawner::procEntitySpawner( LARGE_INTEGER refTick, CInvEntityFactory & entityFactory ):
    mRefTick( refTick ),
    mEntityFactory( entityFactory )
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
          "SPIT",
          0.5f * ( xTopLeft + xBottomRight ),
          yBottomRight - 0.15f * ySize,
          0.33f * xSize,
          0.0f,
          /*SPEED*/ 1.0f );

        stat.isShootRequested = false;
                        // Shoot request is processed
    } );

  } // procEntitySpawner::update

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
    auto view = reg.view< cpId, const cpPosition, const cpGeometry>();
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

  procGarbageCollector::procGarbageCollector( LARGE_INTEGER refTick ):
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
      if( ! entId.active )
        entities.push_back( entity );
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
      gph.standardSprite->Draw(
        pos.X, pos.Y,
        geo.width, geo.height,
        actTick, actTick, gph.diffTick,
        gph.staticStandardImageIndex );
      gph.diffTick.QuadPart++;
    } );                // Sprite animations are driven by tick count stored in cpGraphics component.
                        // It must not be dependent on global tick counter, because each entity starts
                        // its animations independently at random time.

  } // procActorRender::update

} // namespace Inv
