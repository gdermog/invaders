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

    mProcActorStateSelector( tickReferencePoint ),
    mProcEntitySpawner( tickReferencePoint, mEntityFactory ),
    mProcActorMover( tickReferencePoint ),
    mProcActorOutOfSceneCheck( tickReferencePoint, 0.0f, 0.0f, (float)settings.GetWindowWidth(), (float)settings.GetWindowHeight() ),
    mProcGarbageCollector( tickReferencePoint ),
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

  bool CInvGameScene::RenderActualScene( LARGE_INTEGER actualTickPoint )
  {

    mProcActorStateSelector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcEntitySpawner.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcActorMover.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcActorOutOfSceneCheck.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcGarbageCollector.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );
    mProcActorRender.update( mEnTTRegistry, actualTickPoint, mDiffTickPoint );

    return true;

  } // CInvGameScene::RenderActualScene

  //-------------------------------------------------------------------------------------------------

  void CInvGameScene::Reset( LARGE_INTEGER newTickRefPoint )
  {
    mTickReferencePoint = newTickRefPoint;
    mEnTTRegistry.clear();

    mProcActorStateSelector.reset( newTickRefPoint );
    mProcActorRender.reset( newTickRefPoint );

    GenerateNewScene( mSceneTopLeftX, mSceneTopLeftY, mSceneBottomRightX, mSceneBottomRightY );
  } // CInvGameScene::Reset

} // namespace Inv
