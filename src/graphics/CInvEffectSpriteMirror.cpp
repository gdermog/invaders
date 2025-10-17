//****************************************************************************************************
//! \file CInvEffectSpriteMirror.cpp
//! Module contains class CInvEffectSpriteMirror, which defines effect that displays sprite mirrored
//! along vertical axis.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <d3dx9.h>

#include <graphics/CInvEffectSpriteMirror.h>

#include <graphics/CInvSprite.h>

static const std::string lModLogId( "EffectSprite" );

namespace Inv
{

  CInvEffectSpriteMirror::CInvEffectSpriteMirror(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    CInvEffect( settings, pd3dDevice, ePriority )
  {}

  //----------------------------------------------------------------------------------------------

  CInvEffectSpriteMirror::~CInvEffectSpriteMirror() = default;

  //----------------------------------------------------------------------------------------------

  bool CInvEffectSpriteMirror::ApplyEffect(
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

    std::swap( sprite->mTea2[0].x, sprite->mTea2[1].x );
    std::swap( sprite->mTea2[2].x, sprite->mTea2[3].x );

    return true;

  } // CInvEffectSpriteMirror::ApplyEffect

  //----------------------------------------------------------------------------------------------

} // namespace Inv
