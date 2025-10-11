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


  //**************************************************************************************************

  CInvGameScene::CInvGameScene(
    const CInvSettings & settings,
    const CInvText & textCreator,
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
    mTextCreator( textCreator ),
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
    mProcActorOutOfSceneCheck( tickReferencePoint, 0.0f, 0.0f, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight() ),
    mProcCollisionDetector( tickReferencePoint, mCollisionTest ),
    mProcActorRender( tickReferencePoint )
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
      mSceneTopLeftX + mSceneWidth * 0.5f,
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
                        // main scena class if demanded.

    mProcActorStateSelector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcEntitySpawner.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );

    mProcPlayerSpeedUpdater.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint, controlState, controlValue );
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
      EliminateEntity( item.second );
    } // for

    return true;

  } // CInvGameScene::RenderActualScene

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::Reset( LARGE_INTEGER newTickRefPoint )
  {
    mTickReferencePoint = newTickRefPoint;
    mEnTTRegistry.clear();

    mProcGarbageCollector.reset( newTickRefPoint );

    mProcActorStateSelector.reset( newTickRefPoint );
    mProcEntitySpawner.reset( newTickRefPoint );

    mProcActorMover.reset( newTickRefPoint );
    mProcActorOutOfSceneCheck.reset(
      newTickRefPoint,
      0.0f, 0.0f,
      (float)mSettings.GetWindowWidth(),
      (float)mSettings.GetWindowHeight() );

    mProcActorRender.reset( newTickRefPoint );

    mProcCollisionDetector.reset( newTickRefPoint );

    GenerateNewScene( mSceneTopLeftX, mSceneTopLeftY, mSceneBottomRightX, mSceneBottomRightY );
    SpawnPlayer();

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

/**//**//**//**//**//**//**//**//**//**//**//**/
     // SpawnPlayer();
/**//**//**//**//**//**//**//**//**//**//**//**/
/* Player must not be simply spawned like this. There must be some "attention... ready... go! notice
   displayed, number of lives must be substracted, player game stats reseted ... */

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
