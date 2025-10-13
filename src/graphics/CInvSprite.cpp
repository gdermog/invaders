//****************************************************************************************************
//! \file CInvSprite.cpp
//! Module defines class CInvSprite that represents a 2D sprite which can be drawn on screen.
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
    mTea2{},
    mHalfSizeX( 0.0f ),
    mHalfSizeY( 0.0f ),
    mEffects(),
    mImageIndex( 0 ),
    mSettings( settings ),
    mPd3dDevice( pd3dDevice ),
    mTextures(),
    mTextureSizes(),
    mLvl( 0.0f )
#ifdef _DEBUG
    , mDebugId( 0 )
#endif
  {
  }

  //----------------------------------------------------------------------------------------------

  CInvSprite::CInvSprite( const CInvSprite & other ):
    mTea2{},
    mHalfSizeX( other.mHalfSizeX ),
    mHalfSizeY( other.mHalfSizeY ),
    mEffects( other.mEffects ),
    mImageIndex( other.mImageIndex ),
    mSettings( other.mSettings ),
    mPd3dDevice( other.mPd3dDevice ),
    mTextures( other.mTextures ),
    mTextureSizes( other.mTextureSizes )
  {
    memcpy( mTea2, other.mTea2, sizeof( mTea2 ) );
  } // CInvSprite::CInvSprite

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
    D3DXIMAGE_INFO info{};
    if( SUCCEEDED( D3DXGetImageInfoFromFile( imagePath.wstring().c_str(), &info ) ) )
    {
      texSize.first = info.Width;    // original width of the image on disk
      texSize.second = info.Height;  // original height of the image on disk
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
    float xCentre,
    float yCentre,
    float xSize,
    float ySize,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick,
    uint32_t specificImageIndex,
    DWORD color )
  {
    if( nullptr == mPd3dDevice || mTextures.empty() )
      return;

    mImageIndex = specificImageIndex;
    if( (uint32_t)mTextures.size() <= mImageIndex )
      mImageIndex = 0;

    mHalfSizeX = xSize * 0.5f;
    mHalfSizeY = ySize * 0.5f;

    mTea2[0] = { xCentre - mHalfSizeX, yCentre - mHalfSizeY, mLvl, 1.0f, color, 0.0f, 0.0f, };
    mTea2[1] = { xCentre + mHalfSizeX, yCentre - mHalfSizeY, mLvl, 1.0f, color, 1.0f, 0.0f, };
    mTea2[2] = { xCentre - mHalfSizeX, yCentre + mHalfSizeY, mLvl, 1.0f, color, 0.0f, 1.0f, };
    mTea2[3] = { xCentre + mHalfSizeX, yCentre + mHalfSizeY, mLvl, 1.0f, color, 1.0f, 1.0f, };

    if( !mEffects.empty() )
    {
      for( auto & effectCategory : mEffects )
      {
        for( auto & ef : effectCategory.second )
          ef->ApplyEffect( this, referenceTick, actualTick, diffTick );
      } // for

      if( SIZE_MAX == mImageIndex )
        return;         // Sprite drawing was cancelled by effect

      if( mTextures.size() <= mImageIndex )
        mImageIndex = 0;
    } // if

    auto tex = mTextures[mImageIndex];
    if( nullptr == tex )
      return;

    for( auto & teaItem: mTea2 )
    {
      teaItem.x -= 0.5f;
      teaItem.y -= 0.5f;
    } // for

    IDirect3DTexture9 * t = (IDirect3DTexture9 *)tex;
    mPd3dDevice->SetTexture( 0, t );
    mPd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, mTea2, sizeof( CUSTOMVERTEX ) );

  } // CInvSprite::Draw

  //----------------------------------------------------------------------------------------------

  void CInvSprite::AddEffect( std::shared_ptr<CInvEffect> effect )
  {
    if( nullptr == effect )
      return;

    auto & efList = mEffects[effect->GetEffectPriority()];
    if( std::find( efList.begin(), efList.end(), effect ) == efList.end() )
      efList.push_back( effect );

  } // CInvSprite::AddEffect

  //----------------------------------------------------------------------------------------------

  void CInvSprite::RemoveEffect( std::shared_ptr<CInvEffect> effect )
  {
    if( nullptr == effect )
      return;
    auto & efList = mEffects[effect->GetEffectPriority()];

    auto it = std::find( efList.begin(), efList.end(), effect );
    if( it != efList.end() )
      efList.erase( it );
  } // CInvSprite::RemoveEffect

  //----------------------------------------------------------------------------------------------

  std::pair<size_t, size_t> CInvSprite::GetImageSize( size_t imageIndex ) const
  {
    if( imageIndex >= mTextureSizes.size() )
      return { 0, 0 };

    return mTextureSizes[imageIndex];
  } // GetImageSize

  //----------------------------------------------------------------------------------------------

  void CInvSprite::GetResultingPosition(
    float & xTopLeft, float & yTopLeft,
    float & xBottomRight, float & yBottomRight,
    float & xSize, float & ySize,
    size_t & imageIndex ) const
  {
    xTopLeft = mTea2[0].x;
    yTopLeft = mTea2[0].y;
    xBottomRight = mTea2[3].x;
    yBottomRight = mTea2[3].y;
    xSize = 2.0f * mHalfSizeX;
    ySize = 2.0f * mHalfSizeY;
    imageIndex = mImageIndex;
  } // CInvSprite::GetResultingPosition

  //----------------------------------------------------------------------------------------------

  void CInvSprite::GetResultingBoundingBox( float & xMin, float & xMax, float & yMin, float & yMax ) const
  {
    float retVal1 = min( mTea2[0].x, mTea2[1].x );
    float retVal2 = min( mTea2[2].x, mTea2[3].x );
    xMin = min( retVal1, retVal2 );

    retVal1 = max( mTea2[0].x, mTea2[1].x );
    retVal2 = max( mTea2[2].x, mTea2[3].x );
    xMax = max( retVal1, retVal2 );

    retVal1 = min( mTea2[0].y, mTea2[1].y );
    retVal2 = min( mTea2[2].y, mTea2[3].y );
    yMin = min( retVal1, retVal2 );

    retVal1 = max( mTea2[0].y, mTea2[1].y );
    retVal2 = max( mTea2[2].y, mTea2[3].y );
    yMax = max( retVal1, retVal2 );

  } // CInvSprite::GetResultingBoundingBox

} // namespace Inv
