//****************************************************************************************************
//! \file CInvEffectSpriteBlink.h
//! Module contains class CInvEffectSpriteBlink, which declares effect that animates sprite by
//! showing and hiding its image in given pace
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvEffectSpriteBlink
#define H_CInvEffectSpriteBlink

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvEffect.h>

namespace Inv
{
  /*! \brief Effect that animates sprite by changing its image in given pace. The list of individual
      images that make up the Blink is held by the CInvSprite class, to which the effect is
      applied. */
  class CInvEffectSpriteBlink: public CInvEffect
  {
    public:

    CInvEffectSpriteBlink(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    virtual ~CInvEffectSpriteBlink();

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

    void SetPace( uint32_t pace ) { mPace = pace; }
    /*!< \brief Sets pace of blink, number of ticks between changing image state (shown
         or hidden). */

    uint32_t GetPace() const { return mPace; }
    /*!< \brief Returns pace of blink, number of ticks between changing image state (shown
         or hidden). */

    void SetIgnoreDiffTick( bool ignore ) { mIgnoreDiffTick = ignore; }

    void SetTicks( uint32_t ticks ) { mTicksSpan.QuadPart = ticks; mTicksLeft.QuadPart = ticks; }
    /*!< \brief Sets number of ticks left to effect autosuspend. */

    uint32_t GetTicksSpan() const { return (uint32_t)mTicksSpan.QuadPart; }
    /*!< \brief Returns number of ticks in which the effect is active. */

    uint32_t GetTicksLeft() const { return (uint32_t)mTicksLeft.QuadPart; }
    /*!< \brief Returns number of ticks left to effect autosuspend. */

    void AddEventCallback( FnEventCallback_t callback );
    /*!< \brief Adds an event callback that will be called when effect reaches end
         (only if mIsContinuous is false).

         \param[in] callback Callback function to be called, see FnEventCallback_t for details */

  private:

    uint32_t mPace;
    /*!< \brief Pace of Blink, number of ticks between changing to next image.Smaller
          number means faster Blink. Default is 1, which means image changes every tick. */

    bool mIgnoreDiffTick;


    LARGE_INTEGER mTicksSpan;
    /*!< \brief Number of ticks in which the effect is active. */

    LARGE_INTEGER mTicksLeft;
    /*!< \brief Number of ticks left to effect autosuspend. */

    FnEventCallback_t mFinalEventCallback;
    /*!< \brief Callback function that will be called when animation reaches last image
         (only if mIsContinuous is false). */

  };

} // namespace Inv

#endif
