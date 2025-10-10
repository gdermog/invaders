//****************************************************************************************************
//! \file CInvEffectSpriteShrink.h
//! Module contains class CInvEffectSpriteShrink, which declares effect that animates sprite by
//! shrinking its image into given relative size in given pace
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvEffectSpriteShrink
#define H_CInvEffectSpriteShrink

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvEffect.h>

namespace Inv
{
  /*! \brief Effect that animates sprite by changing its image in given pace. The list of individual
      images that make up the Shrink is held by the CInvSprite class, to which the effect is
      applied. */
  class CInvEffectSpriteShrink: public CInvEffect
  {
    public:

    CInvEffectSpriteShrink(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    virtual ~CInvEffectSpriteShrink();

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

    virtual void Restore() override;
    //!< \brief Restores effect, it will be applied again

    void SetPace( uint32_t pace ) { mPace = pace; mTicksLeft = pace;  }
    /*!< \brief Sets pace of Shrink, number of ticks in which final size is reached. Smaller
         number means faster Shrink.

         \param[in] pace Number of ticks between changing to next image. */

    uint32_t GetPace() const { return mPace; }
    /*!< \brief Returns pace of Shrink, number of ticks between changing image state (full size
         to final size). */

    void SetFinalRatio( float ratio ) { mFinalRatio = ratio; }
    /*!< \brief Sets final ratio of size to original size, must be in range (0 - 1).

         \param[in] ratio Final ratio of size to original size, must be in range (0 - 1) */

    float GetFinalRatio() const { return mFinalRatio; }
    /*!< \brief Returns final ratio of final size to original size. */

    void AddEventCallback( FnEventCallback_t callback );
    /*!< \brief Adds an event callback that will be called when effect reaches end
         (only if mIsContinuous is false).

         \param[in] callback Callback function to be called, see FnEventCallback_t for details */

  private:

    uint32_t mPace;
    /*!< \brief Number of ticks between changing image state (full size to final size). */

    float mFinalRatio;
    /*!< \brief Final ratio of size to original size, must be in range (0 - 1). Default is 0.0f,
         which means the sprite shrinks to a point. */

    uint32_t mTicksLeft;
    /*!< \brief Number of ticks left to effect autosuspend/reset. */

    FnEventCallback_t mFinalEventCallback;
    /*!< \brief Callback function that will be called when animation reaches last image
         (only if mIsContinuous is false). */

  };

} // namespace Inv

#endif
