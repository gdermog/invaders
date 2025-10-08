//****************************************************************************************************
//! \file CInvEffectSpriteAnimation.cpp
//! Module contains class CInvEffectSpriteAnimation, which defines effect that animates sprite by
//! changing its image in given pace
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteAnimation.h>

#include <graphics/CInvSprite.h>

#include <CInvLogger.h>

static const std::string lModLogId( "EffectSprite" );

namespace Inv
{

  CInvEffectSpriteAnimation::CInvEffectSpriteAnimation(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffect( settings, pd3dDevice, ePriority ),
    mPace( 1 ),
    mFirstImage( 0 ),
    mLastImage( UINT32_MAX ),
    mIsContinuous( true )
  {}

  //----------------------------------------------------------------------------------------------

  CInvEffectSpriteAnimation::~CInvEffectSpriteAnimation()
  {}

  //----------------------------------------------------------------------------------------------

  bool CInvEffectSpriteAnimation::ApplyEffect(
    void * obj,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick )
  {
    if( nullptr == obj )
      return false;

    if( IsSuspended() )
      return true;

    auto * sprite = static_cast<Inv::CInvSprite *>( obj );

    auto nrOfImages = (uint32_t)sprite->GetNumberOfImages();

    auto lastImage = mLastImage;
    if( nrOfImages <= lastImage )
      lastImage = nrOfImages - 1;

    auto rng = lastImage - mFirstImage + 1;

    LONGLONG idx = actualTick.QuadPart - referenceTick.QuadPart + diffTick.QuadPart;
    idx %= ( mPace * rng );
    sprite->mImageIndex = mFirstImage + idx / mPace;

static size_t mxxx = 0;
if( mxxx < sprite->mImageIndex )
  mxxx = sprite->mImageIndex;

    if( !mIsContinuous && lastImage <= sprite->mImageIndex )
      Suspend();

    return true;

  } // CInvEffectSpriteAnimation::ApplyEffect

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteAnimation::SetImageRange( uint32_t firstImage, uint32_t lastImage )
  {
    if( firstImage > lastImage )
    {
      LOG << "CInvEffectSpriteAnimation::SetImageRange: Warning: firstImage > lastImage, ignoring.";
      return;
    } // if
    mFirstImage = firstImage;
    mLastImage = lastImage;
  } // CInvEffectSpriteAnimation::SetImageRange

  //----------------------------------------------------------------------------------------------

} // namespace Inv
