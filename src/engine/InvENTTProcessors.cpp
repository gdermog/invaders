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

  //****** processor: rendering of actors ************************************************************

  procActorRender::procActorRender( LARGE_INTEGER refTick ):
    mRefTick( refTick )
  {
  } // procActorRender::procActorRender

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
      } );              // Sprite animations are driven by tick count stored in cpGraphics component.
                        // It must not be dependent on global tick counter, because each entity starts
                        // its animations independently at random time.

  } // procActorRender::update

} // namespace Inv
