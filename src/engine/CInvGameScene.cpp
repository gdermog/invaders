//****************************************************************************************************
//! \file CInvGameScene.cpp
//! Module declares class CInvGameScene, which implements the main game scene, containing all entities
//! and processing the game logic.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/CInvGameScene.h>
#include <engine/InvENTTComponents.h>

#include <graphics/CInvSprite.h>

#include <CInvLogger.h>

namespace Inv
{

  static const std::string lModLogId( "GAMESCENE" );

  const float CInvGameScene::mBossAreaCoefficient = 0.1f;
  const float CInvGameScene::mStatusLineAreaCoefficient = 0.925f;
  const float CInvGameScene::mAlienStartingAreaCoefficient = 0.65f;
  const std::string CInvGameScene::mPlayerEntryTextAttention = "ATTENTION";
  const std::string CInvGameScene::mPlayerEntryTextReady = "READY";
  const std::string CInvGameScene::mPlayerEntryTextGo = "GO";
  const float CInvGameScene::mPlayerEntryTextSecond = 1.5f;
  const float CInvGameScene::mPlayerWidthCoefficient = 0.1f;
  const std::string CInvGameScene::mScoreText = "SCORE ";

  //**************************************************************************************************

  CInvGameScene::CInvGameScene(
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    const CInvSpriteStorage & spriteStorage,
    CInvPrimitive & primitives,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER tickReferencePoint ):

    //------ Timing parameters --------------------------------------------------------------------------

    mTickReferencePoint( tickReferencePoint ),
    mDiffTickPoint{ 0 },

    //------ References to superior global objects (DI) and owned genral objects -----------------------

    mSettings( settings ),
    mSettingsRuntime( settingsRuntime ),
    mSpriteStorage( spriteStorage ),
    mPrimitives( primitives ),
    mCollisionTest( settings, pd3dDevice ),
    mEnTTRegistry(),
    mEntityFactory( settings, spriteStorage, mEnTTRegistry, *this, mSettingsRuntime, pD3D, pd3dDevice, pVB ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),

    //------ Game scene parameters and state -----------------------------------------------------------

    mSceneWidth( (float)settings.GetWidth() ),
    mSceneHeight( (float)settings.GetHeight() * mStatusLineAreaCoefficient ),
    mSceneTopLeftX( 0.0f ),
    mSceneTopLeftY( 0.0f ),
    mSceneBottomRightX( (float)settings.GetWidth() ),
    mSceneBottomRightY( (float)settings.GetHeight() * mStatusLineAreaCoefficient ),
    mStatusLineTopLeftX( 0.0f ),
    mStatusLineTopLeftY( (float)settings.GetHeight() * mStatusLineAreaCoefficient ),
    mStatusLineBottomRightX( (float)settings.GetWidth() ),
    mStatusLineBottomRightY( (float)settings.GetHeight() ),
    mStatusLineHeight( (float)settings.GetHeight() * ( 1.0 - mStatusLineAreaCoefficient ) ),
    mOneLiveIconWidth( 0.0f ),
    mOneLiveIconHeight( 0.0f ),
    mLivesIconsStartX( 0.0f ),
    mLiveSprite( spriteStorage.GetSprite( "LIVE" ) ),
    mAmmoIconWidth( 0.0f ),
    mAmmoIconHeight( 0.0f ),
    mAmmoIconsStartX( 0.0f ),
    mAmmoSprite( spriteStorage.GetSprite( "AMMO" ) ),
    mTAttention( nullptr ),
    mTReady( nullptr ),
    mTGo( nullptr ),
    mTBlinkEffect( nullptr ),
    mPlayerEntryLetterSize( 40.0f ),
    mScoreLabel( {}, settings, pd3dDevice ),
    mScoreLabelBuffer(),

    //------ Player global state ---------------------------------------------------------------

    mPlayerEntity{},
    mPlayerWidth( 0.0f ),
    mPlayerHeight( 0.0f ),
    mPlayerStartX( 0.0f ),
    mPlayerStartY( 0.0f ),
    mPlayerEntryInProgress( false ),
    mPlayerEntryTick{ 0 },
    mActualScore( 0 ),
    mPlayerAlive( false ),
    mPlayerLivesLeft( 0 ),
    mPlayerAmmoLeft( 0 ),
    mReloadingTicks( 0 ),
    mTickLeftToReload( 0 ),

    //------ Alien (group) global state ---------------------------------------------------------------

    mVXGroup( 0.0f ),
    mVYGroup( 0.0f ),
    mAliensLeft( 0 ),
    mSaucerSize( 0.0f ),
    mSaucerSpawnY( 0.0f ),
    mSaucerSpawnXLeft( 0.0f ),
    mSaucerSpawnXRight( 0.0f ),

    //------ EnTT processors --------------------------------------------------------------------------

#define PROCCMN  tickReferencePoint, settings, settingsRuntime

    mProcGarbageCollector     ( PROCCMN, BIND_MEMBER_EVENT_CALLBACK( this, CInvGameScene::EntityJustPruned ) ),
    mProcActorStateSelector   ( PROCCMN ),
    mProcEntitySpawner        ( PROCCMN, mEntityFactory ),
    mProcSpecialActorSpawner  ( PROCCMN, mEntityFactory, mAliensLeft, 0.0f, 0.0f, 0.0f, 0.0f ),
    mProcActorMover           ( PROCCMN, mVXGroup, mVYGroup ),
    mProcAlienRaidDriver      ( PROCCMN ),
    mProcPlayerFireUpdater    ( PROCCMN, mEntityFactory, mPlayerAmmoLeft ),
    mProcPlayerSpeedUpdater   ( PROCCMN ),
    mProcPlayerBoundsGuard    ( PROCCMN, 0.0f, 0.0f, (float)settings.GetWidth(), (float)settings.GetHeight() ),
    mProcAlienBoundsGuard     ( PROCCMN, mVXGroup, mVYGroup, 0.0f, 0.0f, (float)settings.GetWidth(), (float)settings.GetHeight() ),
    mProcActorOutOfSceneCheck ( PROCCMN, 0.0f, 0.0f, (float)settings.GetWidth(), (float)settings.GetHeight() ),
    mProcCollisionDetector    ( PROCCMN, mCollisionTest ),
    mProcActorRender          ( PROCCMN )
  {}

  //-------------------------------------------------------------------------------------------------

  CInvGameScene::~CInvGameScene() = default;

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::GenerateNewScene()
  {
    auto maxLetters = max( max(
      mPlayerEntryTextAttention.size(),
      mPlayerEntryTextReady.size() ),
      mPlayerEntryTextGo.size() );
    mPlayerEntryLetterSize = mSceneWidth * 0.5f / (float)maxLetters;
                        // Attention-ready-go text size is set in such way that the longest text
                        // ("attention" by default) takes half of the scene width.

    uint32_t textBlinkPace = (uint32_t)( (float)mSettings.GetTickPerSecond() * ( mPlayerEntryTextSecond / 6.0f ) ) + 1;
    mTBlinkEffect = std::make_shared<CInvEffectSpriteBlink>( mSettings, mPd3dDevice, 1u );
    mTBlinkEffect->SetPace( textBlinkPace );
    mTBlinkEffect->SetIgnoreDiffTick( true );
    mTBlinkEffect->SetContinuous( true );
                        // Each text from attention-ready-go will blink 3 times during its display time

    mTAttention = std::make_unique<CInvText>( mPlayerEntryTextAttention, mSettings, mPd3dDevice );
    mTAttention->AddEffect( mTBlinkEffect );
    mTReady = std::make_unique<CInvText>( mPlayerEntryTextReady, mSettings, mPd3dDevice );
    mTReady->AddEffect( mTBlinkEffect );
    mTGo = std::make_unique<CInvText>( mPlayerEntryTextGo, mSettings, mPd3dDevice );
    mTGo->AddEffect( mTBlinkEffect );
                        // Attention-ready-go texts are created with blinking effect on them. Texts will
                        // be displayed during player entity entry into the scene.

    std::vector<std::pair<uint32_t, std::string>> alienRows =
    {                   // Default alien setup. THis can be changed in future versions, generated randomly,
                        // prescribed as series of defferent scenes and so on.
      { 10, "PINK" },
      {  9, "PINK" },
      { 10, "PINK" },
      {  9, "PINK" },
      { 10, "PINK" },
    };

    auto alienAreaHeight = mSceneHeight * mAlienStartingAreaCoefficient;
                        // Space for placing the alien swarm is limited to upper part of the scene

    uint32_t maxAliens = 0;
    for( const auto & ar : alienRows )
    {                   // Find the maximum number of aliens in a single row
      if( maxAliens < ar.first )
        maxAliens = ar.first;
    } // for

    auto alienWidth = mSceneWidth / ( 1.60f * (float)maxAliens );
    auto spaceInBetween = 0.40f * alienWidth;
                        // Alien width is set in such way that the maximum number of aliens in a row
                        // fits into specific part of the scene width, rest is space in between aliens
                        // and white space at the sides of the scene.
                        //
    auto bossSprite = mSpriteStorage.GetSprite( "SAUCER" );
    auto bossSize = bossSprite->GetImageSize( 0 );
    auto aspectRatio = (float)bossSize.second / (float)bossSize.first;
    mSaucerSize = aspectRatio * mSceneHeight * mBossAreaCoefficient;
    mSaucerSpawnY = mSceneTopLeftY + mSaucerSize * 0.50f;
    mSaucerSpawnXLeft = -mSaucerSize * 0.49f;
    mSaucerSpawnXRight = mSceneBottomRightX + mSaucerSize * 0.49f;
                        // Saucer (boss alien) size is set to fit into the boss area, its spawn
                        // position is just outside of the scene at the top, both on left and
                        // right side.

    mVXGroup = 0.0f;
    mVYGroup = 0.0f;
    mAliensLeft = 0;    // No aliens on the scene yet and alien group is not moving

    uint32_t rowIndex = 0;
    for( auto & ar : alienRows )
    {                   // Generate aliens row by row
      auto baseSprite = mSpriteStorage.GetSprite( ar.second );
      if( nullptr == baseSprite )
        continue;

      auto baseSize = baseSprite->GetImageSize( 0 );
      auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
      auto alienHeight = alienWidth * aspectRatio;
      auto yPos = mSceneTopLeftY + mSceneHeight * mBossAreaCoefficient
                 + ( alienHeight * 1.2f ) * (float)rowIndex + alienHeight * 0.5f;

      auto spaceTakenByAliens = alienWidth * (float)ar.first;
      auto spaceTakenByRow = spaceTakenByAliens + spaceInBetween * ( (uint32_t)ar.first - 1u );

      auto xPos = mSceneTopLeftX + ( mSceneWidth - spaceTakenByRow ) * 0.5f + alienWidth * 0.5f;
      for( uint32_t i = 0; i < ar.first; ++i )
      {
        mEntityFactory.AddAlienEntity( ar.second, xPos, yPos, 0.0f, 0.0f, alienWidth );
        xPos += alienWidth + spaceInBetween;
        ++mAliensLeft;
      } // for

      ++rowIndex;

    } // for

    if( nullptr != mLiveSprite )
    {
      auto baseSize = mLiveSprite->GetImageSize( 0 );
      auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
      mOneLiveIconHeight = mStatusLineHeight * 0.9f;
      mOneLiveIconWidth = mOneLiveIconHeight / aspectRatio;
      mLivesIconsStartX = mStatusLineBottomRightX - mOneLiveIconWidth * mSettings.GetInitialLives() * 1.05f;
    } // if

    if( nullptr != mAmmoSprite )
    {
      auto baseSize = mAmmoSprite->GetImageSize( 0 );
      auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
      mAmmoIconHeight = mStatusLineHeight * 0.9f;
      mAmmoIconWidth = mStatusLineHeight / aspectRatio;
      mAmmoIconsStartX =
        ( IsZero( mLivesIconsStartX ) ? mStatusLineBottomRightX : mLivesIconsStartX ) -
        mAmmoIconWidth * 0.5f * mSettings.GetAmmo() - mOneLiveIconWidth;
    } // if

    mScoreLabelTextSize = mStatusLineHeight * 0.6f;

    return true;

  } // CInvGameScene::GenerateNewScene

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::SpawnPlayer()
  {
    auto baseSprite = mSpriteStorage.GetSprite( "FIGHT" );
    if( nullptr == baseSprite )
      return false;

    mPlayerWidth = mSceneWidth * mPlayerWidthCoefficient;

    auto baseSize = baseSprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mPlayerHeight = mPlayerWidth * aspectRatio;

    mPlayerStartX = mSceneTopLeftX + mSceneWidth * 0.5f;
    mPlayerStartY = mSceneBottomRightY - mPlayerHeight * 0.5f;

    mPlayerEntity =
      mEntityFactory.AddPlayerEntity( "FIGHT", mPlayerStartX, mPlayerStartY, mPlayerWidth );

    mPlayerAlive = true;

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* DOOMED! Player dies at the start of the game for debugging purposes    */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
// mEntityFactory.AddMissileEntity(
//   "SPIT", false,
//   mPlayerStartX - 0.25f * playerWidth,
//   mPlayerStartY - 3.0f * playerHeight,
//   0.33f * playerWidth );
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* QUICKSHOT! Player fires at the start of the game for debugging purposes*/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
//  mEntityFactory.AddMissileEntity(
//    "ROCKET", true, mPlayerStartX, mPlayerStartY, 0.1f * playerWidth, 0.0f, -1.0f );
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* BOSS! Boss is spawned at the start of the game for debugging purposes  */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
//  mEntityFactory.AddAlienBossEntity(
//    "SAUCER",
//    35.0f - mSceneWidth * mBossAreaCoefficient,
//    //mSceneWidth * mBossAreaCoefficient * 0.5f,
//    mPlayerStartY - 1.5f * mPlayerHeight,
//    1.0, 0.0f,
//    mSceneWidth * mBossAreaCoefficient );
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* RAID! Several invaders are raiding at the start of the game for debug- */
/* ging purposes                                                          */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
    uint32_t raiders = 2;
    auto viewA = mEnTTRegistry.view<cpAlienBehave, cpAlienStatus, cpPosition, cpVelocity, cpGeometry>();
    viewA.each( [&]( cpAlienBehave & pBehave, cpAlienStatus & pStat, cpPosition & pPos, cpVelocity & pVel, cpGeometry & pGeo )
    {
      if( 0u < raiders )
      {
        pStat.isInRaid = true;
        pStat.raidTicksLeft = UINT32_MAX;
        --raiders;
      }
    } );
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

    return true;

  } // CInvGameScene::SpawnPlayer

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::RenderActualScene(
    LARGE_INTEGER actualTickPoint,
    ControlStateFlags_t controlState,
    ControlValue_t controlValue )
  {

    mProcGarbageCollector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // Removes entities marked as inactive from the registry, noticing
                        // main scene class if demanded.

    mProcActorStateSelector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities are checked for state changes (firing, raid, etc.) according
                        // to their behavior component and random events.

    mProcEntitySpawner.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // New entities are spawned according to spawn requests stored in the
                        // registry by other processors (as missiles, for example).

    mProcSpecialActorSpawner.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // Special entities (as alien boss) are spawned according to special
                        // spawn requests stored in the registry by other processors.

    mProcPlayerSpeedUpdater.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint, controlState, controlValue );
                        // Player velocity is updated according to control state (keyboard)

    mProcPlayerFireUpdater.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint, controlState, controlValue );
                        // Player shoot requests are processed, new missiles are created if possible

    mProcPlayerBoundsGuard.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // Player entity is kept within scene bounds

    mProcAlienBoundsGuard.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint, mPlayerHeight * 1.25f );
                        // Alien entities are kept within scene bounds, alien group velocity is
                        // changed if needed. When first alien reaches left or right scene border,
                        // whole alien group is moved down for a few moment ant then starts moving in
                        // X-axis in opposite direction.

    mProcActorMover.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities are moved according to their velocity

    mProcAlienRaidDriver.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // Raiding or returning aliens have their velocity adjusted

    mProcActorOutOfSceneCheck.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities out of scene are marked as inactive and will be removed by garbage
                        // collector in next loop.

    mProcActorRender.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities are rendered according to their graphics component and status

    mProcCollisionDetector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    for( auto & item : mProcCollisionDetector.mCollidedPairs )
    {                   // Missile hits and alien-player collisions are handled
      auto [ id1, dmg1 ] = mEnTTRegistry.try_get<cpId, cpDamage>( item.first );

      if( nullptr != id1 && nullptr != dmg1 && dmg1->removeOnHit )
        id1->active = false;
                        // Missile is being removed by simple pruning and garbage collecting (it simply
                        // disappears). Alien that rams into the player, on other hand, is not eliminated
                        // at all (as its removeOnHit flag is set to false).

      EliminateEntity( item.second );
                        // Player or alien hit by missile is eliminated from the scene by much more complex
                        // procedure, involving explosion creation (and possible player respawn).
    } // for

    if( 0 < mTickLeftToReload )
      --mTickLeftToReload;
    else                // Player weapon is reloaded and ready to fire again
    {
      mTickLeftToReload = mReloadingTicks;
      if( mPlayerAmmoLeft < mSettings.GetAmmo() )
        ++mPlayerAmmoLeft;
    } // else

    return true;

  } // CInvGameScene::RenderActualScene

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::PlayerEntryProcessing( LARGE_INTEGER actTick )
  {
    if( !mPlayerEntryInProgress )
      return true;      // Player is already in the scene, nothing to do.

    LONGLONG textInterval = (LONGLONG)( (float)mSettings.GetTickPerSecond() * mPlayerEntryTextSecond );
                        // Each text from attention-ready-go is shown for mPlayerEntryTextSecond seconds

    float posY = mSceneHeight * mAlienStartingAreaCoefficient +
                 0.5f * ( mSceneHeight * ( 1.0f - mAlienStartingAreaCoefficient ) -
                 0.5f * mPlayerEntryLetterSize );
                        // Attention-ready-go text is drawn in the middle of the area below aliens

    if( 0 == mPlayerEntryTick.QuadPart )
    {
      mProcSpecialActorSpawner.mIsSuspended = true;
      mProcActorStateSelector.mIsSuspended = true;
      mProcPlayerSpeedUpdater.mIsSuspended = true;
      mProcPlayerFireUpdater.mIsSuspended = true;
      mProcActorMover.mFormationFreeze = true;
                        // During player entry sequence, aliens are not moving and player cannot
                        // control his ship (which is not visible at all).
      auto view = mEnTTRegistry.view<cpPlayBehave, cpPlayStatus, cpGraphics>();
      view.each( [=]( cpPlayBehave & pBehave, cpPlayStatus & pStat, cpGraphics & pGph )
      {
          pGph.isHidden = true;
      } );
    } // if

    if( mPlayerEntryTick.QuadPart < textInterval )
    {                   // "Attention" text is displayed
      auto width = mTAttention->GetTextLength() * mPlayerEntryLetterSize;
      mTAttention->Draw(
        mSceneTopLeftX + 0.5f * ( mSceneWidth - width ),
        posY,
        mPlayerEntryLetterSize,
        mTickReferencePoint,
        actTick, mDiffTickPoint );
    } // if
    else if( mPlayerEntryTick.QuadPart < 2 * textInterval )
    {                   // "Ready" text is displayed
      auto width = mTReady->GetTextLength() * mPlayerEntryLetterSize;
      mTReady->Draw(
        mSceneTopLeftX + 0.5f * ( mSceneWidth - width ),
        posY,
        mPlayerEntryLetterSize,
        mTickReferencePoint,
        actTick, mDiffTickPoint );
    } // else if
    else if( mPlayerEntryTick.QuadPart < 3 * textInterval )
    {                   // "Go" text is displayed
      auto width = mTGo->GetTextLength() * mPlayerEntryLetterSize;
      mTGo->Draw(
        mSceneTopLeftX + 0.5f * ( mSceneWidth - width ),
        posY,
        mPlayerEntryLetterSize,
        mTickReferencePoint,
        actTick, mDiffTickPoint );
    } // else if
    else
    {                   // Player entity is spawned or respawned and made invulnerable for a while.
                        // Aliens start moving and player can control his ship again.
      if( !mPlayerAlive )
        SpawnPlayer();
      else
      {
        auto [pId, pBehave, pStatus, pPos, pGph] =
          mEnTTRegistry.try_get<cpId, cpPlayBehave, cpPlayStatus, cpPosition, cpGraphics>( mPlayerEntity );
        if( nullptr != pId && nullptr != pBehave && nullptr != pStatus && nullptr != pPos && nullptr != pGph )
        {
          pPos->X = mPlayerStartX;
          pPos->Y = mPlayerStartY;
          pStatus->isInvulnerable = true;
          pGph->standardAnimationEffect->Restore();
                        // Player is made invulnerable for a while, blinking effect is started on his sprite.
        } // if
      } // else

      mPlayerEntryInProgress = false;
      if( IsZero( mVXGroup ) )
        mVXGroup = mSettingsRuntime.mAlienVelocity * mSettingsRuntime.mSceneLevelMultiplicator / (float)mSettings.GetTickPerSecond();

      mProcSpecialActorSpawner.mIsSuspended = false;
      mProcActorStateSelector.mIsSuspended = false;
      mProcPlayerSpeedUpdater.mIsSuspended = false;
      mProcPlayerFireUpdater.mIsSuspended = false;
      mProcActorMover.mFormationFreeze = false;
                        // Aliens start moving and player can control his ship again.

      auto view = mEnTTRegistry.view<cpPlayBehave, cpPlayStatus, cpGraphics>();
      view.each( [=]( cpPlayBehave & pBehave, cpPlayStatus & pStat, cpGraphics & pGph )
      {                 // If (alive) player was hidden during entry sequence, he is made visible now.
        pGph.isHidden = false;
      } );

    } // else

    ++mPlayerEntryTick.QuadPart;

    return true;
  } // CInvGameScene::PlayerEntryProcessing

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::Reset( LARGE_INTEGER newTickRefPoint )
  {
    mTickReferencePoint = newTickRefPoint;
    mEnTTRegistry.clear();

    GenerateNewScene(); // Aliens swarm is placed in the scene. Also recalculates
                        // various scene parameters, whic is then sent to processors.

    mProcGarbageCollector.reset( newTickRefPoint );

    mProcActorStateSelector.reset( newTickRefPoint );

    mProcEntitySpawner.reset( newTickRefPoint );

    mProcSpecialActorSpawner.reset(
      newTickRefPoint,
      mSaucerSize,
      mSaucerSpawnY,
      mSaucerSpawnXLeft,
      mSaucerSpawnXRight );

    mProcPlayerBoundsGuard.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcAlienBoundsGuard.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcActorMover.reset( newTickRefPoint );

    mProcAlienRaidDriver.reset( newTickRefPoint );

    mProcActorOutOfSceneCheck.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcActorRender.reset( newTickRefPoint );

    mProcCollisionDetector.reset( newTickRefPoint );

    mActualScore = 0;
    mPlayerAlive = false;
                        // Score is zeroed, player is not alive (not spawned) yet.

    mPlayerLivesLeft = mSettings.GetInitialLives();
    mPlayerAmmoLeft = mSettings.GetAmmo();
                        // Player lives and ammo are reset to initial values.

    mReloadingTicks = (LONGLONG)( (float)mSettings.GetTickPerSecond() * mSettings.GetReloadTime() );
    mTickLeftToReload = mReloadingTicks;
                        // Player weapon reloading time is set according to settings.

    mPlayerEntryInProgress = true;
    mPlayerEntryTick.QuadPart = 0;
                        // Player entry sequence is started.

  } // CInvGameScene::Reset

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::EliminateEntity( entt::entity entity )
  {
    auto entId = mEnTTRegistry.try_get<cpId>( entity );
    if( nullptr == entId || ! entId->active )
      return false;

    auto [playBehave, playStatus, playHealth, playGph] =
      mEnTTRegistry.try_get<cpPlayBehave, cpPlayStatus, cpHealth, cpGraphics>( entity );
    if( nullptr != playBehave && nullptr != playStatus )
    {                   // Player entity elimination from game scene is commenced (hit by alien or
                        // alien missile). Player entity is marked as dying, explosion is created
                        // at player position and special "dying" effect is started on player sprite.

      if( nullptr == playHealth || nullptr == playGph )
        return false;   // Player has no health or graphics component - this is probably a bug.

      if( playStatus->isInvulnerable || playStatus->isDying )
        return false;   // Player is invulnerable or already dying, ignore the hit.

      if( 0 < playHealth->hitPoints )
        --playHealth->hitPoints;
                        // Player looses one health point.

      if( 0 < playHealth->hitPoints )
        return true;    // Player ship still has some health points and continues to fight.
                        // Remark: this has nothing to do with number of player lives (ships),
                        // number of hitpoints defines how many hits the single ship can take
                        // before it is destroyed.

      playStatus->isDying = true;
                        // Welcome to the scrapyard, pal ...

      auto [ pPos, pVel, pGeo] = mEnTTRegistry.try_get<cpPosition, cpVelocity, cpGeometry>( entity );

      if( mSettings.GetZeroExplosionV() )
      {                // If demanded, destroyed player stops and explosion does not inherit his velocity.
        pVel->vX = 0.0f;
        pVel->vY = 0.0f;
        pVel->vZ = 0.0f;
      }  // if

      auto explosionSize = ( nullptr == pGeo ? 150.0f : playGph->standardSprite->GetResultingSizeX() * 1.5f );
      auto xplX = ( nullptr == pPos ? 0.0f : pPos->X );
      auto xplY = ( nullptr == pPos ? 0.0f : pPos->Y );
      auto xplVx = ( nullptr == pVel ? 0.0f : pVel->vX );
      auto xplVy = ( nullptr == pVel ? 0.0f : pVel->vY );
      mEntityFactory.AddExplosionEntity( "FEXPL", xplX, xplY, explosionSize, xplVx, xplVy );
                        // Explosion is created at player position, moving with the player. Explosion entity
                        // is automatically pruned from game scene when its animation finishes.

      if( nullptr != playGph && nullptr != playGph->dyingAnimationEffect )
        playGph->dyingAnimationEffect->Restore();
                        // Dying effect is started on player sprite. When the effect finishes, player entity
                        // is marked for pruning and removed from game scene by garbage collector. This
                        // then triggers EntityJustPruned() method, which notifies main game scene about
                        // player elimination.

    } // if

    auto [alienBehave, alienStatus, alienHealth, alienGph] =
      mEnTTRegistry.try_get<cpAlienBehave, cpAlienStatus, cpHealth, cpGraphics>( entity );
    if( nullptr != alienBehave && nullptr != alienStatus )
    {                   // Alien entity elimination from game scene is commenced (hit by player missile).
                        // Alien entity is marked as dying, explosion is created at alien position and
                        // special "dying" effect is started on alien sprite.

      if( nullptr == alienHealth || nullptr == alienGph )
        return false;   // Alien has no health or graphics component - this is probably a bug.

      if( alienStatus->isDying )
        return false;   // Alien is already dying, ignore the hit.

      if( 0 < alienHealth->hitPoints )
        --alienHealth->hitPoints;
                        // Alien looses one health point.

      if( 0 < alienHealth->hitPoints )
        return true;    // Alien still has some health points and continues to fight.

      alienStatus->isDying = true;
                        // Welcome to the graveard, bastard ...

      auto [ pPos, pVel, pGeo] = mEnTTRegistry.try_get<cpPosition, cpVelocity, cpGeometry>( entity );

      if( mSettings.GetZeroExplosionV() )
      {                 // If demanded, destroyed alien stops and explosion does not inherit his velocity.
        pVel->vX = 0.0f;
        pVel->vY = 0.0f;
        pVel->vZ = 0.0f;
      }  // if

      auto explosionSize = ( nullptr == pGeo ? 150.0f : alienGph->standardSprite->GetResultingSizeX() * 1.5f );
      auto xplX = ( nullptr == pPos ? 0.0f : pPos->X );
      auto xplY = ( nullptr == pPos ? 0.0f : pPos->Y );
      auto xplVx = ( nullptr == pVel ? 0.0f : pVel->vX );
      auto xplVy = ( nullptr == pVel ? 0.0f : pVel->vY );
      mEntityFactory.AddExplosionEntity( "PINKEXPL", xplX, xplY, explosionSize, xplVx, xplVy );
                        // Explosion is created at alien position, moving with the invader. Explosion entity
                        // is automatically pruned from game scene when its animation finishes.

      if( nullptr != alienGph && nullptr != alienGph->dyingAnimationEffect )
        alienGph->dyingAnimationEffect->Restore();
                        // Dying effect is started on invader sprite. When the effect finishes, entity
                        // is marked for pruning and removed from game scene by garbage collector. This
                        // then triggers EntityJustPruned() method, which notifies main game scene about
                        // invader elimination.

    } // if


    auto [aliennBossBehave, alienBossStatus, aliennBossHealth, aliennBossGph] =
      mEnTTRegistry.try_get<cpAlienBehave, cpAlienBossStatus, cpHealth, cpGraphics>( entity );
    if( nullptr != aliennBossBehave && nullptr != alienBossStatus )
    {                   // Alien entity elimination from game scene is commenced (hit by player missile).
                        // Alien entity is marked as dying, explosion is created at alien position and
                        // special "dying" effect is started on alien sprite.

      if( nullptr == aliennBossHealth || nullptr == aliennBossGph )
        return false;   // Alien has no health or graphics component - this is probably a bug.

      if( alienBossStatus->isDying )
        return false;   // Alien is already dying, ignore the hit.

      if( 0 < aliennBossHealth->hitPoints )
        --aliennBossHealth->hitPoints;
                        // Alien looses one health point.

      if( 0 < aliennBossHealth->hitPoints )
        return true;    // Alien still has some health points and continues to fight.

      alienBossStatus->isDying = true;
                        // Welcome to the graveard, bastard ...

      auto [pPos, pVel, pGeo] = mEnTTRegistry.try_get<cpPosition, cpVelocity, cpGeometry>( entity );

      if( mSettings.GetZeroExplosionV() )
      {                 // If demanded, destroyed alien stops and explosion does not inherit his velocity.
        pVel->vX = 0.0f;
        pVel->vY = 0.0f;
        pVel->vZ = 0.0f;
      }  // if

      auto explosionSize = ( nullptr == pGeo ? 150.0f : alienGph->standardSprite->GetResultingSizeX() * 1.5f );
      auto xplX = ( nullptr == pPos ? 0.0f : pPos->X );
      auto xplY = ( nullptr == pPos ? 0.0f : pPos->Y );
      auto xplVx = ( nullptr == pVel ? 0.0f : pVel->vX );
      auto xplVy = ( nullptr == pVel ? 0.0f : pVel->vY );
      mEntityFactory.AddExplosionEntity( "SAUCEREXPL", xplX, xplY, explosionSize, xplVx, xplVy );
                        // Explosion is created at alien position, moving with the invader. Explosion entity
                        // is automatically pruned from game scene when its animation finishes.

      if( nullptr != aliennBossGph && nullptr != aliennBossGph->dyingAnimationEffect )
        aliennBossGph->dyingAnimationEffect->Restore();
                        // Dying effect is started on invader sprite. When the effect finishes, entity
                        // is marked for pruning and removed from game scene by garbage collector. This
                        // then triggers EntityJustPruned() method, which notifies main game scene about
                        // invader elimination.

    } // if

    return true;

  } // CInvGameScene::EliminateEntity

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::EntityJustPruned( entt::entity entity, uint32_t nr )
  {
    auto [entId, entBehave, entStatus] = mEnTTRegistry.try_get<cpId, cpPlayBehave, cpPlayStatus>( entity );
    if( nullptr != entId && nullptr != entBehave && nullptr != entStatus )
    {                   // Player entity elimination from game scene is done, appropriate measures
                        // must be taken (respawn, reduce number of lives, end of game etc.)

      if( entId->active )
      {
        LOG << "Trying to prune active player!";
        return;         // Player entity is still active, this is probably a bug.
      }

      LOG << "Player was pruned from game scene.";

      mPlayerEntryInProgress = true;
      mPlayerEntryTick.QuadPart = 0;
      mPlayerAlive = false;
                        // Player is no more in the scene, new entry sequence must be started.

      if( 0 < mPlayerLivesLeft )
        --mPlayerLivesLeft;
                        // One player life (ship) is lost.
      return;

    } // if

    auto [aId, aBehave, aStatus] = mEnTTRegistry.try_get<cpId, cpAlienBehave, cpAlienStatus>( entity );
    if( nullptr != aId && nullptr != aBehave && nullptr != aStatus )
    {
      if( aId->active )
      {
        LOG << "Trying to prune active alien!";
        return;         // Player entity is still active, this is probably a bug.
      } // if

      mActualScore += aBehave->scoreToAdd;
      mSettingsRuntime.mAlienSpeedupFactor += mSettings.GetSpeedupPerKill();

      LOG << "Alien was pruned from game scene, " << aBehave->scoreToAdd
        << " points added, speed upscaled to " << mSettingsRuntime.mAlienSpeedupFactor;

      if( 0u < mAliensLeft )
      --mAliensLeft;
    } // if

    auto [aBossId, aBossBehave, aBossStatus, aBossPos, aBossGeo] =
      mEnTTRegistry.try_get<cpId, cpAlienBehave, cpAlienBossStatus, cpPosition, cpGeometry>( entity );
    if( nullptr != aId && nullptr != aBehave && nullptr != aBossStatus )
    {
      if( aBossId->active )
      {
        LOG << "Trying to prune active alien boss!";
        return;         // Player entity is still active, this is probably a bug.
      } // if

      if( nullptr == aBossPos || nullptr == aBossGeo )
        return;

      if( ( mSceneTopLeftX < aBossPos->X + aBossGeo->width * 0.5f ) &&
          ( mSceneTopLeftY < aBossPos->Y + aBossGeo->height * 0.5f ) &&
          ( aBossPos->X - aBossGeo->width * 0.5f < mSceneBottomRightX ) &&
          ( aBossPos->Y - aBossGeo->height * 0.5f < mSceneBottomRightY ) )
      {
        mActualScore += aBossBehave->scoreToAdd;

        LOG << "Alien boss was pruned from game scene, " << aBossBehave->scoreToAdd << " score added";
      }

      if( 0u < mAliensLeft )
        --mAliensLeft;

    } // if

    if( 0u == mAliensLeft )
      NewSwarm();       // All aliens are dead, new swarm must be generated.

  } // CInvGameScene::EntityJustPruned

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::NewSwarm()
  {

    mPlayerEntryInProgress = true;
    mPlayerEntryTick.QuadPart = 0;

    auto view = mEnTTRegistry.view<cpId, cpDamage>();
    view.each( [=]( entt::entity e, cpId & eId, cpDamage & dmg )
    {                   // All missiles currently in the scene are removed, as they have no target
                        // to hit anymore in current swarm - and we do not want to have them flying
                        // around while new swarm is being generated.
      auto aStat = mEnTTRegistry.try_get<cpAlienStatus>( e );
      if( nullptr != aStat )
        return;

      auto aBossStat = mEnTTRegistry.try_get<cpAlienBossStatus>( e );
      if( nullptr != aBossStat )
        return;

      auto pStat = mEnTTRegistry.try_get<cpPlayStatus>( e );
      if( nullptr != pStat )
        return;

      eId.active = false;
    } );

    GenerateNewScene();

    mSettingsRuntime.mSceneLevelMultiplicator *= mSettings.GetDifficultyBuildup();
    ++mSettingsRuntime.mSceneLevel;
    mSettingsRuntime.mAlienSpeedupFactor = 1.0f;

    LOG;
    LOG << "New alien swarm generated, level " << mSettingsRuntime.mSceneLevel
        << ", speed factor reset to 1.0, level multiplicator is now "
        << mSettingsRuntime.mSceneLevelMultiplicator;
    LOG;

    mSettingsRuntime.Preprint();

  } // CInvGameScene::NewSwarm

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::RenderStatusBar( LARGE_INTEGER actualTickPoint )
  {
    float topLine = mStatusLineTopLeftY * 1.04f;
                        // Status line have small margin on top

    if( nullptr != mLiveSprite && 1u < mPlayerLivesLeft )
    {                   // Player lives (ships) are drawn in the right part of the status line.
                        // One life (ship) is not drawn, as it is represented by player entity
                        // in the scene.
      float posX = mLivesIconsStartX;
      for( uint32_t i = 0; i < (mPlayerLivesLeft - 1u); ++i )
      {
        mLiveSprite->Draw(
          posX, topLine, mOneLiveIconWidth, mOneLiveIconHeight,
          actualTickPoint, actualTickPoint, mDiffTickPoint );
        posX += mOneLiveIconWidth * 1.05f;
      } // for
    } // if

    if( nullptr != mAmmoSprite )
    {                   // Player ammo (missiles) are drawn in the right part of the status line,
                        // left to player lives (ships). Ammo icons are drawn half-overlapped (skew
                        // effects on single rockets).
      float posX = mAmmoIconsStartX;
      for( uint32_t i = 0; i < mPlayerAmmoLeft; ++i )
      {
        mAmmoSprite->Draw(
          posX, topLine, mAmmoIconWidth, mAmmoIconHeight,
          actualTickPoint, actualTickPoint, mDiffTickPoint );
        posX += mAmmoIconWidth * 0.5f;
      } // for
    } // if

    mScoreLabelBuffer = mScoreText + std::to_string( mActualScore );
    mScoreLabel.SetText( mScoreLabelBuffer );
    mScoreLabel.Draw(
      mStatusLineTopLeftX, mStatusLineTopLeftY + 0.2f * mStatusLineHeight,
      mScoreLabelTextSize, mTickReferencePoint, actualTickPoint, mDiffTickPoint );
                        // Actual score is drawn in the left part of the status line.

    return true;

  } // CInvGameScene::RenderStatusBar

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::CallbackUnsetActive( entt::entity ent, uint32_t nr )
  {
    auto pId = mEnTTRegistry.try_get<cpId>( ent );
    if( nullptr != pId )
      pId->active = false;
  } // CInvGameScene::CallbackUnsetActive

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::CallbackPlayerInvulnerabilityCanceled( entt::entity ent, uint32_t nr )
  {
    auto pStat = mEnTTRegistry.try_get<cpPlayStatus>( ent );
    if( nullptr != pStat )
      pStat->isInvulnerable = false;
  } // CInvGameScene::CallbackPlayerInvulnerabilityCanceled

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::CallbackAlienAnimationDone( entt::entity ent, uint32_t nr )
  {
    auto aStat = mEnTTRegistry.try_get<cpAlienStatus>( ent );
    if( nullptr != aStat )
      aStat->isAnimating = false;
  } // CInvGameScene::CallbackAlienAnimationDone

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::CallbackAlienFiringDone( entt::entity ent, uint32_t nr )
  {
    auto aStat = mEnTTRegistry.try_get<cpAlienStatus>( ent );
    if( nullptr != aStat )
      aStat->isFiring = false;

  } // CInvGameScene::CallbackAlienFiringDone

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::CallbackAlienShootRequested( entt::entity ent, uint32_t nr )
  {
    auto aStat = mEnTTRegistry.try_get<cpAlienStatus>( ent );
    if( nullptr != aStat )
      aStat->isShootRequested = true;
  } // CInvGameScene::CallbackAlienShootRequested

  //-------------------------------------------------------------------------------------------------

} // namespace Inv
