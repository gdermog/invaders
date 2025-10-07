//****************************************************************************************************
//! \file CInvSprite.cpp                                                                         
//! Module contains class CInvSprite, which implements singleton pattern for global logging                                                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <filesystem>

#include <d3dx9.h>

#include <graphics/CInvSprite.h>

#include <CInvLogger.h>
#include <InvStringTools.h>


static const std::string lModLogId( "SPRITE" );

namespace Inv
{
  CInvSprite::CInvSprite( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice ):
    mSettings( settings ),
    mPd3dDevice( pd3dDevice ),
    mTextures()
  {} 

  //----------------------------------------------------------------------------------------------

  CInvSprite::~CInvSprite()
  {}

  //----------------------------------------------------------------------------------------------

  void CInvSprite::AddSpriteImage( const std::string & imageName )
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
    D3DSURFACE_DESC desc;
    HRESULT hr = tex->GetLevelDesc( 0, &desc );
    if( SUCCEEDED( hr ) )
    {
      texSize.first = desc.Width;
      texSize.second = desc.Height;
    } // if

    mTextures.push_back( tex );
    mTextureSizes.push_back( texSize );

  } // CInvSprite::AddSpriteImage

  //----------------------------------------------------------------------------------------------

  void CInvSprite::AddMultipleSpriteImages( const std::string & imageNameTemplate )
  {
    if( nullptr == mPd3dDevice )
    {
      LOG << "Direct3D device is null, cannot load images.";
      return;
    } // if

    std::filesystem::path imagePath;
    std::string imageTemplateFilled;

    uint32_t index = 0;
    while( index < 1000 )
    {
      imageTemplateFilled = FormatStr( imageNameTemplate, index + 1 );
      imagePath = mSettings.GetImagePath() + "/" + imageTemplateFilled;

      if( !std::filesystem::exists( imagePath ) )
        break;

      AddSpriteImage( imageTemplateFilled );

      ++index;

    } // while

    LOG << index << " images was loaded according to template '" << imageNameTemplate << "'.";

  } // CInvSprite::AddMultipleSpriteImages

  //----------------------------------------------------------------------------------------------

  void CInvSprite::Draw( 
    size_t imageIndex, 
    float xCentre, 
    float yCentre, 
    float xSize, 
    float ySize, 
    float rotateRad, 
    DWORD color )
  {
    if( nullptr == mPd3dDevice || mTextures.size() <= imageIndex )
      return;

    auto tex = mTextures[imageIndex];
    if( nullptr == tex )
      return;

    IDirect3DTexture9 * t = (IDirect3DTexture9 *)tex;
    mPd3dDevice->SetTexture( 0, t );

    xSize *= 0.5f;
    ySize *= 0.5f;

    if( !IsZero( rotateRad ) )
    {
      float c = cosf( rotateRad );
      float s = sinf( rotateRad );

#define ROTATE(xx,yy) ( xCentre + (xx)*c + (yy)*s ), ( yCentre + (yy)*c - (xx)*s ) 
      CUSTOMVERTEX tea2[] =
      {
        { ROTATE( -xSize, -ySize ), 0.5f, 1.0f, color, 0.0f, 0.0f, }, // x, y, z, rhw, color
        { ROTATE(  xSize, -ySize ), 0.5f, 1.0f, color, 1.0f, 0.0f, },
        { ROTATE( -xSize,  ySize ), 0.5f, 1.0f, color, 0.0f, 1.0f, },
        { ROTATE(  xSize,  ySize ), 0.5f, 1.0f, color, 1.0f, 1.0f, },
      };

      mPd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, tea2, sizeof( CUSTOMVERTEX ) );
    } // if
    else
    {
      CUSTOMVERTEX tea2[] =
      {
        { xCentre - xSize, yCentre - ySize, 0.5f, 1.0f, color, 0.0f, 0.0f, }, // x, y, z, rhw, color
        { xCentre + xSize, yCentre - ySize, 0.5f, 1.0f, color, 1.0f, 0.0f, },
        { xCentre - xSize, yCentre + ySize, 0.5f, 1.0f, color, 0.0f, 1.0f, },
        { xCentre + xSize, yCentre + ySize, 0.5f, 1.0f, color, 1.0f, 1.0f, },
      };

      mPd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, tea2, sizeof( CUSTOMVERTEX ) );
    } // else


  } // CInvSprite::Draw

  //----------------------------------------------------------------------------------------------

  std::pair<size_t, size_t> CInvSprite::GetImageSize( size_t imageIndex ) const
  {
    if( imageIndex >= mTextureSizes.size() )
      return { 0, 0 };

    return mTextureSizes[imageIndex];
  } // GetImageSize

} // namespace Inv
