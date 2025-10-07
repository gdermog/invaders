//****************************************************************************************************
//! \file CInvPlayItScreen.h                                                                          
//! Module contains class CInvPlayItScreen, which implements singleton pattern for global logging                                                                      
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvPlayItScreen
#define H_CInvPlayItScreen

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>

namespace Inv
{

  class CInvPlayItScreen
  {
  public:

    CInvPlayItScreen(
      const CInvSettings & settings,
      const CInvText & textCreator,
      CInvPrimitive & primitives,
      LPDIRECT3D9 pD3D,
      LPDIRECT3DDEVICE9 pd3dDevice,
      LPDIRECT3DVERTEXBUFFER9 pVB,
      LARGE_INTEGER timeReferencePoint );

    CInvPlayItScreen( const CInvPlayItScreen & ) = delete;
    CInvPlayItScreen & operator=( const CInvPlayItScreen & ) = delete;
    ~CInvPlayItScreen();

    bool MainLoop(
      uint32_t & newScoreToEnter,
      bool & gameEnd,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue,
      LARGE_INTEGER actualTimePoint );

    void Reset( LARGE_INTEGER newTimeRefPoint );

  private:


    LARGE_INTEGER mTimeReferencePoint;

    const CInvSettings & mSettings;
    const CInvText & mTextCreator;
    CInvPrimitive & mPrimitives;

    LPDIRECT3D9             mPD3D;
    LPDIRECT3DDEVICE9       mPd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 mPVB;

    uint32_t mActualScore;

  };

} // namespace Inv

#endif
