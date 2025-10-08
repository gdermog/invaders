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

static const std::string lModLogId( "EffectSprite" );

namespace Inv
{

  CInvEffectSpriteAnimation::CInvEffectSpriteAnimation(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffect( settings, pd3dDevice, ePriority ),
    mPace( 1 )
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

    if( mIsSuspended )
      return true;

    auto * sprite = static_cast<Inv::CInvSprite *>( obj );

    LONGLONG idx = actualTick.QuadPart - referenceTick.QuadPart + diffTick.QuadPart;
    idx %= ( mPace * sprite->GetNumberOfImages() );
    sprite->mImageIndex = idx / mPace;

    return true;

  } // CInvEffectSpriteAnimation::ApplyEffect

  //----------------------------------------------------------------------------------------------

} // namespace Inv
