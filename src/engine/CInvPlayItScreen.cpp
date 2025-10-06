//****************************************************************************************************
//! \file CInvPlayItScreen.cpp                                                                         
//! Module contains class CInvPlayItScreen, which implements...                                                                   
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
    LARGE_INTEGER timeReferencePoint ):

    mTimeReferencePoint( timeReferencePoint ),
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
    LARGE_INTEGER actualTimePoint )
  {

newScoreToEnter = uint32_t( 1000000.0 * ( (float)std::rand() / (float)RAND_MAX ) );
Sleep( 600 );
gameEnd = true;

    return true;

  } // CInvPlayItScreen::MainLoop

  //-------------------------------------------------------------------------------------------------

  void CInvPlayItScreen::Reset( LARGE_INTEGER newTimeRefPoint )
  {

  } // CInvPlayItScreen::Reset


} // namespace Inv
