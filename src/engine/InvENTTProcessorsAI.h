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
#include <engine/CInvEntityFactory.h>

namespace Inv
{

  //****** processor: setting of actors to specific states *******************************************

  /*! \brief This processor sgenerates special actors (alien bosses and so on) according to
      random events. */
  struct procSpecialActorSpawner: public procEnTTBase
  {
    procSpecialActorSpawner(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      CInvEntityFactory & entityFactory,
      const CInvSoundsStorage & soundStorage,
      uint32_t & aliensLeft,
      uint32_t & alienBossesLeft,
      std::map<uint32_t, AlienBossDescriptor_t> & bossDescriptor );


    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      float playerYPos,
      uint32_t quickDeathTicksLeft );

    CInvEntityFactory & mEntityFactory;

    const CInvSoundsStorage & mSoundStorage;

    uint32_t &mAliensLeft;

    uint32_t & mAlienBossesLeft;

    std::map<uint32_t, AlienBossDescriptor_t> & mBossDescriptor;

  }; // procActorStateSelector

  //****** processor: setting of actors to specific states *******************************************

  /*! \brief This processor sets states of computer-controlled actors, like aliens, based on
      probabilities defined in their behavior component (firing, raiding and so on). */
  struct procActorStateSelector: public procEnTTBase
  {
    procActorStateSelector(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      bool & isInDangerousArea);

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      uint32_t quickDeathTicksLeft );

    bool & mIsInDangerousArea;

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

  //****** processor: setting of actors to specific states *******************************************

  /*! \brief This processor handles behavior of aliens that are presently on raid or returning
      from raid into the formation. */
  struct procAlienRaidDriver: public procEnTTBase
  {
    procAlienRaidDriver(
      LARGE_INTEGER refTick,
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime );

    void update(
      entt::registry & reg,
      LARGE_INTEGER actTick,
      LARGE_INTEGER diffTick,
      uint32_t quickDeathTicksLeft );


  }; // procAlienRaidDriver


} // namespace Inv

#endif
