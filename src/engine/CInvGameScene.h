//****************************************************************************************************
//! \file CInvGameScene.h
//! Module defines class CInvGameScene, which implements the main game scene, containing all entities
//! and processing the game logic.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvGameScene
#define H_CInvGameScene

#include <d3d9.h>

#include <CInvSettings.h>
#include <CInvSettingsRuntime.h>
#include <CInvSoundsStorage.h>

#include <entity/registry.hpp>

#include <graphics/CInvText.h>
#include <graphics/CInvBackground.h>
#include <graphics/CInvPrimitive.h>
#include <graphics/CInvSpriteStorage.h>
#include <graphics/CInvCollisionTest.h>
#include <graphics/CInvEffectSpriteBlink.h>

#include <engine/CInvEntityFactory.h>
#include <engine/InvENTTProcessors.h>
#include <engine/InvENTTProcessorsAI.h>

namespace Inv
{
  /*! \brief ... */
  class CInvGameScene
  {
  public:

    CInvGameScene(
      const CInvSettings & settings,
      CInvSettingsRuntime & settingsRuntime,
      const CInvSpriteStorage & spriteStorage,
      const CInvSoundsStorage & soundStorage,
      const CInvBackground & background,
      CInvPrimitive & primitives,
      std::map<uint32_t, AlienBossDescriptor_t> & alienBosses,
      LPDIRECT3D9 pD3D,
      LPDIRECT3DDEVICE9 pd3dDevice,
      LPDIRECT3DVERTEXBUFFER9 pVB,
      LARGE_INTEGER tickReferencePoint );

    CInvGameScene( const CInvGameScene & ) = delete;
    CInvGameScene & operator=( const CInvGameScene & ) = delete;
    ~CInvGameScene();

    bool GenerateNewScene();
    /*!< \brief Generates a new game scene, with aliens and possible other entities placed at their
         starting positions.

         \returns true if the scene was generated successfully, false otherwise. */

    bool SpawnPlayer();
    /*!< \brief Spawns the player entity at the bottom center of the scene. The player entity
         is created in "invulnerable" state. This cannot be done by procEntitySpawner, because
         there coud be a lot of "noise" around spawning the player ship (as decreasing of lives
         counter and others), so this special function is implemented. */

    bool GameOver() const { return 0u == mPlayerLivesLeft; }
    /*!< \brief Returns true if the game is over (no lives left), false otherwise. */

    uint32_t GetCurrentScore() const { return mActualScore; }
    /*!< \brief Returns current score. */

    bool RenderActualScene(
      LARGE_INTEGER actualTickPoint,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue );
    /*!< \brief Renders the actual game scene using EnTT processors chain.

         \param[in] actualTickPoint Current tick point, used to calculate game situation
         \returns false in case of error.  */

    bool PlayerEntryProcessing( LARGE_INTEGER actTick );
    /*!< \brief Processes player entry sequence, when player entity is entering the scene
         (after spawn or respawn). During this sequence, player cannot control the ship, aliens
         should not shoot at the player and special text is displayed on the screen.

         \param[in] actTick Current tick point, used to calculate game situation
         \returns false in case of error. */

    bool RenderStatusBar( LARGE_INTEGER actualTickPoint );
    /*!< \brief Renders status bar at the bottom of the scene, containing information about
          current score, number of lives left, current level etc.

          \param[in] actualTickPoint Current tick point, used to calculate game situation
          \returns false in case of error. */

    void EngineOnHold( bool onHold );
    /*!< \brief Puts the game engine on hold (true) or releases it from hold (false). When
         the engine is on hold, all entities are frozen, player cannot control his ship,
         aliens do not shoot etc. This is used during player entry sequence.

         \param[in] onHold True to put the engine on hold, false to release it from hold. */

    void Reset( LARGE_INTEGER newTickRefPoint );
    /*!< \brief Resets the game state to initial conditions, ready for a new game.

         \param[in] newTickRefPoint New reference tick point, usually current time */

    void CallbackUnsetActive( entt::entity ent, uint32_t nr );
    /*!< \brief Callback called when entity is to be unset as active, usually when its life
         ends (after dying period) or exits scene area. The entity is eliminated from the
         registry in nearest possible time. */

    void CallbackPlayerInvulnerabilityCanceled( entt::entity ent, uint32_t nr );
    /*!< \brief Callback called when player invulnerability period ends, player can be hit
         by aliens or their missiles again. */

    void CallbackAlienAnimationDone( entt::entity ent, uint32_t nr );
    /*!< \brief Callback called when alien animation is done and can be called again by random event. */

    void CallbackAlienFiringDone( entt::entity ent, uint32_t nr );
    /*!< \brief Callback called when alien firing animation is done and alien can shoot again. */

    void CallbackAlienShootRequested( entt::entity ent, uint32_t nr );
    /*!< \brief Callback called when alien requested to shoot. New missile will be generated by
         appropriate method in nearest possible time (see procEntitySpawner::update()). */

  private:

    bool EliminateEntity( entt::entity entity );
    /*!< \brief Eliminates given entity from the game scene, replacing it by appropriate explosion.
         if the eliminated entity is the player, starts sequence leading to respawn and reducing
         number of lives. This sequence is usually driven by explosion or destruction animation,
         after which the EntityJustPruned() method is called automatically. In that moment,
         eliminated entity finnaly cease to exist. */

    void EntityJustPruned( entt::entity entity, uint32_t nr );
    /*!< \brief Callback called by garbage collector when entity is actually pruned from the
         registry. If the pruned entity is the player, appropriate measures are taken (chain of
         events that leads respawn, reduce number of lives, end of game etc. is initiated) */

    void NewSwarm();
    /*!< \brief Generates new alien swarm, increases level counter and speedup factor. It is called
         when all aliens are destroyed. */

    void CalculateQuickDeathTicks();

    //------ Timing parameters --------------------------------------------------------------------------

    LARGE_INTEGER mTickReferencePoint;
    //!< \brief Reference tick point, used to calculate elapsed time during the game.

    LARGE_INTEGER mDiffTickPoint;
    //!<\brief Dummy tick difference, usually zero. It used for satisfying of arbitrary diff tick
    //!  parameter in some methods.

    //------ References to superior global objects (DI) and owned general objects ----------------------

    const CInvSettings & mSettings;
    //!< \brief Reference to global user settings

    CInvSettingsRuntime & mSettingsRuntime;
    //!< \brief Reference to global runtime settings, used to store current game state

    const CInvSpriteStorage & mSpriteStorage;
    //!< \brief Reference to sprite storage object, used to access sprites.

    const CInvSoundsStorage & mSoundStorage;
    //!< \brief Sound storage object, used to manage and access sound effects.

    const CInvBackground & mBackground;
    //!< \brief Reference to background object, used to draw the game background

    CInvPrimitive & mPrimitives;
    //!< \brief Reference to primitive drawer, used to draw basic shapes on screen

    CInvCollisionTest mCollisionTest;
    //!< \brief Class used to detect collisions between entities

    entt::registry mEnTTRegistry;
    //!< EnTT registry containing all entities and components of the current game scene

    CInvEntityFactory mEntityFactory;
    //<! \brief Entity factory, used to create game actors

    LPDIRECT3D9 mPD3D;
    //<! Direct3D interface, used to create device

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //<! Direct3D device, used to draw on screen

    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //<! Vertex buffer, used to draw primitives

    //------ Game scene parameters and state -----------------------------------------------------------

    float mSceneWidth;
    //!< \brief Width of the whole game scene in pixels.

    float mSceneHeight;
    //!< \brief Height of the whole game scene in pixels.

    float mSceneTopLeftX;
    //!< \brief X coordinate of top left corner of the game scene in pixels.

    float mSceneTopLeftY;
    //!< \brief Y coordinate of top left corner of the game scene in pixels.

    float mSceneBottomRightX;
    //!< \brief X coordinate of bottom right corner of the game scene in pixels.

    float mSceneBottomRightY;
    //!< \brief Y coordinate of bottom right corner of the game scene in pixels.

    static const float mStatusLineAreaCoefficient;
    //!< \brief Coefficient defining the area at the bottom of the scene where status line is placed

    static const float mBossAreaCoefficient;

    float mStatusLineTopLeftX;
    //!< \brief X coordinate of top left corner of the status line in pixels.

    float mStatusLineTopLeftY;
    //!< \brief Y coordinate of top left corner of the status line in pixels.

    float mStatusLineBottomRightX;
    //!< \brief X coordinate of bottom right corner of the status line in pixels.

    float mStatusLineBottomRightY;
    //!< \brief Y coordinate of bottom right corner of the status line in pixels.

    float mStatusLineHeight;
    //!< \brief Height of the status line in pixels.

    float mOneLiveIconWidth;
    //!< \brief Width of one icon representing one player life (ship) in the status line

    float mOneLiveIconHeight;
    //!< \brief Height of one icon representing one player life (ship) in the status line

    float mLivesIconsStartX;
    //!< \brief X coordinate where the lives icons start in the status line

    std::shared_ptr<CInvSprite> mLiveSprite;
    //!< \brief Sprite used to render one icon representing one player life (ship) in the
    //!  status line

    float mAmmoIconWidth;
    //!< \brief Width of one icon representing one ammo (rocket) in the status line

    float mAmmoIconHeight;
    //!< \brief Height of one icon representing one ammo (rocket) in the status line

    float mAmmoIconsStartX;
    //!< \brief X coordinate where the ammo icons start in the status line

    std::shared_ptr<CInvSprite> mAmmoSprite;
    //!< \brief Sprite used to render one icon representing one ammo (rocket) in the status

    static const float mAlienStartingAreaCoefficient;
    //!< \brief Coefficient defining the area at the top of the scene where aliens can start

    static const std::string mPlayerEntryTextAttention;
    //<! \brief Texts shown during player entry sequence (1. part)

    static const std::string mPlayerEntryTextReady;
    //!< \brief Texts shown during player entry sequence (2. part)

    static const std::string mPlayerEntryTextGo;
    //!< \brief Texts shown during player entry sequence (3. part)

    static const float mPlayerEntryTextSecond;
    //!< \brief Time in seconds during which each of the entry texts is shown

    std::unique_ptr<CInvText> mTAttention;
    //<! \brief Text objects for player entry sequence (1. part)

    std::unique_ptr<CInvText> mTReady;
    //<! \brief Text objects for player entry sequence (2. part)

    std::unique_ptr<CInvText> mTGo;
    //<! \brief Text objects for player entry sequence (3. part)

    std::shared_ptr<CInvEffectSpriteBlink> mTBlinkEffect;
    //<! \brief Blinking effect applied to entry texts

    float mPlayerEntryLetterSize;
    //<! \brief Size of letters in entry texts, calculated according to scene width

    //------ Player global state ---------------------------------------------------------------

    entt::entity mPlayerEntity;
    //<! \brief EnTT entity handle of the player ship. Used to quickly access the player's ECS
    //!  entity outside of EnTT processors.

    static const float mPlayerWidthCoefficient;
    //<! \brief Coefficient defining the width of the player ship in relation to scene width.

    float mPlayerWidth;
    //<! \brief Width of the player ship in pixels.

    float mPlayerHeight;
    //<! \brief Height of the player ship in pixels.

    float mPlayerStartX;
    //<! \brief X coordinate of the player ship starting position (bottom center of the scene)

    float mPlayerStartY;
    //<! \brief Y coordinate of the player ship starting position (bottom center of the scene)

    float mPlayerActX;
    //<! \brief Actual X coordinate of the player ship

    float mPlayerActY;
    //<! \brief Actual Y coordinate of the player ship

    bool mPlayerAlive;
    //!< \brief Flag indicating whether player entity is alive (true) or dead (false).

    bool mPlayerEntryInProgress;
    //!< \brief Flag indicating that player entity is entering the scene (after spawn or respawn).

    LARGE_INTEGER mPlayerEntryTick;
    //!< \brief Ticking when player entity started entering the scene.

    uint32_t mActualScore;
    //<! \brief Actual score of the player in the current game.

    uint32_t mPlayerLivesLeft;
    //<! \brief Number of player lives (ships) left in the current game.

    uint32_t mPlayerAmmoLeft;
    //<! \brief Number of ammo (rockets) left for the player in the current

    LONGLONG mReloadingTicks;
    //<! \brief Number of ticks needed to reload player weapon after firing.

    LONGLONG mTickLeftToReload;
    //<! \brief Number of ticks left to reload player weapon after firing.

    CInvText mScoreLabel;
    //<! \brief Text object for "SCORE" label in status line

    static const std::string mScoreText;
    //<! \brief "SCORE" label text

    float mScoreLabelTextSize;
    //<! \brief Size of letters in "SCORE" label text, calculated according to status line height

    std::string mScoreLabelBuffer;
    //<! \brief Buffer used to create "SCORE" label text

    bool mIsInDangerousArea;
    //<! \brief Flag indicating whether player is in dangerous area (above all aliens)

    uint32_t mQuickDeathTicksLeft;
    //<! \brief Number of ticks left to initiate quick death mode

    //------ Alien global state -----------------------------------------------------------------------

    float mVXGroup;
    //<! \brief Current velocity of alien group in X-axis [px/tick].

    float mVYGroup;
    //<! \brief Current velocity of alien group in Y-axis [px/tick].

    uint32_t mAliensLeft;
    //!< \brief Number of aliens still alive in the scene.

    uint32_t mAlienBossesLeft;
    //!< \brief Number of alien bosses still alive in the scene.

    std::map<uint32_t, AlienBossDescriptor_t> & mAlienBosses;
    //!< \brief Descriptors of boss aliens that can appear in the scene.

    uint32_t mLastPipBeeped;

    //------ EnTT processors --------------------------------------------------------------------------

    procGarbageCollector mProcGarbageCollector;
    procActorStateSelector mProcActorStateSelector;
    procEntitySpawner mProcEntitySpawner;
    procSpecialActorSpawner mProcSpecialActorSpawner;
    procPlayerFireUpdater mProcPlayerFireUpdater;
    procPlayerSpeedUpdater mProcPlayerSpeedUpdater;
    procPlayerBoundsGuard mProcPlayerBoundsGuard;
    procPlayerInDanger mProcPlayerInDanger;
    procAlienBoundsGuard mProcAlienBoundsGuard;
    procActorMover mProcActorMover;
    procAlienRaidDriver mProcAlienRaidDriver;
    procActorOutOfSceneCheck mProcActorOutOfSceneCheck;
    procCollisionDetector mProcCollisionDetector;
    procActorRender mProcActorRender;

  };

} // namespace Inv

#endif
