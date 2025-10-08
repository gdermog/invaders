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
    /*!< \brief Applies effect to given element, returns true if element was changed

        \param[in,out] element   Pointer to element to which effect is applied, actual
                                 type depends on effect. CInvSprite or its descendant is expected
                                 here. Beware, no smart typecheck is implemented, it is up to
                                 caller to provide correct type.
        \param[in] referenceTick Reference tick, usually time when effect was started
        \param[in] actualTick    Current tick
        \param[in] diffTick      An artificially introduced correction to the current tick,
                                 used when one effect is applied to multiple objects and the
                                 results are required to differ somewhat from each other. */

    void SetShift( float shiftX, float shiftY ) { mShiftX = shiftX; mShiftY = shiftY; }
    /*!< \brief Sets shift in X and Y direction that will be applied to sprite every time the
         effect is applied. Default is (0,0), which means no shift. */

    void GetShift( float & shiftX, float & shiftY ) const { shiftX = mShiftX; shiftY = mShiftY; }
    /*!< \brief Returns shift in X and Y direction that will be applied to sprite every time the
         effect is applied. */


  protected:

    float mShiftX;
    //!< \brief Shift in X direction that will be applied to sprite every time the effect is applied.

    float mShiftY;
    //!< \brief Shift in Y direction that will be applied to sprite every time the effect is applied.

  };

} // namespace Inv

#endif
