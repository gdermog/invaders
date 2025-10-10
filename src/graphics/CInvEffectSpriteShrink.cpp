//****************************************************************************************************
//! \file CInvEffectSpriteShrink.cpp
//! Module contains class CInvEffectSpriteShrink, which defines effect that animates sprite by
//! shrinking its image into given relative size in given pace
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteShrink.h>

#include <graphics/CInvSprite.h>

#include <CInvLogger.h>

static const std::string lModLogId( "EffectSpriteShrink" );

namespace Inv
{

  CInvEffectSpriteShrink::CInvEffectSpriteShrink(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffect( settings, pd3dDevice, ePriority ),
    mPace( 1 ),
    mFinalRatio{ 0.0f }
  {}

  //----------------------------------------------------------------------------------------------

  CInvEffectSpriteShrink::~CInvEffectSpriteShrink()
  {}

  //----------------------------------------------------------------------------------------------

  bool CInvEffectSpriteShrink::ApplyEffect(
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

    LONGLONG idx = actualTick.QuadPart - referenceTick.QuadPart + diffTick.QuadPart;
    if( idx < 0 )
      idx = 0;
    idx %= mPace;
    idx++;

    float actRatio = 1.0f - ( (float)idx / (float)mPace ) * ( 1.0f - mFinalRatio );

    float xCentre = sprite->mTea2[0].x + sprite->mHalfSizeX;
    float yCentre = sprite->mTea2[0].y + sprite->mHalfSizeY;

    sprite->mHalfSizeX *= actRatio;
    sprite->mHalfSizeY *= actRatio;

    sprite->mTea2[0].x = xCentre - sprite->mHalfSizeX;
    sprite->mTea2[1].x = xCentre + sprite->mHalfSizeX;
    sprite->mTea2[2].x = xCentre - sprite->mHalfSizeX;
    sprite->mTea2[3].x = xCentre + sprite->mHalfSizeX;

    sprite->mTea2[0].y =  yCentre - sprite->mHalfSizeY;
    sprite->mTea2[1].y =  yCentre - sprite->mHalfSizeY;
    sprite->mTea2[2].y =  yCentre + sprite->mHalfSizeY;
    sprite->mTea2[3].y =  yCentre + sprite->mHalfSizeY;

    if( ! IsContinuous() && (uint32_t)idx == mPace )
    {
        if( nullptr != mFinalEventCallback )
          mFinalEventCallback( (uint32_t)idx );
        Suspend();
    } // if

    return true;

  } // CInvEffectSpriteShrink::ApplyEffect

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteShrink::Restore()
  {
    CInvEffect::Restore();
  } // CInvEffectSpriteShrink::Restore

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteShrink::AddEventCallback( FnEventCallback_t callback )
  {
    if( nullptr == callback )
    {
      LOG << "CInvEffectSpriteShrink::AddEventCallback: Warning: null final callback, ignoring.";
      return;
    } // if

    mFinalEventCallback = callback;
  } // CInvEffectSpriteAnimation::AddEventCallback

  //----------------------------------------------------------------------------------------------

} // namespace Inv
