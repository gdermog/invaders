//****************************************************************************************************
//! \file CInvEffectSpriteBlink.cpp
//! Module contains class CInvEffectSpriteBlink, which defines effect that animates sprite by
//! showing and hiding its image in given pace
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteBlink.h>

#include <graphics/CInvSprite.h>

#include <CInvLogger.h>

static const std::string lModLogId( "EffectSprite" );

namespace Inv
{

  CInvEffectSpriteBlink::CInvEffectSpriteBlink(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffect( settings, pd3dDevice, ePriority ),
    mPace( 1 ),
    mTicksSpan{ 0 },
    mTicksLeft{ 0 }
  {}

  //----------------------------------------------------------------------------------------------

  CInvEffectSpriteBlink::~CInvEffectSpriteBlink()
  {}

  //----------------------------------------------------------------------------------------------

  bool CInvEffectSpriteBlink::ApplyEffect(
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
    idx /= mPace;
    if( ( 1 == ( idx % 2 ) ) )
      sprite->mImageIndex = SIZE_MAX;

    if( !IsContinuous() )
    {
      if( mTicksLeft.QuadPart > 0 )
        --mTicksLeft.QuadPart;
      else
      {
        if( nullptr != mFinalEventCallback )
          mFinalEventCallback( 0 );
        Suspend();
      } // else
    } // if
    else
    {
      if( mTicksLeft.QuadPart > 0 )
        --mTicksLeft.QuadPart;
      else
        mTicksLeft.QuadPart = mTicksSpan.QuadPart;
    }

    return true;

  } // CInvEffectSpriteBlink::ApplyEffect

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteBlink::Restore()
  {
    mTicksLeft = mTicksSpan;
    CInvEffect::Restore();
  } // CInvEffectSpriteBlink::Restore

  //----------------------------------------------------------------------------------------------

  void CInvEffectSpriteBlink::AddEventCallback( FnEventCallback_t callback )
  {
    if( nullptr == callback )
    {
      LOG << "CInvEffectSpriteBlink::AddEventCallback: Warning: null final callback, ignoring.";
      return;
    } // if

    mFinalEventCallback = callback;
  } // CInvEffectSpriteAnimation::AddEventCallback

  //----------------------------------------------------------------------------------------------

} // namespace Inv
