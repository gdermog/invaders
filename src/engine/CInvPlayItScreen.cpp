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
    CInvPrimitive & primitives,
    CInvSettingsRuntime & settingsRuntime,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER tickReferencePoint ):

    mTickReferencePoint( tickReferencePoint ),
    mSettings( settings ),
    mSpriteStorage( spriteStorage ),
    mPrimitives( primitives ),
    mSettingsRuntime( settingsRuntime ),
    mGameScene( settings, settingsRuntime, spriteStorage, primitives, pD3D, pd3dDevice, pVB, tickReferencePoint ),
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
