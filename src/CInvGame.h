//****************************************************************************************************
//! \file CInvGame.h                                                                          
//! Module contains class CInvGame, which implements singleton pattern for global logging                                                                      
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvGame
#define H_CInvGame

#include <d3d9.h>
#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>

#include <engine/CInvHiscoreList.h>
#include <engine/CInvInsertCoinScreen.h>
#include <engine/CInvPlayItScreen.h>

namespace Inv
{

  class CInvGame
  {
  public:

    CInvGame( const CInvSettings & settings );

    CInvGame( const CInvGame & ) = delete;
    CInvGame & operator=( const CInvGame & ) = delete;
    ~CInvGame();

    bool Initialize();
    //!< Initializes game, returns true if successful

    bool Run();
    //!< Runs the game, returns true if successful

    bool Cleanup();
    //!< Releases resources of the game, returns true if successful


  private:


    HRESULT InitD3D();

    HRESULT InitVB();

    bool IsKeyDown( int key );

    void ProcessInput( ControlStateFlags_t & controlState, ControlValue_t & controlValue );

    std::unique_ptr<CInvText> mTextCreator;

    std::unique_ptr<CInvHiscoreList> mHiscoreKeeper;

    std::unique_ptr <CInvPrimitive> mPrimitives;

    std::unique_ptr<CInvInsertCoinScreen> mInsertCoinScreen;
    std::unique_ptr<CInvPlayItScreen> mPlayItScreen;

    const CInvSettings & mSettings;

    WNDCLASSEX mWindowClass;

    HWND mHWnd;

    LARGE_INTEGER mStartTime;
    LARGE_INTEGER mFreq;

    LPDIRECT3D9             mPD3D; 
    LPDIRECT3DDEVICE9       mPd3dDevice; 
    LPDIRECT3DVERTEXBUFFER9 mPVB; 

    DWORD mClearColor;

    static const std::wstring mWindiwClassId;

    static const std::wstring mWindiwName;

  };

} // namespace Inv

#endif
