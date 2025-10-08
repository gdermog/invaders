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

  //****** processor: rendering of actors ************************************************************

  procActorRender::procActorRender( LARGE_INTEGER refTick ):
    mRefTick( refTick )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorRender::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  }

  //--------------------------------------------------------------------------------------------------

  void procActorRender::update(
    entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    auto view = reg.view<const cpGraphics, const cpPosition, const cpGeometry>();

    view.each( [=]( const cpGraphics & gph, const cpPosition & pos, const cpGeometry & geo )
    {
      gph.standardSprite->Draw(
        pos.X, pos.Y,
        geo.width, geo.height,
        mRefTick, actTick, diffTick,
        gph.staticStandardImageIndex );
    });

  };



} // namespace Inv
