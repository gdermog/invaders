//****************************************************************************************************
//! \file CInvEffect.cpp                                                                         
//! Module contains class CInvEffect, which implements singleton pattern for global logging                                                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <d3dx9.h>
#include <graphics/CInvEffect.h>

static const std::string lModLogId( "Effect" );

namespace Inv
{

  CInvEffect::CInvEffect( 
    const CInvSettings & settings, 
    LPDIRECT3DDEVICE9 pd3dDevice,
    uint32_t ePriority ):

    mSettings( settings ),
    mPd3dDevice( pd3dDevice ),
    mIsSuspended( false ),
    mEffectPriority( ePriority )
  {} 

  //----------------------------------------------------------------------------------------------

  CInvEffect::~CInvEffect()
  {}

  //----------------------------------------------------------------------------------------------

} // namespace Inv
