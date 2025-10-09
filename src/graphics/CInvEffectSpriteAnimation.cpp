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
    mIsContinuous( true ),
    mFinalEventCallbackReported( false ),
    mFinalEventCallback( nullptr ),
    mEventCallbacks()
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

    if( ! mEventCallbacks.empty() )
    {
      auto cbIt = mEventCallbacks.find( sprite->mImageIndex );
      if( cbIt != mEventCallbacks.end() && false == cbIt->second.first )
      {                 // If the animation reached an important image with registered callback,
                        // calls it
        cbIt->second.second( (uint32_t)sprite->mImageIndex );
        cbIt->second.first = true;

      }
    } // if

    if( lastImage <= (uint32_t)sprite->mImageIndex )
    {
      for( auto & cbIt : mEventCallbacks )
        cbIt.second.first = false;
                        // On final image, reset all event callbacks to be callable again

      if( !mIsContinuous )
      {                 // Non-continuous animation reached its end - class suspends
                        // itself and calls final callback
        if( nullptr != mFinalEventCallback && false == mFinalEventCallbackReported )
        {
          mFinalEventCallback( (uint32_t)sprite->mImageIndex );
          mFinalEventCallbackReported = true;
        } // if
        Suspend();
      } // if
    } // if

    return true;

  } // CInvEffectSpriteAnimation::ApplyEffect

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteAnimation::Suspend()
  {

    CInvEffect::Suspend();
  } // CInvEffectSpriteAnimation::Suspend

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteAnimation::Restore()
  {
    mFinalEventCallbackReported = false;
    for( auto & cbIt : mEventCallbacks )
      cbIt.second.first = false;

    CInvEffect::Restore();
  } // CInvEffectSpriteAnimation::Restore

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

  void CInvEffectSpriteAnimation::AddEventCallback( FnEventCallback_t callback )
  {
    if( nullptr == callback )
    {
      LOG << "CInvEffectSpriteAnimation::AddEventCallback: Warning: null final callback, ignoring.";
      return;
    } // if

    mFinalEventCallbackReported = false;
    mFinalEventCallback = callback;
  } // CInvEffectSpriteAnimation::AddEventCallback

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteAnimation::AddEventCallback( uint32_t imageIndex, FnEventCallback_t callback )
  {
    if( nullptr == callback )
    {
      LOG << "CInvEffectSpriteAnimation::AddEventCallback: Warning: null callback (idx "
          << imageIndex <<"), ignoring.";
      return;
    } // if

    mEventCallbacks[imageIndex] = std::make_pair( false, callback );
  } // CInvEffectSpriteAnimation::AddEventCallback

  //----------------------------------------------------------------------------------------------

} // namespace Inv
