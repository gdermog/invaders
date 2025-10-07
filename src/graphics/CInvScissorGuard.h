//****************************************************************************************************
//! \file CInvScissorGuard.h                                                                          
//! Module contains class CInvScissorGuard, which implements ...                                                                     
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvScissorGuard
#define H_CInvScissorGuard

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>

namespace Inv
{

  class CInvScissorGuard
  {
    public:

    CInvScissorGuard( LPDIRECT3DDEVICE9 pd3dDevice, const RECT & newRect );
    CInvScissorGuard( const CInvScissorGuard & ) = delete;
    CInvScissorGuard & operator=( const CInvScissorGuard & ) = delete;
    ~CInvScissorGuard();

    void Restore(); 

  private:

    LPDIRECT3DDEVICE9 mPd3dDevice;
    DWORD mPrevEnabled;
    RECT  mPrevRect;
    bool  mHavePrevRect;
    bool  mHavePrevEnabled;
    bool  mRestored;

  };

} // namespace Inv

#endif
