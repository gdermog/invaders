//****************************************************************************************************
//! \file CInvInsertCoinScreen.cpp
//! Module defines class CInvInsertCoinScreen, which implements implements the "Insert Coin" screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/CInvInsertCoinScreen.h>

#include <CInvLogger.h>
#include <InvStringTools.h>

#include <graphics/CInvScissorGuard.h>
#include <graphics/CInvEffectSpriteAnimation.h>
#include <graphics/CInvEffectSpriteShiftRotate.h>

namespace Inv
{

  static const std::string lModLogId( "INSERTCOIN" );

  const std::string CInvInsertCoinScreen::mWelcomeHeader( "  welcome to invaders  " );

  const std::string CInvInsertCoinScreen::mPressToStart( "press enter to start" );

  const std::string CInvInsertCoinScreen::mYouQualified( "  you qualified for the honor roll  " );

  const std::string CInvInsertCoinScreen::mEnterCallsign( "enter your callsign" );

  const uint32_t CInvInsertCoinScreen::mHighScoreAreaVisibleLines = 10;
  const uint32_t CInvInsertCoinScreen::mHighScoreRollingStepPerLine = 20;
  const float CInvInsertCoinScreen::mHighScoreAreaFontRelativeSize = 0.85f;
  const float CInvInsertCoinScreen::mPressEnterFontRelativeSize = 0.90f;

  //-------------------------------------------------------------------------------------------------


  CInvInsertCoinScreen::CInvInsertCoinScreen(
    const CInvSettings & settings,
    const CInvSpriteStorage & spriteStorage,
    CInvHiscoreList & hiscoreKeeper,
    CInvPrimitive & primitives,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB,
    LARGE_INTEGER tickReferncePoint ):

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
    mTextCreator( {}, settings, pd3dDevice ),
    mHiscoreKeeper( hiscoreKeeper ),
    mSpriteStorage( spriteStorage ),
    mCurrentCallsign(),
    mLastControlValue( 0 ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mTitleSprite( nullptr ),
    mNrOfTitleSprites( 0 ),
    mTitleSpriteImageSequenceNr( 0 ),
    mTitleSpriteWidth( 20.0f ),
    mTitleSpriteX(),
    mTitleSpriteY( 0.0f ),
    mTitleSpriteAnimationEffect( nullptr ),
    mTitleSpriteShiftRotateEffect( nullptr ),
    mTickReferencePoint( tickReferncePoint ),
    mDiffTick{ 0 }
  {

    auto windowWidth = mSettings.GetWidth();
    auto nrOfLetters = mWelcomeHeader.size();
    mLetterSize = (float)windowWidth / (float)nrOfLetters;

    mHeaderXPos = 0.0f;
    mHeaderYPos = mLetterSize * 0.1f;

    mHighScoreLetterSize = mLetterSize * mHighScoreAreaFontRelativeSize;
    mHighScoreWidth = CInvHiscoreList::mMaxHiscoreLineLen * mHighScoreLetterSize;
    mHighScoreHeight = mHighScoreAreaVisibleLines * mHighScoreLetterSize;

    mHighScoreTopLeftX = ( (float)mSettings.GetWidth() - mHighScoreWidth ) * 0.5f;
    mHighScoreTopLeftY = mLetterSize + mHighScoreLetterSize;
    mHighScoreBottomRightX = mHighScoreTopLeftX + mHighScoreWidth;
    mHighScoreBottomRightY = mHighScoreTopLeftY + mHighScoreHeight;

    mHighScoreLineContent.reserve( CInvHiscoreList::mMaxHiscoreLineLen );

    mPressEnterLetterSize = mLetterSize * mPressEnterFontRelativeSize;
    mPressEnterTopLeftX =
      ( (float)mSettings.GetWidth() - mPressToStart.size() * mPressEnterLetterSize ) * 0.5f;
    mPressEnterTopLeftY =  (float)mSettings.GetHeight() - mPressEnterLetterSize * 1.1f;

    mQualifiedLetterSize = (float)mSettings.GetWidth() / (float)mYouQualified.size();
    if( mLetterSize * 0.9f < mQualifiedLetterSize )
      mQualifiedLetterSize = mLetterSize * 0.9f;

    mQualifiedTopLeftX = ( (float)mSettings.GetWidth() - mYouQualified.size() * mQualifiedLetterSize ) * 0.5f;
    mQualifiedTopLeftY = mHeaderYPos + 3 * mLetterSize;

    mEnterCallsignTopLeftX =
      ( (float)mSettings.GetWidth() - mEnterCallsign.size() * mQualifiedLetterSize ) * 0.5f;
    mEnterCallsignTopLeftY = mQualifiedTopLeftY + mQualifiedLetterSize * 1.5f;

    mCallsignLetterSize = mLetterSize * 1.5f;
    mCallsignTopLeftX =
      ( (float)mSettings.GetWidth() - CInvHiscoreList::mMaxHiscoreNameLen * mCallsignLetterSize ) * 0.5f;
    mCallsignTopLeftY = mEnterCallsignTopLeftY + mQualifiedLetterSize * 4.5f;

    mTitleSprite = mSpriteStorage.GetSprite( "SAUCEREXPL" /*"PINK"*/ );
    if( nullptr == mTitleSprite )
      LOG << "Title sprite not found in storage.";
    else
    {
      mTitleSpriteImageSequenceNr = (uint32_t)mTitleSprite->GetNumberOfImages();
      mNrOfTitleSprites = 5;
      mTitleSpriteWidth = 80.0f;
      float testSpritesDistance = (float)mSettings.GetWidth() / (float)( mNrOfTitleSprites + 1 );
      mTitleSpriteX.reserve( mNrOfTitleSprites );
      for( uint32_t i = 0; i < mNrOfTitleSprites; ++i )
        mTitleSpriteX.push_back( testSpritesDistance * (float)( i + 1 ) );
      mTitleSpriteY = 0.5f * ( mPressEnterTopLeftY + mHighScoreBottomRightY );

      auto * animEff = new CInvEffectSpriteAnimation( mSettings, mPd3dDevice, 1 );
      animEff->SetPace( 6 );
      mTitleSpriteAnimationEffect = std::shared_ptr<CInvEffect>( animEff );
      mTitleSprite->AddEffect( mTitleSpriteAnimationEffect );

      auto * shiftRotEff = new CInvEffectSpriteShiftRotate( mSettings, mPd3dDevice, 2 );
      shiftRotEff->SetShift( mTitleSpriteWidth * 0.2f, 0.0f );
      shiftRotEff->SetPace( 300 );
      mTitleSpriteShiftRotateEffect = std::shared_ptr<CInvEffect>( shiftRotEff );
      mTitleSprite->AddEffect( mTitleSpriteShiftRotateEffect );

    } // else

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
    LARGE_INTEGER actualTick )
  {

    gameStart = false;

    mTextCreator.SetText( mWelcomeHeader );
    mTextCreator.Draw(
      mHeaderXPos, mHeaderYPos, mLetterSize,
      mTickReferencePoint, actualTick, mDiffTick );

    if( 0 == newScoreToEnter )
    {
      gameStart = ControlStateHave( controlState, ControlState_t::kStart );
      return DrawHighScores( actualTick );
    }

    bool isDone = false;
    auto retVal = DrawEnterCallsign( actualTick, controlValue, isDone );

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

  void CInvInsertCoinScreen::Reset( LARGE_INTEGER newTickRefPoint )
  {
    mTickReferencePoint = newTickRefPoint;
    auto &hiscores = mHiscoreKeeper.GetHiscoreList();
    mRollMax = hiscores.size() * mHighScoreRollingStepPerLine;
    mLastControlValue = 0;
    mRollState = 0;
    mRollMax = mHiscoreKeeper.GetHiscoreList().size() * mHighScoreRollingStepPerLine;

  } // CInvInsertCoinScreen::Reset

  //-------------------------------------------------------------------------------------------------

  void CInvInsertCoinScreen::FormatHighScore( uint32_t inScore, const std::string & inName )
  {
    mHighScoreLineContent = FormatScoreNumber( inScore );

    mHighScoreLineContent += "   ";

    if( inName.size() < CInvHiscoreList::mMaxHiscoreNameLen )
      mHighScoreLineContent += std::string( CInvHiscoreList::mMaxHiscoreNameLen - inName.size(), ' ' );

    mHighScoreLineContent += inName;

  } // CInvInsertCoinScreen::FormatHighScore

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::DrawHighScores( LARGE_INTEGER actualTick )
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

          mTextCreator.SetText( mHighScoreLineContent );
          mTextCreator.DrawFromRight(
            mHighScoreTopLeftX,
            lineYPos,
            mHighScoreWidth,
            mHighScoreLetterSize,
            mTickReferencePoint, actualTick, mDiffTick );
        } // if

        lineYPos += mHighScoreLetterSize;

      } // for

    } // if

    if( nullptr != mTitleSprite && 0 < mTitleSpriteImageSequenceNr )
    {
      LARGE_INTEGER diffTick;
      for( uint32_t i = 0; i < mNrOfTitleSprites; ++i )
      {
        diffTick.QuadPart = - (LONGLONG)(2 * i * mNrOfTitleSprites);
        mTitleSprite->Draw(
          mTitleSpriteX[i],
          mTitleSpriteY,
          mTitleSpriteWidth,
          mTitleSpriteWidth,
          mTickReferencePoint,
          actualTick,
          diffTick );
      } // for
    } // if

    mTextCreator.SetText( mPressToStart );
    mTextCreator.Draw(
      mPressEnterTopLeftX, mPressEnterTopLeftY, mPressEnterLetterSize,
      mTickReferencePoint, actualTick, mDiffTick );

    return true;
  } // CInvInsertCoinScreen::DrawHighScores

  //-------------------------------------------------------------------------------------------------

  bool CInvInsertCoinScreen::DrawEnterCallsign(
    LARGE_INTEGER actualTick,
    ControlValue_t controlValue,
    bool & isDone )
  {
    isDone = false;

    mTextCreator.SetText( mYouQualified );
    mTextCreator.Draw(
      mQualifiedTopLeftX, mQualifiedTopLeftY, mQualifiedLetterSize,
      mTickReferencePoint, actualTick, mDiffTick );

    mTextCreator.SetText( mEnterCallsign );
    mTextCreator.Draw(
      mEnterCallsignTopLeftX, mEnterCallsignTopLeftY, mQualifiedLetterSize,
      mTickReferencePoint, actualTick, mDiffTick );

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
    {
      mTextCreator.SetText( mCurrentCallsign );
      mTextCreator.Draw(
        mCallsignTopLeftX, mCallsignTopLeftY, mCallsignLetterSize,
        mTickReferencePoint, actualTick, mDiffTick );
    }

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
