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
  //****** processor: setting of actors to specific states *******************************************

  struct procActorStateSelector
  {
    procActorStateSelector( LARGE_INTEGER refTick );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;
  };

  //****** processor: rendering of actors ************************************************************

  struct procActorRender
  {
    procActorRender( LARGE_INTEGER refTick );

    void reset( LARGE_INTEGER refTick );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

    LARGE_INTEGER mRefTick;

  };


} // namespace Inv

#endif
