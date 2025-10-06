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

  const std::string CInvInsertCoinScreen::mYouQualified( "  you qualified for the honor roll  " );

  const std::string CInvInsertCoinScreen::mEnterCallsign( "enter your callsign" );

  const uint32_t CInvInsertCoinScreen::mHighScoreAreaVisibleLines = 10;
  const uint32_t CInvInsertCoinScreen::mHighScoreRollingStepPerLine = 20;
  const float CInvInsertCoinScreen::mHighScoreAreaFontRelativeSize = 0.85;
  const float CInvInsertCoinScreen::mPressEnterFontRelativeSize = 0.90;

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::CInvInsertCoinScreen(
    const CInvSettings & settings,
    const CInvText & textCreator,
    CInvHiscoreList & hiscoreKeeper,
    CInvPrimitive & primitives,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER timeReferncePoint ):

    mPrimitives( primitives ),
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
    mPressEnterLetterSize( 16.0f ),
    mPressEnterTopLeftX( 0.0f ),
    mPressEnterTopLeftY( 0.0f ),
    mQualifiedLetterSize( 16.0f ),
    mQualifiedTopLeftX( 0.0f ),
    mQualifiedTopLeftY( 0.0f ),
    mEnterCallsignTopLeftX( 0.0f ),
    mEnterCallsignTopLeftY( 0.0f ),
    mCallsignLetterSize( 20.0f ),
    mCallsignTopLeftX( 0.0f ),
    mCallsignTopLeftY( 0.0f ),
    mRollState(0),
    mSettings( settings ),
    mTextCreator( textCreator ),
    mHiscoreKeeper( hiscoreKeeper ),
    mCurrentCallsign(),
    mLastControlValue( 0 ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mTimeReferencePoint( timeReferncePoint )
  {

    auto windowWidth = mSettings.GetWindowWidth();
    auto nrOfLetters = mWelcomeHeader.size();
    mLetterSize = (float)windowWidth / (float)nrOfLetters;

    mHeaderXPos = 0.0f;
    mHeaderYPos = mLetterSize * 0.1f;

    mHighScoreLetterSize = mLetterSize * mHighScoreAreaFontRelativeSize;
    mHighScoreWidth = CInvHiscoreList::mMaxHiscoreLineLen * mHighScoreLetterSize;
    mHighScoreHeight = mHighScoreAreaVisibleLines * mHighScoreLetterSize;

    mHighScoreTopLeftX = ( (float)mSettings.GetWindowWidth() - mHighScoreWidth ) * 0.5f;
    mHighScoreTopLeftY = mLetterSize + mHighScoreLetterSize;
    mHighScoreBottomRightX = mHighScoreTopLeftX + mHighScoreWidth;
    mHighScoreBottomRightY = mHighScoreTopLeftY + mHighScoreHeight;

    mHighScoreLineContent.reserve( CInvHiscoreList::mMaxHiscoreLineLen );

    mPressEnterLetterSize = mLetterSize * mPressEnterFontRelativeSize;
    mPressEnterTopLeftX = 
      ( (float)mSettings.GetWindowWidth() - mPressToStart.size() * mPressEnterLetterSize ) * 0.5f;
    mPressEnterTopLeftY =  (float)mSettings.GetWindowHeight() - mPressEnterLetterSize * 1.1f;

    mQualifiedLetterSize = (float)mSettings.GetWindowWidth() / (float)mYouQualified.size();
    if( mLetterSize * 0.9f < mQualifiedLetterSize )
      mQualifiedLetterSize = mLetterSize * 0.9f;

    mQualifiedTopLeftX = ( (float)mSettings.GetWindowWidth() - mYouQualified.size() * mQualifiedLetterSize ) * 0.5f;
    mQualifiedTopLeftY = mHeaderYPos + 3 * mLetterSize;

    mEnterCallsignTopLeftX = 
      ( (float)mSettings.GetWindowWidth() - mEnterCallsign.size() * mQualifiedLetterSize ) * 0.5f;
    mEnterCallsignTopLeftY = mQualifiedTopLeftY + mQualifiedLetterSize * 1.5f;

    mCallsignLetterSize = mLetterSize * 1.5f;
    mCallsignTopLeftX =
      ( (float)mSettings.GetWindowWidth() - CInvHiscoreList::mMaxHiscoreNameLen * mCallsignLetterSize ) * 0.5f;
    mCallsignTopLeftY = mEnterCallsignTopLeftY + mQualifiedLetterSize * 4.5f;

  } // CInvInsertCoinScreen::CInvInsertCoinScreen

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::~CInvInsertCoinScreen()
  {
  } // CInvInsertCoinScreen::~CInvInsertCoinScreen

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::MainLoop(
    uint32_t & newScoreToEnter,
    bool & gameStart,
    ControlStateFlags_t controlState,
    ControlValue_t controlValue,
    LARGE_INTEGER actualTimePoint )
  {

    gameStart = false;

    mTextCreator.Draw( mWelcomeHeader.c_str(), mHeaderXPos, mHeaderYPos, mLetterSize );

    if( 0 == newScoreToEnter )
    {
      gameStart = ControlStateHave( controlState, ControlState_t::kStart );
      return DrawHighScores();
    }

    bool isDone = false;
    auto retVal = DrawEnterCallsign( controlValue, isDone );

    if( isDone )
    {
      mHiscoreKeeper.AddNewHiscore( newScoreToEnter, mCurrentCallsign );
      newScoreToEnter = 0;
      mLastControlValue = 0;
      mRollState = 0;
      mRollMax = mHiscoreKeeper.GetHiscoreList().size() * mHighScoreRollingStepPerLine;
    } // if

    return retVal;

  } // CInvInsertCoinScreen::MainLoop

  //-------------------------------------------------------------------------------------------------

  void CInvInsertCoinScreen::Reset( LARGE_INTEGER newTimeRefPoint )
  {
    mTimeReferencePoint = newTimeRefPoint;
    auto &hiscores = mHiscoreKeeper.GetHiscoreList();
    mRollMax = hiscores.size() * mHighScoreRollingStepPerLine;
    mLastControlValue = 0;
    mRollState = 0;
    mRollMax = mHiscoreKeeper.GetHiscoreList().size() * mHighScoreRollingStepPerLine;
  } // CInvInsertCoinScreen::Reset

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

    if( inName.size() < CInvHiscoreList::mMaxHiscoreNameLen )
      mHighScoreLineContent += std::string( CInvHiscoreList::mMaxHiscoreNameLen - inName.size(), ' ' );

    mHighScoreLineContent += inName;

  } // CInvInsertCoinScreen::FormatHighScore

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::DrawHighScores()
  {
    auto &hiscores = mHiscoreKeeper.GetHiscoreList();
    float shift = 0.0;

    if( mRollState <= mRollMax )
    {
      shift = (float)( mRollMax - mRollState ) * mHighScoreLetterSize / 20;
      ++mRollState;
    } // if

    if( !hiscores.empty() )
    {
      CInvScissorGuard scissor(
        mPd3dDevice,
        {
          (LONG)mHighScoreTopLeftX, (LONG)mHighScoreTopLeftY,
          (LONG)mHighScoreBottomRightX, (LONG)mHighScoreBottomRightY
        } );

      float lineYPos = mHighScoreTopLeftY - shift;
      for( auto hs = hiscores.begin(); hs != hiscores.end(); ++hs )
      {

        if( mHighScoreTopLeftY <= lineYPos + mHighScoreLetterSize &&
          lineYPos - mHighScoreLetterSize <= mHighScoreBottomRightY )
        {               // Text is (at least partially) visible, so it must be rendered

          FormatHighScore( hs->first, hs->second );

          mTextCreator.DrawFromRight(
            mHighScoreLineContent,
            mHighScoreTopLeftX,
            lineYPos,
            mHighScoreWidth,
            mHighScoreLetterSize );
        } // if

        lineYPos += mHighScoreLetterSize;

      } // for

    } // if

    mTextCreator.Draw( mPressToStart.c_str(), mPressEnterTopLeftX, mPressEnterTopLeftY, mPressEnterLetterSize );

    return true;
  } // CInvInsertCoinScreen::DrawHighScores

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::DrawEnterCallsign( ControlValue_t controlValue, bool & isDone )
  {
    isDone = false;

    mTextCreator.Draw( mYouQualified.c_str(), mQualifiedTopLeftX, mQualifiedTopLeftY, mQualifiedLetterSize );
    mTextCreator.Draw( mEnterCallsign.c_str(), mEnterCallsignTopLeftX, mEnterCallsignTopLeftY, mQualifiedLetterSize );

    if( 0 != mLastControlValue && 0 == controlValue )
    {
      if( VK_BACK == mLastControlValue && !mCurrentCallsign.empty() )
        mCurrentCallsign.pop_back();
      else if( VK_RETURN == mLastControlValue )
        isDone = true;
      else if( ( 'A' <= mLastControlValue && mLastControlValue <= 'Z' ) ||
               ( 'a' <= mLastControlValue && mLastControlValue <= 'z' ) ||
               ( '0' <= mLastControlValue && mLastControlValue <= '9' ) )
      {
        if( mCurrentCallsign.size() < CInvHiscoreList::mMaxHiscoreNameLen )
          mCurrentCallsign += (char)std::tolower( (int)mLastControlValue );
      } // else if
    }

    mLastControlValue = controlValue;

    auto letters = mCurrentCallsign.size();
    auto lettersLeft = 
      letters < CInvHiscoreList::mMaxHiscoreNameLen ?
      CInvHiscoreList::mMaxHiscoreNameLen - letters : 0;

    if( 0 < letters )
      mTextCreator.Draw( mCurrentCallsign, mCallsignTopLeftX, mCallsignTopLeftY, mCallsignLetterSize );

    if( 0 < lettersLeft )
    {
      mPrimitives.DrawLine(
        mCallsignTopLeftX + letters * mCallsignLetterSize,
        mCallsignTopLeftY + mCallsignLetterSize,
        mCallsignTopLeftX + ( letters + lettersLeft ) * mCallsignLetterSize,
        mCallsignTopLeftY + mCallsignLetterSize,
        D3DCOLOR_ARGB( 255, 0, 255, 0 ) );
    } // if

    return true;
  } // CInvInsertCoinScreen::DrawEnterCallsign

  //-------------------------------------------------------------------------------------------------

} // namespace Inv
