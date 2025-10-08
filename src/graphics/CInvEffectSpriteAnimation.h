//****************************************************************************************************
//! \file CInvEffectSpriteAnimation.h
//! Module contains class CInvEffectSpriteAnimation, which declares effect that animates sprite by
//! changing its image in given pace
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
  /*! \brief Effect that animates sprite by changing its image in given pace. The list of individual
      images that make up the animation is held by the CInvSprite class, to which the effect is
      applied. */
  class CInvEffectSpriteAnimation: public CInvEffect
  {
    public:

    CInvEffectSpriteAnimation(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    virtual ~CInvEffectSpriteAnimation();

    virtual bool ApplyEffect(
      void * obj,
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

    void SetPace( uint32_t pace ) { mPace = pace; }
    /*!< \brief Sets pace of animation, number of ticks between changing to next image.
         Smaller number means faster animation. Default is 1, which means image changes
         every tick. */

    uint32_t GetPace() const { return mPace; }
    /*!< \brief Returns pace of animation, number of ticks between changing to next image. */

  private:

    uint32_t mPace;
    /*!< \brief Pace of animation, number of ticks between changing to next image.Smaller
          number means faster animation. Default is 1, which means image changes every tick. */


  };

} // namespace Inv

#endif
