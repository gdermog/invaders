//****************************************************************************************************
//! \file CInvGameScene.cpp
//! Module declares class CInvGameScene, which implements ...
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

  const std::string CInvGameScene::mPlayerEntryTextAttention = "ATTENTION";
  const std::string CInvGameScene::mPlayerEntryTextReady = "READY";
  const std::string CInvGameScene::mPlayerEntryTextGo = "GO";
  const float CInvGameScene::mPlayerEntryTextSecond = 1.5f;

  //**************************************************************************************************

  CInvGameScene::CInvGameScene(
    const CInvSettings & settings,
    const CInvSpriteStorage & spriteStorage,
    CInvPrimitive & primitives,
    CInvSettingsRuntime & settingsRuntime,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER tickReferencePoint ):

    mTickReferencePoint( tickReferencePoint ),
    mDiffTickPoint{ 0 },
    mSettings( settings ),
    mSpriteStorage( spriteStorage ),
    mPrimitives( primitives ),
    mSettingsRuntime( settingsRuntime ),
    mCollisionTest( settings, pd3dDevice ),
    mEnTTRegistry(),
    mEntityFactory( settings, spriteStorage, mEnTTRegistry, *this, mSettingsRuntime, pD3D, pd3dDevice, pVB ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),

    mSceneWidth( (float)settings.GetWindowWidth() ),
    mSceneHeight( (float)settings.GetWindowHeight() ),
    mSceneTopLeftX( 0.0f ),
    mSceneTopLeftY( 0.0f ),
    mSceneBottomRightX( (float)settings.GetWindowWidth() ),
    mSceneBottomRightY( (float)settings.GetWindowHeight() ),
    mAlienStartingAreaCoefficient( 0.65f ),

    mVXGroup( 0.0f ),
    mVYGroup( 0.0f ),

    mPlayerEntity{},
    mPlayerWidth( 0.0f ),
    mPlayerHeight( 0.0f ),

    mPlayerAlive( false ),

    mAliensLeft( 0 ),

    mProcGarbageCollector(
      tickReferencePoint, BIND_MEMBER_EVENT_CALLBACK( this, CInvGameScene::EntityJustPruned ) ),
    mProcActorStateSelector( tickReferencePoint ),
    mProcEntitySpawner( tickReferencePoint, mEntityFactory, settingsRuntime ),
    mProcActorMover( tickReferencePoint, mVXGroup, mVYGroup, settingsRuntime ),
    mProcPlayerFireUpdater( tickReferencePoint, mEntityFactory, settingsRuntime ),
    mProcPlayerSpeedUpdater( tickReferencePoint, settings, settingsRuntime ),
    mProcPlayerBoundsGuard(
      tickReferencePoint, 0.0f, 0.0f, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight() ),
    mProcAlienBoundsGuard(
      tickReferencePoint, mVXGroup, mVYGroup, 0.0f, 0.0f,
      (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight(), settings, settingsRuntime ),
    mProcActorOutOfSceneCheck(
      tickReferencePoint, 0.0f, 0.0f, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight() ),
    mProcCollisionDetector( tickReferencePoint, mCollisionTest ),
    mProcActorRender( tickReferencePoint ),

    mPlayerEntryInProgress( false ),
    mPlayerEntryTick{ 0 },
    mTAttention( nullptr ),
    mTReady( nullptr ),
    mTGo( nullptr ),
    mTBlinkEffect( nullptr ),
    mPlayerEntryLetterSize( 40.0f ),

    mActualScore( 0 )
  {
  } // CInvGameScene::CInvGameScene

  //-------------------------------------------------------------------------------------------------

  CInvGameScene::~CInvGameScene()
  {

  } // CInvGameScene::~CInvGameScene

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::GenerateNewScene(
    float sceneTopLeftX, float sceneTopLeftY,
    float sceneBottomRightX, float sceneBottomRightY )
  {
    mSceneWidth = min( (float)mSettings.GetWindowWidth(), sceneBottomRightX - sceneTopLeftX );
    mSceneHeight = min( (float)mSettings.GetWindowHeight(), sceneBottomRightY - sceneTopLeftY );

    mSceneTopLeftX = sceneTopLeftX;
    if( mSceneTopLeftX < 0.0f )
      mSceneTopLeftX = 0.0f;
    if( (float)mSettings.GetWindowWidth() < mSceneTopLeftX + mSceneWidth )
      mSceneTopLeftX = (float)mSettings.GetWindowWidth() - mSceneWidth;

    mSceneTopLeftY = sceneTopLeftY;
    if( mSceneTopLeftY < 0.0f )
      mSceneTopLeftY = 0.0f;
    if( (float)mSettings.GetWindowHeight() < mSceneTopLeftY + mSceneHeight )
      mSceneTopLeftY = (float)mSettings.GetWindowHeight() - mSceneHeight;

    mSceneBottomRightX = mSceneTopLeftX + mSceneWidth;
    if( (float)mSettings.GetWindowWidth() < mSceneBottomRightX )
      mSceneBottomRightX = (float)mSettings.GetWindowWidth();

    mSceneBottomRightY = mSceneTopLeftY + mSceneHeight;
    if( (float)mSettings.GetWindowHeight() < mSceneBottomRightY )
      mSceneBottomRightY = (float)mSettings.GetWindowHeight();

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
    {
      { 10, "PINK" },
      {  9, "PINK" },
      { 10, "PINK" },
      {  9, "PINK" },
      { 10, "PINK" },
    };

    auto alienAreaHeight = mSceneHeight * mAlienStartingAreaCoefficient;

    uint32_t maxAliens = 0;
    for( const auto & ar : alienRows )
    {
      if( maxAliens < ar.first )
        maxAliens = ar.first;
    } // for

    auto alienWidth = mSceneWidth / ( 1.60f * (float)maxAliens );
    auto spaceInBetween = 0.40f * alienWidth;

    mVXGroup = 0.0f;
    mVYGroup = 0.0f;

    mAliensLeft = 0;

    uint32_t rowIndex = 0;
    for( auto & ar : alienRows )
    {
      auto baseSprite = mSpriteStorage.GetSprite( ar.second );
      if( nullptr == baseSprite )
        continue;

      auto baseSize = baseSprite->GetImageSize( 0 );
      auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
      auto alienHeight = alienWidth * aspectRatio;
      auto yPos = mSceneTopLeftY + ( alienHeight * 1.2f ) * (float)rowIndex + alienHeight * 0.5f;

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

    return true;
  } // CInvGameScene::GenerateNewScene

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::SpawnPlayer()
  {
    auto baseSprite = mSpriteStorage.GetSprite( "FIGHT" );
    if( nullptr == baseSprite )
      return false;

    mPlayerWidth = mSceneWidth * 0.1f;

    auto baseSize = baseSprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mPlayerHeight = mPlayerWidth * aspectRatio;

    mPlayerEntity = mEntityFactory.AddPlayerEntity(
      "FIGHT",
      mSceneTopLeftX + mSceneWidth * 0.5f,
      mSceneBottomRightY - mPlayerHeight,
      mPlayerWidth );

    mPlayerAlive = true;

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* DOOMED! Player dies at the start of the game for debugging purposes    */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
// mEntityFactory.AddMissileEntity(
//   "SPIT", false,
//   mSceneTopLeftX + mSceneWidth * 0.5f - 0.25*playerWidth,
//   mSceneBottomRightY - 4 * playerHeight,
//   0.33f * playerWidth );
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* QUICKSHOT! Player fires at the start of the game for debugging purposes*/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
//  mEntityFactory.AddMissileEntity(
//    "ROCKET", true,
//    mSceneTopLeftX + mSceneWidth * 0.5f ,
//    mSceneBottomRightY - playerHeight,
//    0.1f * playerWidth,
//    0.0f, -1.0f );
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

    return true;

  } // CInvGameScene::RenderActualScene

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::PlayerEntryProcessing( LARGE_INTEGER actTick )
  {
    if( ! mPlayerEntryInProgress )
      return true;      // Player is already in the scene, nothing to do.

    LONGLONG textInterval = (LONGLONG)( (float)mSettings.GetTickPerSecond() * mPlayerEntryTextSecond );
                        // Each text from attention-ready-go is shown for mPlayerEntryTextSecond seconds

     float posY = mSceneHeight * mAlienStartingAreaCoefficient +
                  0.5f * ( mSceneHeight * ( 1.0f - mAlienStartingAreaCoefficient ) -
                  0.5f * mPlayerEntryLetterSize );
                        // Attention-ready-go text is drawn in the middle of the area below aliens

    if( mPlayerEntryTick.QuadPart < textInterval )
    {
      auto width = mTAttention->GetTextLength() * mPlayerEntryLetterSize;
      mTAttention->Draw(
        mSceneTopLeftX + 0.5f * ( mSceneWidth - width ),
        posY,
        mPlayerEntryLetterSize,
        mTickReferencePoint,
        actTick, mDiffTickPoint );
    } // if
    else if( mPlayerEntryTick.QuadPart < 2*textInterval )
    {
      auto width = mTReady->GetTextLength() * mPlayerEntryLetterSize;
      mTReady->Draw(
        mSceneTopLeftX + 0.5f * ( mSceneWidth - width ),
        posY,
        mPlayerEntryLetterSize,
        mTickReferencePoint,
        actTick, mDiffTickPoint );
    } // else if
    else if( mPlayerEntryTick.QuadPart < 3 * textInterval )
    {
      auto width = mTGo->GetTextLength() * mPlayerEntryLetterSize;
      mTGo->Draw(
        mSceneTopLeftX + 0.5f * ( mSceneWidth - width ),
        posY,
        mPlayerEntryLetterSize,
        mTickReferencePoint,
        actTick, mDiffTickPoint );
    } // else if
    else
    {
      if( !mPlayerAlive )
        SpawnPlayer();
      else
      {
        auto [pId, pBehave, pStatus, pGph] =
          mEnTTRegistry.try_get<cpId, cpPlayBehave, cpPlayStatus, cpGraphics>( mPlayerEntity );
        if( nullptr != pId && nullptr != pBehave && nullptr != pStatus && nullptr != pGph )
        {
          pStatus->isInvulnerable = true;
          pGph->standardAnimationEffect->Restore();
                        // Player is made invulnerable for a while, blinking effect is started on his sprite.
        } // if
      } // else

      mPlayerEntryInProgress = false;
      if( IsZero( mVXGroup ) )
        mVXGroup = mSettingsRuntime.mAlienVelocity * mSettingsRuntime.mSceneLevelMultiplicator / (float)mSettings.GetTickPerSecond();
    } // else

    ++mPlayerEntryTick.QuadPart;

    return true;
  } // CInvGameScene::PlayerEntryProcessing

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::Reset( LARGE_INTEGER newTickRefPoint )
  {
    mTickReferencePoint = newTickRefPoint;
    mEnTTRegistry.clear();

    mProcGarbageCollector.reset( newTickRefPoint );

    mProcActorStateSelector.reset( newTickRefPoint );
    mProcEntitySpawner.reset( newTickRefPoint );

    mProcPlayerBoundsGuard.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcAlienBoundsGuard.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcActorMover.reset( newTickRefPoint );
    mProcActorOutOfSceneCheck.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcActorRender.reset( newTickRefPoint );

    mProcCollisionDetector.reset( newTickRefPoint );

    mActualScore = 0;
    mPlayerAlive = false;

    GenerateNewScene( mSceneTopLeftX, mSceneTopLeftY, mSceneBottomRightX, mSceneBottomRightY );
                        // Aliens swarm is placed in the scene.

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

      if( nullptr == playHealth || nullptr == playGph)
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
      {                // If demanded, destroyed alien stops and explosion does not inherit his velocity.
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

    return true;
  } // CInvGameScene::EliminateEntity

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::EntityJustPruned( entt::entity entity, uint32_t nr )
  {
    auto [ entId, entBehave, entStatus ] = mEnTTRegistry.try_get<cpId, cpPlayBehave, cpPlayStatus>(entity);
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

      return;

    } // if

    auto [aId, aBehave, aStatus] = mEnTTRegistry.try_get<cpId, cpAlienBehave, cpAlienStatus>( entity );
    if( nullptr != aId && nullptr != aBehave && nullptr != aStatus )
    {
      if( entId->active )
      {
        LOG << "Trying to prune active alien!";
        return;         // Player entity is still active, this is probably a bug.
      }

      mActualScore += aBehave->scoreToAdd;
      mSettingsRuntime.mAlienSpeedupFactor += mSettings.GetSpeedupPerKill();

      LOG << "Alien was pruned from game scene, " << aBehave->scoreToAdd
          << " points added, speed upscaled to " << mSettingsRuntime.mAlienSpeedupFactor;

      --mAliensLeft;
    }

    if( 0 == mAliensLeft )
      NewSwarm();       // All aliens are dead, new swarm must be generated.

  } // CInvGameScene::EntityJustPruned

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::NewSwarm()
  {

    mPlayerEntryInProgress = true;
    mPlayerEntryTick.QuadPart = 0;

    GenerateNewScene( mSceneTopLeftX, mSceneTopLeftY, mSceneBottomRightX, mSceneBottomRightY );

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
