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
        LPDIRECT3DVERTEXBUFFER9 pVB );

    CInvInsertCoinScreen( const CInvInsertCoinScreen & ) = delete;
    CInvInsertCoinScreen & operator=( const CInvInsertCoinScreen & ) = delete;
    ~CInvInsertCoinScreen();
   

    bool MainLoop( uint32_t & newScoreToEnter, bool &gameStart );


  private:

    static const std::string mWelcomeHeader;

    static const std::string mPressToStart;

    static const std::string mEnterCallsign;

    float mLetterSize;
    float mHeaderXPos;
    float mHeaderYPos;

    const CInvSettings & mSettings;
    const CInvText & mTextCreator;
    CInvHiscoreList & mHiscoreKeeper;

    LPDIRECT3D9             mPD3D;
    LPDIRECT3DDEVICE9       mPd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 mPVB;

  };

} // namespace Inv

#endif
