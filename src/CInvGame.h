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

    // A structure for our custom vertex type
    struct CUSTOMVERTEX
    {
      FLOAT x, y, z, rhw; // The transformed position for the vertex
      DWORD color;        // The vertex color
      float u, v;
    };

    // Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

    HRESULT InitD3D();

    HRESULT InitVB();

    bool IsKeyDown( int key );

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
