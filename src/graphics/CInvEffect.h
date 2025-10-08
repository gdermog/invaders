//****************************************************************************************************
//! \file CInvEffect.h
//! Module contains class CInvEffect, which declares base class for all effects that can be applied
//! to graphical elements.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvEffect
#define H_CInvEffect

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

namespace Inv
{

  /*! \brief Base class for all effects that can be applied to graphical elements like
      sprites or text. Class is pure virtual, basically defines only interface for
      an effect usage. */
  class CInvEffect
  {
    public:


    CInvEffect(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    virtual ~CInvEffect();

    uint32_t GetEffectPriority() const { return mEffectPriority; }
    //!< \brief Returns effect priority, lower number means higher priority

    virtual bool ApplyEffect(
      void * element,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick ) = 0;
    /*!< \brief Applies effect to given element, returns true if element was changed

        \param[in,out] element   Pointer to element to which effect is applied, actual
                                 type depends on effect. Beware, no smart typecheck is
                                 implemented, it is up to caller to provide correct type.
        \param[in] referenceTick Reference tick, usually time when effect was started
        \param[in] actualTick    Current tick
        \param[in] diffTick      An artificially introduced correction to the current tick,
                                 used when one effect is applied to multiple objects and the
                                 results are required to differ somewhat from each other. */

    void Suspend() { mIsSuspended = true; }
    //!< \brief Suspends effect, it will not be applied until restored

    void Restore() { mIsSuspended = false; }
    //!< \brief Restores effect, it will be applied again

    bool IsSuspended() const { return mIsSuspended; }
    //!< \brief Returns true if effect is currently suspended

  protected:

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //!< \brief Pointer to Direct3D device

    bool mIsSuspended;
    //!< \brief If true, effect is suspended and will not be applied

    uint32_t mEffectPriority;
    //!< \brief Effect priority, lower number means higher priority

  };

} // namespace Inv

#endif
