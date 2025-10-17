//****************************************************************************************************
//! \file CInvPlayItScreen.cpp
//! Module declares class CInvPlayItScreen, which implements the "Play It" screen, where the actual
//! game is played.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/CInvPlayItScreen.h>

namespace Inv
{

  //-------------------------------------------------------------------------------------------------

  CInvPlayItScreen::CInvPlayItScreen(
    const CInvSettings & settings,
    const CInvSpriteStorage & spriteStorage,
    const CInvSoundsStorage & soundStorage,
    const CInvBackground & background,
    CInvPrimitive & primitives,
    CInvSettingsRuntime & settingsRuntime,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER tickReferencePoint ):

    mTickReferencePoint( tickReferencePoint ),
    mSettings( settings ),
    mSpriteStorage( spriteStorage ),
    mSoundStorage( soundStorage ),
    mBackground( background ),
    mPrimitives( primitives ),
    mSettingsRuntime( settingsRuntime ),

    mAlienBosses
    {
      { 10u, {
           10u,             // mTypeId;
           "SAUCER",        // mSpriteId
           false,           // mMirrorIfFromRight
           1.5f,            // mAnimationLength
           5000u,           // mPoints
           0u,              // mIsSpawned
           2u,              // mMaxSpawned
           0.001f,          // mSpawnProbability
           80.0f,           // mSize - will be recalculated in GenerateNewScene() according to scene size
           40.0f,           // mSpawnY - will be recalculated in GenerateNewScene()
           0.0f,            // mSpawnXLeft - will be recalculated in GenerateNewScene()
           0.0f,            // mSpawnXRight - will be recalculated in GenerateNewScene()
           1.0f             // mSpeedCoef
      }},
      { 20u, {
           20u,             // mTypeId
           "PACVADER",      // mSpriteId
           true,            // mMirrorIfFromRight
           0.5f,            // mAnimationLength
           15000u,          // mPoints
           0u,              // mIsSpawned
           1u,              // mMaxSpawned
           0.0002f,         // mSpawnProbability
           80.0f,           // mSize - will be recalculated in GenerateNewScene() according to scene size
           -1.0f,           // mSpawnY - will be adjusted according to player position
           0.0f,            // mSpawnXLeft - will be recalculated in GenerateNewScene()
           0.0f,            // mSpawnXRight - will be recalculated in GenerateNewScene()
           2.0f             // mSpeedCoef
      }}
    },

    mGameScene(
      settings,
      settingsRuntime,
      spriteStorage,
      soundStorage,
      background,
      primitives,
      mAlienBosses,
      pD3D,
      pd3dDevice,
      pVB,
      tickReferencePoint ),

    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mActualScore( 0 )
  {
  } // CInvPlayItScreen::CInvPlayItScreen

  //-------------------------------------------------------------------------------------------------

  CInvPlayItScreen::~CInvPlayItScreen()
  {

  } // CInvPlayItScreen::~CInvPlayItScreen

  //-------------------------------------------------------------------------------------------------

  bool CInvPlayItScreen::MainLoop(
    uint32_t & newScoreToEnter,
    bool & gameEnd,
    ControlStateFlags_t controlState,
    ControlValue_t controlValue,
    LARGE_INTEGER actualTickPoint )
  {

    bool retVal = true;

    retVal |= mGameScene.RenderActualScene( actualTickPoint, controlState, controlValue );
    retVal |= mGameScene.RenderStatusBar( actualTickPoint );
    retVal |= mGameScene.PlayerEntryProcessing( actualTickPoint );

    if( mGameScene.GameOver() )
    {
      newScoreToEnter = mGameScene.GetCurrentScore();
      if( newScoreToEnter < mSettings.GetMinScore() )
        newScoreToEnter = 0u;
                        // Did not qualify for hiscore entry

      for( auto & bossIt : mAlienBosses )
      {                 // If the game ends while there is a boss active,
                        // its "running" sound must be stoped.
        auto & boss = bossIt.second;
        boss.mIsSpawned = 0u;
        mSoundStorage.StopSound( boss.mSpriteId + "LOOP" );
      } // for

      gameEnd = true;
    } // if

    return retVal;

  } // CInvPlayItScreen::MainLoop

  //-------------------------------------------------------------------------------------------------

  void CInvPlayItScreen::Reset( LARGE_INTEGER newTickRefPoint )
  {
    mSettingsRuntime.ResetToDefaults();
    mSettingsRuntime.Preprint();

    mGameScene.Reset( newTickRefPoint );

  } // CInvPlayItScreen::Reset


} // namespace Inv
