//****************************************************************************************************
//! \file CInvInsertCoinScreen.cpp                                                                         
//! Module contains class CInvInsertCoinScreen, which implements singleton pattern for global logging                                                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <engine/CInvInsertCoinScreen.h>

#include <CInvLogger.h>
#include <InvStringTools.h>

#include <graphics/CInvScissorGuard.h>


namespace Inv
{

  static const std::string lModLogId( "INSERTCOIN" );

  const std::string CInvInsertCoinScreen::mWelcomeHeader( "  welcome to invaders  " );

  const std::string CInvInsertCoinScreen::mPressToStart( "press enter to start" );

  const std::string CInvInsertCoinScreen::mEnterCallsign( "enter your callsign" );

  const uint32_t CInvInsertCoinScreen::mHighScoreAreaVisibleLines = 10;
  const float CInvInsertCoinScreen::mHighScoreAreaFontRelativeSize = 0.85;

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::CInvInsertCoinScreen(
    const CInvSettings & settings,
    const CInvText & textCreator,
    CInvHiscoreList & hiscoreKeeper,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER timeReferncePoin ):

    mLetterSize( 20.0f ),
    mHeaderXPos( 0.0f ),
    mHeaderYPos( 0.0f ),
    mHighScoreLetterSize( 12.0f ),
    mHighScoreTopLeftX( 0.0f ),
    mHighScoreTopLeftY( 0.0f ),
    mHighScoreBottomRightX( 0.0f ),
    mHighScoreBottomRightY( 0.0f ),
    mHighScoreWidth( 0.0f ),
    mHighScoreHeight( 0.0f ),
    mHighScoreLineContent(),
    mRollState(0),
    mSettings( settings ),
    mTextCreator( textCreator ),
    mHiscoreKeeper( hiscoreKeeper ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mTimeReferncePoint( timeReferncePoin )
  {

    auto windowWidth = mSettings.GetWindowWidth();
    auto nrOfLetters = mWelcomeHeader.size();
    mLetterSize = (float)windowWidth / (float)nrOfLetters;

    mHighScoreLetterSize = mLetterSize * mHighScoreAreaFontRelativeSize;
    mHighScoreWidth = CInvHiscoreList::mMaxHiscoreLineLen * mHighScoreLetterSize;
    mHighScoreHeight = mHighScoreAreaVisibleLines * mHighScoreLetterSize;

    mHighScoreTopLeftX = ( (float)mSettings.GetWindowWidth() - mHighScoreWidth ) * 0.5;
    mHighScoreTopLeftY = mLetterSize + mHighScoreLetterSize;
    mHighScoreBottomRightX = mHighScoreTopLeftX + mHighScoreWidth;
    mHighScoreBottomRightY = mHighScoreTopLeftY + mHighScoreHeight;

    mHighScoreLineContent.reserve( CInvHiscoreList::mMaxHiscoreLineLen );

  } // CInvInsertCoinScreen::CInvInsertCoinScreen

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::~CInvInsertCoinScreen()
  {
  } // CInvInsertCoinScreen::~CInvInsertCoinScreen

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::MainLoop(
    uint32_t & newScoreToEnter,
    bool & gameStart,
    LARGE_INTEGER actualTimePoint )
  {

    //------ Main window header ---------------------------------------------------------------------

    mTextCreator.Draw( mWelcomeHeader.c_str(), mHeaderXPos, mHeaderYPos, mLetterSize );


    //------ Hiscore area ---------------------------------------------------------------------------

    auto hiscores = mHiscoreKeeper.GetHiscoreList();

    float shift = 0.0;

/**//**//**//**/
    mRollMax = hiscores.size() * 20;
/**//**//**//**/
    
    if( mRollState <= mRollMax )
    {
      shift = (float)( mRollMax-mRollState) * mHighScoreLetterSize / 20 ;
      ++mRollState;
    }

    if( !hiscores.empty() )
    {
      CInvScissorGuard scissor(
        mPd3dDevice,
        {
          (LONG)mHighScoreTopLeftX, (LONG)mHighScoreTopLeftY,
          (LONG)mHighScoreBottomRightX, (LONG)mHighScoreBottomRightY
        } );

      float lineY = 0;



      for( auto hs = hiscores.begin(); hs != hiscores.end(); ++hs )
      {

        FormatHighScore( hs->first, hs->second );

        mTextCreator.DrawFromRight( 
          mHighScoreLineContent,
          mHighScoreTopLeftX, 
          mHighScoreTopLeftY + lineY - shift,
          mHighScoreWidth,
          mHighScoreLetterSize );

        lineY += mHighScoreLetterSize;
      } // for

    } // if

    return true;
  } // CInvInsertCoinScreen::MainLoop

  //-------------------------------------------------------------------------------------------------

  void CInvInsertCoinScreen::FormatHighScore( uint32_t inScore, const std::string & inName )
  {
    uint32_t ones = inScore % 1000;
    uint32_t thousands = ( inScore / 1000 ) % 1000;
    uint32_t millions = ( inScore / 1000000 ) % 1000;

    mHighScoreLineContent.clear();

    if( 0 < millions )
      mHighScoreLineContent += FormatStr( "%3u", millions );
    else
      mHighScoreLineContent += "   ";

    mHighScoreLineContent += " ";

    if( 0 == thousands && 0 == millions )
      mHighScoreLineContent += "   ";
    else if( 0 < thousands && 0 == millions )
      mHighScoreLineContent += FormatStr( "%3u", thousands );
    else
      mHighScoreLineContent += FormatStr( "%03u", thousands );

    mHighScoreLineContent += " ";

    if( 0 == thousands && 0 == millions && 0 == ones )
      mHighScoreLineContent += "   ";
    else if( 0 < ones && 0 == thousands && 0 == millions )
      mHighScoreLineContent += FormatStr( "%3u", ones );
    else
      mHighScoreLineContent += FormatStr( "%03u", ones );

    mHighScoreLineContent += "   ";
    mHighScoreLineContent += inName;

  } // CInvInsertCoinScreen::FormatHighScore

  //-------------------------------------------------------------------------------------------------

} // namespace Inv
