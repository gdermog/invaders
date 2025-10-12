//****************************************************************************************************
//! \file CInvEffectSpriteAnimation.h
//! Module contains class CInvEffectSpriteAnimation, which declares effect that animates sprite by
//! changing its image in given pace.
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

    virtual void Restore() override;
    //!< \brief Restores effect, it will be applied again

    void SetPace( uint32_t pace ) { mPace = pace; }
    /*!< \brief Sets pace of animation, number of ticks between changing to next image.
         Smaller number means faster animation. Default is 1, which means image changes
         every tick. */

    uint32_t GetPace() const { return mPace; }
    /*!< \brief Returns pace of animation, number of ticks between changing to next image. */

    void SetImageRange( uint32_t firstImage, uint32_t lastImage );
    /*!< \brief Sets range of images to be used in animation. If range is not set
         explicitly, all images in sprite are used.

          \param[in] firstImage Index of first image to be used in animation
          \param[in] lastImage  Index of last image to be used in animation */



    void AddEventCallback( FnEventCallback_t callback );
    /*!< \brief Adds an event callback that will be called when animation reaches last image
         (only if mIsContinuous is false).

         \param[in] callback Callback function to be called, see FnEventCallback_t for details */

    void AddEventCallback( uint32_t imageIndex, FnEventCallback_t callback );
    /*!< \brief Adds an event callback that will be called when animation reaches given image index.

         \param[in] imageIndex Index of image at which callback is triggered
         \param[in] callback   Callback function to be called, see FnEventCallback_t for details */

  private:

    uint32_t mPace;
    /*!< \brief Pace of animation, number of ticks between changing to next image.Smaller
          number means faster animation. Default is 1, which means image changes every tick. */

    uint32_t mFirstImage;
    /*!< \brief Index of first image to be used in animation. */

    uint32_t mLastImage;
    /*!< \brief Index of last image to be used in animation. */

    bool mFinalEventCallbackReported;
    /*!< \brief Internal flag to prevent multiple calls of final event callback if animation.
         Flag is reseted on calling Restore(). */

    FnEventCallback_t mFinalEventCallback;
    /*!< \brief Callback function that will be called when animation reaches last image
         (only if mIsContinuous is false). */

    std::map<uint32_t, std::pair<bool,FnEventCallback_t>> mEventCallbacks;
    /*!< \brief Map of event callbacks, where key is image index at which callback is triggered,
         and value is the callback function. Boolean flag is set to true when the callback is
         triggered, so it is not triggered again in next tick which results in the same image
         index. Flags are reseted when last image is reached. */

  };

} // namespace Inv

#endif
