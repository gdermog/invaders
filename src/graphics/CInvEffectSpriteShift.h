//****************************************************************************************************
//! \file CInvEffectSpriteShift.h
//! Module contains class CInvEffectSpriteShift, which declares effect that shifts sprite by given
//! offset in X and Y direction.
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

  /*! \brief Effect that shifts sprite by given offset in X and Y direction. The shift is applied
      every time the effect is applied, so multiple applications will result in cumulative
      shift. The effect can be used to implement simple movement of sprites. The effect itself is
      not time-dependent, it is a simple translation. The axis itself is not very important, it is
      used more as a base class for more advanced effects involving translating an object in some
      direction.*/
  class CInvEffectSpriteShift: public CInvEffect
  {
    public:

    CInvEffectSpriteShift(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

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
