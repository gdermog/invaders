//****************************************************************************************************
//! \file CInvScissorGuard.cpp
//! Module contains class CInvScissorGuard, which implements ...
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <graphics/CInvScissorGuard.h>

namespace Inv
{

  //-------------------------------------------------------------------------------------------------

  CInvScissorGuard::CInvScissorGuard( LPDIRECT3DDEVICE9 pd3dDevice, const RECT & newRect ):
    mPd3dDevice( pd3dDevice ),
    mPrevEnabled( 0 ),
    mPrevRect(),
    mHavePrevRect( false ),
    mHavePrevEnabled( false ),
    mRestored( false )
  {
    if( nullptr == mPd3dDevice )
      return;

    DWORD prev = 0;
    if( SUCCEEDED( mPd3dDevice->GetRenderState( D3DRS_SCISSORTESTENABLE, &prev ) ) )
    {
      mPrevEnabled = prev;
      mHavePrevEnabled = true;
    } // if
    else
    {
      mPrevEnabled = 0;
      mHavePrevEnabled = false;
    } // else

    mHavePrevRect = SUCCEEDED( mPd3dDevice->GetScissorRect( &mPrevRect ) );

    mPd3dDevice->SetScissorRect( &newRect );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );

  } // CInvScissorGuard::CInvScissorGuard

  //-------------------------------------------------------------------------------------------------

  CInvScissorGuard::~CInvScissorGuard()
  {
    Restore();
  } // CInvScissorGuard::~CInvScissorGuard

  //-------------------------------------------------------------------------------------------------

  void CInvScissorGuard::Restore()
  {
    if( mRestored || nullptr == mPd3dDevice ) return;

    if( mHavePrevEnabled )
      mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, mPrevEnabled ? TRUE : FALSE );
    else
      mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

    if( mHavePrevRect )
      mPd3dDevice->SetScissorRect( &mPrevRect );

    mRestored = true;
  } // CInvScissorGuard::Restore

} // namespace Inv
