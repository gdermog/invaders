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
    //<! \brief Reference to entity factory, used to create new special actor entities

    const CInvSoundsStorage & mSoundStorage;
    //<! \brief Reference to sound storage, used to play boss running sounds

    uint32_t &mAliensLeft;
    //<! \brief Reference to variable containing number of aliens left in the scene

    uint32_t & mAlienBossesLeft;
    //<! \brief Reference to variable containing number of alien bosses left in the scene

    std::map<uint32_t, AlienBossDescriptor_t> & mBossDescriptor;
    //<! \brief Reference to map of alien boss descriptors, used to determine spawning parameters

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
    //<! \brief Reference to variable indicating whether the player is in dangerous area

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
    //!< \brief Reference to current velocity of alien formation in X axis
    float & mVYGroup;
    //!< \brief Reference to current velocity of alien formation in Y axis

    uint32_t mYGroupTranslationCounter;
    //!< \brief Counter of ticks left for downward translation of alien formation

    bool mTranslatingDown;
    //!< \brief Flag indicating whether alien formation is presently translating downwards
    float mNextVXGroup;
    //!< \brief Next horizontal velocity of alien formation after downward translation is finished

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
