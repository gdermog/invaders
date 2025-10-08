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
    const CInvText & textCreator,
    CInvPrimitive & primitives,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER tickReferencePoint ):

    mTickReferencePoint( tickReferencePoint ),
    mSettings( settings ),
    mTextCreator( textCreator ),
    mPrimitives( primitives ),
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

newScoreToEnter = uint32_t( 1000000.0 * ( (float)std::rand() / (float)RAND_MAX ) );
Sleep( 600 );
gameEnd = true;

    return true;

  } // CInvPlayItScreen::MainLoop

  //-------------------------------------------------------------------------------------------------

  void CInvPlayItScreen::Reset( LARGE_INTEGER newTickRefPoint )
  {

  } // CInvPlayItScreen::Reset


} // namespace Inv
