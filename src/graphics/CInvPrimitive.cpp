//****************************************************************************************************
//! \file CInvPrimitive.cpp
//! Module contains class CInvPrimitive, which implements singleton pattern for global logging
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

    // Half-pixel correction (experimentuj +/-0.5)
    if( pixelPerfect )
    {
      const float half = -0.5f;
      x1 += half; y1 += half;
      x2 += half; y2 += half;
    }

    LineVertex verts[] =
    {
      { x1, y1, 0.5f, 1.0f, color},
      { x2, y2, 0.5f, 1.0f, color},
    };

    // Uložíme a obnovíme VS/PS a FVF (safe path)
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

    // Přepneme na fixed-function, vypneme depth a lighting a scissor (pro overlay)
    mPd3dDevice->SetVertexShader( NULL );
    mPd3dDevice->SetPixelShader( NULL );
    mPd3dDevice->SetFVF( LineVertex::FVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Důležité: DrawPrimitiveUP očekává počet primitiv (tady 1 LINELIST)
    mPd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, verts, sizeof( LineVertex ) );

    // Obnovení stavů
    mPd3dDevice->SetFVF( prevFVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, prevZEnable );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, prevLighting );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, prevScissor );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, prevAlpha );

    // Obnovíme předchozí shadery (musejí být nastaveny před Release)
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

    // Half-pixel correction (experimentuj +/-0.5)
    if( pixelPerfect )
    {
      const float half = -0.5f;
      x1 += half; y1 += half;
      x2 += half; y2 += half;
    }

    LineVertex verts[] =
    {
      { x1, y1, 0.5f, 1.0f, color}, // levý horní roh
      { x2, y1, 0.5f, 1.0f, color}, // pravý horní roh
      { x2, y2, 0.5f, 1.0f, color}, // pravý dolní roh
      { x1, y2, 0.5f, 1.0f, color}, // levý dolní roh
      { x1, y1, 0.5f, 1.0f, color}, // levý horní roh (uzavření)
    };

    // Uložíme a obnovíme VS/PS a FVF (safe path)
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

    // Přepneme na fixed-function, vypneme depth a lighting a scissor (pro overlay)
    mPd3dDevice->SetVertexShader( NULL );
    mPd3dDevice->SetPixelShader( NULL );
    mPd3dDevice->SetFVF( LineVertex::FVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Důležité: DrawPrimitiveUP očekává počet primitiv (tady 4 segmenty)
    mPd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, verts, sizeof( LineVertex ) );

    // Obnovení stavů
    mPd3dDevice->SetFVF( prevFVF );
    mPd3dDevice->SetRenderState( D3DRS_ZENABLE, prevZEnable );
    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, prevLighting );
    mPd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, prevScissor );
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, prevAlpha );

    // Obnovíme předchozí shadery (musejí být nastaveny před Release)
    if( prevVS ) { mPd3dDevice->SetVertexShader( prevVS ); prevVS->Release(); }
    else mPd3dDevice->SetVertexShader( NULL );

    if( prevPS ) { mPd3dDevice->SetPixelShader( prevPS ); prevPS->Release(); }
    else mPd3dDevice->SetPixelShader( NULL );

  } // CInvPrimitive::DrawLine



} // namespace Inv
