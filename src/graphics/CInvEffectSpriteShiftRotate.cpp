//****************************************************************************************************
//! \file CInvEffectSpriteShiftRotate.cpp
//! Module contains class CInvEffectSpriteShiftRotate, which makes sprite circling around a center
//! position.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteShiftRotate.h>

#include <graphics/CInvSprite.h>

static const std::string lModLogId( "EffectSpriteRotate" );

namespace Inv
{

  CInvEffectSpriteShiftRotate::CInvEffectSpriteShiftRotate(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffectSpriteShift( settings, pd3dDevice, ePriority ),
    mPace( 1 )
  {}

  //----------------------------------------------------------------------------------------------

  CInvEffectSpriteShiftRotate::~CInvEffectSpriteShiftRotate() = default;

  //----------------------------------------------------------------------------------------------

  bool CInvEffectSpriteShiftRotate::ApplyEffect(
    void * obj,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick )
  {
    if( nullptr == obj )
      return false;

    if( mIsSuspended )
      return true;

    auto angleIdx =
      ( actualTick.QuadPart - referenceTick.QuadPart + diffTick.QuadPart ) % mPace;

    float angleRad = ((float)g2PI) * ( (float)angleIdx ) / ( (float)mPace );

    float c = cosf( angleRad );
    float s = sinf( angleRad );

    float x = mShiftX * c - mShiftY * s;
    float y = mShiftY * c + mShiftX * s;

    auto * sprite = static_cast<Inv::CInvSprite *>( obj );

    sprite->mTea2[0].x += x;
    sprite->mTea2[1].x += x;
    sprite->mTea2[2].x += x;
    sprite->mTea2[3].x += x;

    sprite->mTea2[0].y += y;
    sprite->mTea2[1].y += y;
    sprite->mTea2[2].y += y;
    sprite->mTea2[3].y += y;

    return true;

  } // CInvEffectSpriteShiftRotate::ApplyEffect

  //----------------------------------------------------------------------------------------------

} // namespace Inv
