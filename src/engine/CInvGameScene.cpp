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

    mProcGarbageCollector( tickReferencePoint, BIND_MEMBER_EVENT_CALLBACK( this, CInvGameScene::EntityJustPruned ) ),
    mProcActorStateSelector( tickReferencePoint ),
    mProcEntitySpawner( tickReferencePoint, mEntityFactory, settingsRuntime ),
    mProcActorMover( tickReferencePoint ),
    mProcPlayerSpeedUpdater( tickReferencePoint, settingsRuntime ),
    mProcPlayerBoundsGuard( tickReferencePoint, 0.0f, 0.0f, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight() ),
    mProcActorOutOfSceneCheck( tickReferencePoint, 0.0f, 0.0f, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight() ),
    mProcCollisionDetector( tickReferencePoint, mCollisionTest ),
    mProcActorRender( tickReferencePoint ),

    mPlayerEntryInProgress( false ),
    mPlayerEntryTick{ 0 },
    mTAttention( nullptr ),
    mTReady( nullptr ),
    mTGo( nullptr ),
    mTBlinkEffect( nullptr ),
    mPlayerEntryLetterSize( 40.0f )
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
      {  8, "PINK" },
      {  7, "PINK" },
      {  6, "PINK" },
    };

    auto alienAreaHeight = mSceneHeight * mAlienStartingAreaCoefficient;

    uint32_t maxAliens = 0;
    for( const auto & ar : alienRows )
    {
      if( maxAliens < ar.first )
        maxAliens = ar.first;
    } // for

    auto alienWidth = mSceneWidth / ( 1.2f * (float)maxAliens );

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
      auto spaceInBetween = ( mSceneWidth - spaceTakenByAliens ) / (float)( ar.first + 1 );

      auto xPos = mSceneTopLeftX + spaceInBetween + alienWidth * 0.5f;
      for( uint32_t i = 0; i < ar.first; ++i )
      {
        mEntityFactory.AddAlienEntity( ar.second, xPos, yPos, alienWidth );
        xPos += alienWidth + spaceInBetween;
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

    float playerWidth = mSceneWidth * 0.1f;

    auto baseSize = baseSprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    auto playerHeight = playerWidth * aspectRatio;


    mEntityFactory.AddPlayerEntity(
      "FIGHT",
      mSceneTopLeftX + mSceneWidth * 0.5f,
      mSceneBottomRightY - playerHeight,
      playerWidth );


/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* DOOMED! Player dies at the start of the game for debugging purposes    */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
   mEntityFactory.AddMissileEntity(
     "SPIT", false,
     mSceneTopLeftX + mSceneWidth * 0.5f - 0.25*playerWidth,
     mSceneBottomRightY - 4 * playerHeight,
     0.33f * playerWidth );
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
    mProcEntitySpawner.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );

    mProcPlayerSpeedUpdater.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint, controlState, controlValue );
                        // Player velocity is updated according to control state (keyboard)

    mProcPlayerBoundsGuard.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // Player entity is kept within scene bounds

    mProcActorMover.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcActorOutOfSceneCheck.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities arfe moved according to their velocity, entities out of scene
                        // are marked as inactive and will be removed by garbage collector in next loop.

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
      SpawnPlayer();
      mPlayerEntryInProgress = false;
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

    mProcActorMover.reset( newTickRefPoint );
    mProcActorOutOfSceneCheck.reset(
      newTickRefPoint,
      mSceneTopLeftX, mSceneTopLeftY,
      mSceneBottomRightX, mSceneBottomRightY );

    mProcActorRender.reset( newTickRefPoint );

    mProcCollisionDetector.reset( newTickRefPoint );

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

      if( nullptr != playGph && nullptr != playGph->specificAnimationEffect )
      {                 // Dying effect is started on player sprite. When the effect finishes, player entity
                        // is marked for pruning and removed from game scene by garbage collector. This
                        // then triggers EntityJustPruned() method, which notifies main game scene about
                        // player elimination.
        if( nullptr != playGph->specificAnimationEffect )
          playGph->specificAnimationEffect->Restore();
      } // if

    } // if

    return true;
  } // CInvGameScene::EliminateEntity

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::EntityJustPruned( entt::entity entity, uint32_t nr )
  {
     auto [ entId, entBehave, entStatus ] = mEnTTRegistry.try_get<cpId, cpPlayBehave, cpPlayStatus>(entity);
    if( nullptr == entId || entId->active )
      return;
    if( nullptr != entBehave && nullptr != entStatus )
    {                   // Player entity elimination from game scene is done, appropriate measures
                        // must be taken (respawn, reduce number of lives, end of game etc.)

      LOG << "Player was pruned from game scene.";

      mPlayerEntryInProgress = true;
      mPlayerEntryTick.QuadPart = 0;
                        // Player is no more in the scene, new entry sequence must be started.

    } // if

  } // CInvGameScene::EntityJustPruned

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
