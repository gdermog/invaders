//****************************************************************************************************
//! \file CInvEffectSpriteShift.h                                                                          
//! Module contains class CInvEffectSpriteShift, which ..                                                                     
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvEffectSpriteShift
#define H_CInvEffectSpriteShift

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvSprite.h>

namespace Inv
{

  class CInvSprite;

  class CInvEffectSpriteShift: public CInvEffect
  {
    public:

    CInvEffectSpriteShift( 
      const CInvSettings & settings, 
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    CInvEffectSpriteShift( const CInvEffectSpriteShift & ) = delete;
    CInvEffectSpriteShift & operator=( const CInvEffectSpriteShift & ) = delete;
    ~CInvEffectSpriteShift();

    virtual bool ApplyEffect(
      void *sprite,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick ) override;

    void SetShift( float shiftX, float shiftY ) 
    { mShiftX = shiftX; mShiftY = shiftY; }

    void GetShift( float & shiftX, float & shiftY ) const
    { shiftX = mShiftX; shiftY = mShiftY; }

  protected:

    float mShiftX;
    float mShiftY;

  };

} // namespace Inv

#endif
