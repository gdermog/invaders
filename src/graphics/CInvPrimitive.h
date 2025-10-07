//****************************************************************************************************
//! \file CInvPrimitive.h                                                                          
//! Module contains class CInvPrimitive, which ..                                                                     
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvPrimitive
#define H_CInvPrimitive

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>


namespace Inv
{

  class CInvPrimitive
  {
    public:

    CInvPrimitive( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );
    CInvPrimitive( const CInvPrimitive & ) = delete;
    CInvPrimitive & operator=( const CInvPrimitive & ) = delete;
    ~CInvPrimitive();

    void DrawLine(
      float x1, float y1,
      float x2, float y2,
      D3DCOLOR color,
      bool pixelPerfect = false );

    void DrawSquare(
      float x1, float y1,
      float x2, float y2,
      D3DCOLOR color,
      bool pixelPerfect = false );

  private:

    const CInvSettings & mSettings;

    LPDIRECT3DDEVICE9 mPd3dDevice;

  };

} // namespace Inv

#endif
