//****************************************************************************************************
//! \file CInvBackground.cpp
//! Module defines class CInvBackground that represents a 2D Background which can be drawn on screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <filesystem>

#include <d3dx9.h>

#include <graphics/CInvBackground.h>

#include <CInvLogger.h>
#include <InvStringTools.h>


static const std::string lModLogId( "Background" );

namespace Inv
{
  CInvBackground::CInvBackground( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice ):

    mSettings( settings ),
    mPd3dDevice( pd3dDevice ),
    mTexture( nullptr ),
    mTextureSize{ 0u, 0u },
    mLvl( 0.1f ),
    mRollCoef( 0.33f ),
    mVprWidth( (float)settings.GetWidth() ),
    mVprHeight( (float)settings.GetHeight() ),
    mTxtrWidth( 1.0f ),
    mTxtrHeight( 1.0f )
  {}

  //----------------------------------------------------------------------------------------------

  CInvBackground::~CInvBackground()
  {}

  //----------------------------------------------------------------------------------------------

  void CInvBackground::AddBackgroundImage( const std::string & imageName )
  {
    if( nullptr == mPd3dDevice )
    {
      LOG << "Direct3D device is null, cannot load image '" << imageName << "'.";
      return;
    } // if

    std::filesystem::path imagePath( mSettings.GetImagePath() + "/" + imageName );

    if( !std::filesystem::exists( imagePath ) )
    {
      LOG << "Image file '" << imagePath << "' does not exist.";
      return;
    } // if

    IDirect3DTexture9 * tex = NULL;
    D3DXCreateTextureFromFile( mPd3dDevice, imagePath.wstring().c_str(), &tex);

    if( nullptr == tex )
    {
      LOG << "Cannot load image file '" << imagePath << "'.";
      return;
    } // if

    std::pair<size_t, size_t> texSize( 0, 0 );
    D3DXIMAGE_INFO info{};
    if( SUCCEEDED( D3DXGetImageInfoFromFile( imagePath.wstring().c_str(), &info ) ) )
    {
      texSize.first = info.Width;    // original width of the image on disk
      texSize.second = info.Height;  // original height of the image on disk
    } // if

    mTexture = tex;
    mTextureSize = texSize;

    mTxtrWidth = (float)mSettings.GetWidth() / (float)mTextureSize.first;
    mTxtrHeight = (float)mSettings.GetHeight() / (float)mTextureSize.second;

  } // CInvBackground::AddBackgroundImage

  //----------------------------------------------------------------------------------------------

  void CInvBackground::Draw(
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick,
    DWORD color ) const
  {
    if( nullptr == mPd3dDevice || nullptr == mTexture )
      return;

    auto dTick = (size_t)( mRollCoef * (float)( actualTick.QuadPart - referenceTick.QuadPart) );
    float t = 2.0 * ( 1.0 - (float)( dTick % mTextureSize.second ) / ((float)mTextureSize.second) );

    mTea2[0] = { 0.0f,      0.0f,       mLvl, 1.0f, color, 0.0f,       t };
    mTea2[1] = { mVprWidth, 0.0f,       mLvl, 1.0f, color, mTxtrWidth, t };
    mTea2[2] = { 0.0f,      mVprHeight, mLvl, 1.0f, color, 0.0f,       t + mTxtrHeight };
    mTea2[3] = { mVprWidth, mVprHeight, mLvl, 1.0f, color, mTxtrWidth, t + mTxtrHeight };

    IDirect3DStateBlock9 * stateBlock = nullptr;
    mPd3dDevice->CreateStateBlock( D3DSBT_ALL, &stateBlock );

    mPd3dDevice->SetTexture( 0, mTexture );
    mPd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    mPd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
    mPd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    mPd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    mPd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, mTea2, sizeof( CUSTOMVERTEX ) );

    if( nullptr != stateBlock )
    {
      stateBlock->Apply();
      stateBlock->Release();
    } // if

  } // CInvBackground::Draw

} // namespace Inv
