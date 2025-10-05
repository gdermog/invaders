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
#include <d3dx9.h>

#include <InvGlobals.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvText.h>

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
        LPDIRECT3D9 pD3D,
        LPDIRECT3DDEVICE9 pd3dDevice,
        LPDIRECT3DVERTEXBUFFER9 pVB,
        LARGE_INTEGER timeReferncePoint );

    CInvInsertCoinScreen( const CInvInsertCoinScreen & ) = delete;
    CInvInsertCoinScreen & operator=( const CInvInsertCoinScreen & ) = delete;
    ~CInvInsertCoinScreen();
   
    bool MainLoop( 
      uint32_t & newScoreToEnter, 
      bool &gameStart, 
      LARGE_INTEGER actualTimePoint );

    void UpdateTimeReferencePoint( LARGE_INTEGER newTimeRefPoint )
    { mTimeReferncePoint = newTimeRefPoint; }

  private:

    void FormatHighScore( uint32_t inScore, const std::string & inName );

    LARGE_INTEGER mTimeReferncePoint;

    static const std::string mWelcomeHeader;

    static const std::string mPressToStart;

    static const std::string mEnterCallsign;

    static const uint32_t mHighScoreAreaVisibleLines;
    static const float mHighScoreAreaFontRelativeSize;

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

    uint64_t mRollState;
    uint64_t mRollMax;

    const CInvSettings & mSettings;
    const CInvText & mTextCreator;
    CInvHiscoreList & mHiscoreKeeper;

    LPDIRECT3D9             mPD3D;
    LPDIRECT3DDEVICE9       mPd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 mPVB;

  };

} // namespace Inv

#endif
