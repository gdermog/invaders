//****************************************************************************************************
//! \file CInvEffectSpriteShift.cpp
//! Module contains class CInvEffectSpriteShift, which defines effect that shifts sprite by given
//! offset in X and Y direction.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteShift.h>

#include <graphics/CInvSprite.h>

static const std::string lModLogId( "EffectSprite" );

namespace Inv
{

  CInvEffectSpriteShift::CInvEffectSpriteShift(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffect( settings, pd3dDevice, ePriority ),
    mShiftX( 0.0f ),
    mShiftY( 0.0f )
  {}

  //----------------------------------------------------------------------------------------------

  CInvEffectSpriteShift::~CInvEffectSpriteShift()
  {}

  //----------------------------------------------------------------------------------------------

  bool CInvEffectSpriteShift::ApplyEffect(
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

    sprite->mTea2[0].x += mShiftX;
    sprite->mTea2[1].x += mShiftX;
    sprite->mTea2[2].x += mShiftX;
    sprite->mTea2[3].x += mShiftX;

    sprite->mTea2[0].y += mShiftY;
    sprite->mTea2[1].y += mShiftY;
    sprite->mTea2[2].y += mShiftY;
    sprite->mTea2[3].y += mShiftY;

    return true;

  } // CInvEffectSpriteShift::ApplyEffect

  //----------------------------------------------------------------------------------------------

} // namespace Inv
