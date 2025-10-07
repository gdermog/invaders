//****************************************************************************************************
//! \file CInvInsertCoinScreen.h                                                                          
//! Module contains class CInvInsertCoinScreen, which implements singleton pattern for global logging                                                                      
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvInsertCoinScreen
#define H_CInvInsertCoinScreen

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>

#include <engine/CInvHiscoreList.h>

namespace Inv
{

  class CInvInsertCoinScreen
  {
    public:
    
      CInvInsertCoinScreen(
        const CInvSettings & settings,
        const CInvText & textCreator,
        CInvHiscoreList & hiscoreKeeper,
        CInvPrimitive & primitives,
        LPDIRECT3D9 pD3D,
        LPDIRECT3DDEVICE9 pd3dDevice,
        LPDIRECT3DVERTEXBUFFER9 pVB,
        LARGE_INTEGER tickReferncePoint );

    CInvInsertCoinScreen( const CInvInsertCoinScreen & ) = delete;
    CInvInsertCoinScreen & operator=( const CInvInsertCoinScreen & ) = delete;
    ~CInvInsertCoinScreen();
   
    bool MainLoop( 
      uint32_t & newScoreToEnter, 
      bool &gameStart, 
      ControlStateFlags_t controlState,
      ControlValue_t controlValue,
      LARGE_INTEGER actualTick );

    void Reset( LARGE_INTEGER newTickRefPoint );

  private:

    void FormatHighScore( uint32_t inScore, const std::string & inName );

    bool DrawHighScores( LARGE_INTEGER actualTick );

    bool DrawEnterCallsign(
      LARGE_INTEGER actualTick, 
      ControlValue_t controlValue, 
      bool & isDone );

    static const std::string mWelcomeHeader;
    static const std::string mPressToStart;
    static const std::string mYouQualified;
    static const std::string mEnterCallsign;

    static const uint32_t mHighScoreAreaVisibleLines;
    static const uint32_t mHighScoreRollingStepPerLine;
    static const float mHighScoreAreaFontRelativeSize;
    static const float mPressEnterFontRelativeSize;

    LARGE_INTEGER mTickReferencePoint;
    LARGE_INTEGER mDiffTick;

    float mLetterSize;
    float mHeaderXPos;
    float mHeaderYPos;

    float mHighScoreLetterSize;
    float mHighScoreTopLeftX;
    float mHighScoreTopLeftY;
    float mHighScoreBottomRightX;
    float mHighScoreBottomRightY;
    float mHighScoreWidth;
    float mHighScoreHeight;
    std::string mHighScoreLineContent;

    float mPressEnterLetterSize;
    float mPressEnterTopLeftX;
    float mPressEnterTopLeftY;

    float mQualifiedLetterSize;
    float mQualifiedTopLeftX;
    float mQualifiedTopLeftY;
    float mEnterCallsignTopLeftX;
    float mEnterCallsignTopLeftY;

    float mCallsignLetterSize;
    float mCallsignTopLeftX;
    float mCallsignTopLeftY;

    uint64_t mRollState;
    uint64_t mRollMax;

    const CInvSettings & mSettings;
    const CInvText & mTextCreator;
    CInvHiscoreList & mHiscoreKeeper;
    CInvPrimitive & mPrimitives;

    std::string mCurrentCallsign;
    ControlValue_t mLastControlValue;

    LPDIRECT3D9             mPD3D;
    LPDIRECT3DDEVICE9       mPd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 mPVB;

    std::unique_ptr<CInvSprite> mTitleSprite;
    uint32_t mNrOfTitleSprites;
    uint32_t mTitleSpriteImageSequenceNr;
    float mTitleSpriteWidth;
    std::vector<float> mTitleSpriteX;
    float mTitleSpriteY;

    std::shared_ptr<CInvEffect> mTitleSpriteAnimationEffect;
    std::shared_ptr<CInvEffect> mTitleSpriteShiftRotateEffect;
  };

} // namespace Inv

#endif
