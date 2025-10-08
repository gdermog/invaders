//****************************************************************************************************
//! \file CInvPrimitive.cpp
//! Module defines class CInvPrimitive, which provides methods to draw basic primitives such as
//! lines and rectangles.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <graphics/CInvPrimitive.h>

#include <CInvLogger.h>

static const std::string lModLogId( "Primitive" );

namespace Inv
{
  CInvPrimitive::CInvPrimitive( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice ):
    mSettings( settings ),
    mPd3dDevice( pd3dDevice )
  {}

  //----------------------------------------------------------------------------------------------

  CInvPrimitive::~CInvPrimitive()
  {}

  //----------------------------------------------------------------------------------------------

  struct LineVertex {
    float x, y, z, rhw;
    DWORD color;
    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
  };

  //----------------------------------------------------------------------------------------------

  void CInvPrimitive::DrawLine(
    float x1, float y1,
    float x2, float y2,
    D3DCOLOR color,
    bool pixelPerfect )
  {
    if( nullptr == mPd3dDevice )
      return;

    // Half-pixel correction
    if( pixelPerfect )
    {
      const float half = -0.5f;
      x1 += half; y1 += half;
      x2 += half; y2 += half;
    }

    LineVertex verts[] =
    {
      { x1, y1, 0.5f, 1.0f, color },
      { x2, y2, 0.5f, 1.0f, color },
    };

    // We save and restore VS/PS and FVF (safe path)
    IDirect3DVertexShader9 * prevVS = nullptr;
    IDirect3DPixelShader9 * prevPS = nullptr;
    DWORD prevFVF = 0;
    DWORD prevZEnable = 0;
    DWORD prevLighting = 0;
    DWORD prevScissor = 0;
    DWORD prevAlpha = 0;

    // Get current state
    mPd3dDevice->GetVertexShader( &prevVS );
    mPd3dDevice->GetPixelShader( &prevPS );
    mPd3dDevice->GetFVF( &prevFVF );
    mPd3dDevice->GetRenderState( D3DRS_ZENABLE, &prevZEnable );
    mPd3dDevice->GetRenderState( D3DRS_LIGHTING, &prevLighting );
    mPd3dDevice->GetRenderState( D3DRS_SCISSORTESTENABLE, &prevScissor );
    mPd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &prevAlpha );

    // Switch to fixed-function, turn off depth and lighting and scissor (for overlay)
    mPd3dDevice->SetVertexShader( NULL );
    mPd3dDevice->SetPixelShader( NULL );
    mPd3dDevice->SetFVF( LineVertex::FVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Important: DrawPrimitiveUP expects a number of primitives (here 1 LINELIST)
    mPd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, verts, sizeof( LineVertex ) );

    // Restoring states
    mPd3dDevice->SetFVF( prevFVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, prevZEnable );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, prevLighting );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, prevScissor );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, prevAlpha );

    // Restore previous shaders (must have been set before Release)
    if( prevVS ) { mPd3dDevice->SetVertexShader( prevVS ); prevVS->Release(); }
    else mPd3dDevice->SetVertexShader( NULL );

    if( prevPS ) { mPd3dDevice->SetPixelShader( prevPS ); prevPS->Release(); }
    else mPd3dDevice->SetPixelShader( NULL );

  } // CInvPrimitive::DrawLine

  //----------------------------------------------------------------------------------------------

  void CInvPrimitive::DrawSquare(
    float x1, float y1,
    float x2, float y2,
    D3DCOLOR color,
    bool pixelPerfect )
  {
    if( nullptr == mPd3dDevice )
      return;

    // Half-pixel correction
    if( pixelPerfect )
    {
      const float half = -0.5f;
      x1 += half; y1 += half;
      x2 += half; y2 += half;
    }

    LineVertex verts[] =
    {
      { x1, y1, 0.5f, 1.0f, color}, // top left corner
      { x2, y1, 0.5f, 1.0f, color}, // top right corner
      { x2, y2, 0.5f, 1.0f, color}, // bottom right corner
      { x1, y2, 0.5f, 1.0f, color}, // bottom left corner
      { x1, y1, 0.5f, 1.0f, color}, // top left corner (close)
    };

    // We save and restore VS/PS and FVF (safe path))
    IDirect3DVertexShader9 * prevVS = nullptr;
    IDirect3DPixelShader9 * prevPS = nullptr;
    DWORD prevFVF = 0;
    DWORD prevZEnable = 0;
    DWORD prevLighting = 0;
    DWORD prevScissor = 0;
    DWORD prevAlpha = 0;

    // Get current state (ignorujeme chyby, ale logneme je)
    mPd3dDevice->GetVertexShader( &prevVS );
    mPd3dDevice->GetPixelShader( &prevPS );
    mPd3dDevice->GetFVF( &prevFVF );
    mPd3dDevice->GetRenderState( D3DRS_ZENABLE, &prevZEnable );
    mPd3dDevice->GetRenderState( D3DRS_LIGHTING, &prevLighting );
    mPd3dDevice->GetRenderState( D3DRS_SCISSORTESTENABLE, &prevScissor );
    mPd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &prevAlpha );

    // Switch to fixed-function, turn off depth and lighting and scissor (for overlay)
    mPd3dDevice->SetVertexShader( NULL );
    mPd3dDevice->SetPixelShader( NULL );
    mPd3dDevice->SetFVF( LineVertex::FVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Important: DrawPrimitiveUP expects a number of primitives (here 4 segments)
    mPd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, verts, sizeof( LineVertex ) );

    // Restoring states
    mPd3dDevice->SetFVF( prevFVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, prevZEnable );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, prevLighting );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, prevScissor );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, prevAlpha );

    // Restore previous shaders (must have been set before Release)
    if( prevVS ) { mPd3dDevice->SetVertexShader( prevVS ); prevVS->Release(); }
    else mPd3dDevice->SetVertexShader( NULL );

    if( prevPS ) { mPd3dDevice->SetPixelShader( prevPS ); prevPS->Release(); }
    else mPd3dDevice->SetPixelShader( NULL );

  } // CInvPrimitive::DrawSquare



} // namespace Inv
