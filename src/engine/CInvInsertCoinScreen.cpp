//****************************************************************************************************
//! \file CInvInsertCoinScreen.cpp                                                                         
//! Module contains class CInvInsertCoinScreen, which implements singleton pattern for global logging                                                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <engine/CInvInsertCoinScreen.h>

namespace Inv
{

  const std::string CInvInsertCoinScreen::mWelcomeHeader( "  welcome to invaders  " );

  const std::string CInvInsertCoinScreen::mPressToStart( "press enter to start" );

  const std::string CInvInsertCoinScreen::mEnterCallsign( "enter your callsign" );

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::CInvInsertCoinScreen(
    const CInvSettings & settings,
    const CInvText & textCreator,
    CInvHiscoreList & hiscoreKeeper,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB ):

    mLetterSize( 20.0f ),
    mHeaderXPos( 0.0f ),
    mHeaderYPos( 0.0f ),
    mSettings( settings ),
    mTextCreator( textCreator ),
    mHiscoreKeeper( hiscoreKeeper ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB )
  {

    auto windowWidth = mSettings.GetWindowWidth();
    auto nrOfLetters = mWelcomeHeader.size();
    mLetterSize = (float)windowWidth / ( (float)nrOfLetters + 4.0f );




  } // CInvInsertCoinScreen::CInvInsertCoinScreen

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::~CInvInsertCoinScreen()
  {
  } // CInvInsertCoinScreen::~CInvInsertCoinScreen

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::MainLoop( uint32_t & newScoreToEnter, bool & gameStart )
  {

    mTextCreator.Draw( mWelcomeHeader.c_str(), mHeaderXPos, mHeaderYPos, mLetterSize );

    return true;
  } // CInvInsertCoinScreen::MainLoop

  //-------------------------------------------------------------------------------------------------

} // namespace Inv
