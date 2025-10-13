;//****************************************************************************************************
//! \file InvENTTProcessorsAI.h
//! Module contains declarations of EnTT processors concernig alien behavior
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_InvENTTProcessorsAI
#define H_InvENTTProcessorsAI

#include <entity/registry.hpp>

#include <InvGlobals.h>
#include <engine/InvENTTComponents.h>
#include <engine/InvENTTProcessors.h>

namespace Inv
{


  //****** processor: setting of actors to specific states *******************************************

  /*! \brief This processor sets states of computer-controlled actors, like aliens, based on
      probabilities defined in their behavior component (firing, raiding and so on). */
  struct procActorStateSelector: public procEnTTBase
  {
    procActorStateSelector(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick );

  }; // procActorStateSelector

  //****** processor: bounds guard - aliens ************************************************

  /*! \brief Processor that guards the group of aliens against crossing the scene bounds. Also it is
      responsible for moving aliens when in formation. */
  struct procAlienBoundsGuard: public procEnTTBase
  {
    procAlienBoundsGuard(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      float & vXGroup,
      float & vYGroup,
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
      LARGE_INTEGER diffTick,
      float bottomGuardedArea );

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

  }; // procAlienBoundsGuard


} // namespace Inv

#endif
