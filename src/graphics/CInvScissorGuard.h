//****************************************************************************************************
//! \file CInvScissorGuard.h
//! Module contains class CInvScissorGuard, which implements DX9 device scissor rectangle application
//! and restoration in form of scope guard.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvScissorGuard
#define H_CInvScissorGuard

#include <d3d9.h>

namespace Inv
{

  /*! \brief The class saves current DX9 device scissor rectangle and its enabled/disabled
      state upon construction, and restores them upon destruction. The class is intended to
      be used as a stack variable, so that the previous state is automatically restored when
      the variable goes out of scope. */
  class CInvScissorGuard
  {
    public:

    CInvScissorGuard( LPDIRECT3DDEVICE9 pd3dDevice, const RECT & newRect );
    CInvScissorGuard( const CInvScissorGuard & ) = delete;
    CInvScissorGuard & operator=( const CInvScissorGuard & ) = delete;
    ~CInvScissorGuard();

    void Restore();
    /*!< \brief Restores previous scissor rectangle and enabled/disabled state. It is
         automatically called in destructor, but can be called manually earlier. */

  private:

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //<! Direct3D device, used to set and get scissor rectangle

    DWORD mPrevEnabled;
    //<! Previous state of scissor test, value set by GetRenderState() method

    RECT  mPrevRect;
    //<! Previous scissor rectangle

    bool  mHavePrevRect;
    //<! True if previous rectangle was successfully retrieved

    bool  mHavePrevEnabled;
    //<! True if previous enabled state was successfully retrieved

    bool  mRestored;
    //<! True if previous state was already restored

  };

} // namespace Inv

#endif
