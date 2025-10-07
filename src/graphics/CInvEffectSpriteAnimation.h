//****************************************************************************************************
//! \file CInvEffectSpriteAnimation.h                                                                          
//! Module contains class CInvEffectSpriteAnimation, which ..                                                                     
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvEffectSpriteAnimation
#define H_CInvEffectSpriteAnimation

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvEffect.h>

namespace Inv
{

  class CInvEffectSpriteAnimation: public CInvEffect
  {
    public:

    CInvEffectSpriteAnimation( 
      const CInvSettings & settings, 
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    CInvEffectSpriteAnimation( const CInvEffectSpriteAnimation & ) = delete;
    CInvEffectSpriteAnimation & operator=( const CInvEffectSpriteAnimation & ) = delete;
    virtual ~CInvEffectSpriteAnimation();


    virtual bool ApplyEffect(
      void * obj,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick ) override;

    void SetPace( uint32_t pace ) { mPace = pace; }

    uint32_t GetPace() const { return mPace; }

  private:

    uint32_t mPace;

  };

} // namespace Inv

#endif
