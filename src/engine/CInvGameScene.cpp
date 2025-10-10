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
    mCollisionTest( settings, pd3dDevice ),
    mEnTTRegistry(),
    mEntityFactory( settings, spriteStorage, mEnTTRegistry, pD3D, pd3dDevice, pVB ),
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

    mProcGarbageCollector( tickReferencePoint ),
    mProcActorStateSelector( tickReferencePoint ),
    mProcEntitySpawner( tickReferencePoint, mEntityFactory ),
    mProcActorMover( tickReferencePoint ),
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

    std::vector<std::pair<uint32_t, std::string>> alienRows = //{ {1,"PINK"} };
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

  bool CInvGameScene::SpawnPlayer( )
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

    return true;
  } // CInvGameScene::SpawnPlayer

  //-------------------------------------------------------------------------------------------------

  bool CInvGameScene::RenderActualScene( LARGE_INTEGER actualTickPoint )
  {

    mProcGarbageCollector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // Removes entities marked as inactive from the registry, noticing
                        // main scena class if demanded.

    mProcActorStateSelector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcEntitySpawner.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );

    mProcActorMover.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcActorOutOfSceneCheck.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities arfe moved according to their velocity, entities out of scene
                        // are marked as inactive and will be removed by garbage collector in next loop.

    mProcActorRender.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
                        // All entities are rendered according to their graphics component and status

    mProcCollisionDetector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    for( auto & item: mProcCollisionDetector.mCollidedPairs )
      EliminateEntity( item.second );
                        // Missile hits and alien-player collisions are handled

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
    return true;
  }

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::EntityJustPruned( entt::entity & entity )
  {
    auto [ entId, entBehave, entStatus ] = mEnTTRegistry.try_get<cpId, cpPlayBehave, cpPlayStatus>(entity);

    if( nullptr == entId || entId->active )
      return;

    if( nullptr != entBehave && nullptr != entStatus )
    {                   // Player entity elimination from game scene is done, appropriate measures¨
                        // must be taken (respawn, reduce number of lives, end of game etc.)

      LOG << lModLogId << "Player was pruned from game scene.";

    } // if

  } // CInvGameScene::EntityJustPruned

} // namespace Inv
