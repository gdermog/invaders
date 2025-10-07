//****************************************************************************************************
//! \file CInvEffectSpriteShiftRotate.cpp                                                                         
//! Module contains class CInvEffectSpriteShiftRotate, which implements singleton pattern for global logging                                                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteShiftRotate.h>

#include <CInvLogger.h>
#include <InvStringTools.h>

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

  CInvEffectSpriteShiftRotate::~CInvEffectSpriteShiftRotate()
  {}

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

    float x = mShiftX;
    float y = mShiftY;

    auto angleIdx = 
      ( actualTick.QuadPart - referenceTick.QuadPart + diffTick.QuadPart ) % mPace;

    float angleRad = g2PI * ( (float)angleIdx ) / ( (float)mPace );

    float c = cosf( angleRad );
    float s = sinf( angleRad );

    mShiftX = x * c - y * s;
    mShiftY = y * c + x * s;

    auto retVal = CInvEffectSpriteShift::ApplyEffect( obj, referenceTick, actualTick, diffTick );

    mShiftX = x;
    mShiftY = y;

    return retVal;

  } // CInvEffectSpriteShiftRotate::ApplyEffect

  //----------------------------------------------------------------------------------------------

} // namespace Inv
