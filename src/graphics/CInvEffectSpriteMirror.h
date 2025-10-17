//****************************************************************************************************
//! \Mirror CInvEffectSpriteMirror.h
//! Module contains class CInvEffectSpriteMirror, which declares effect that displays sprite mirrored
//! along vertical axis.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvEffectSpriteMirror
#define H_CInvEffectSpriteMirror

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvSprite.h>

namespace Inv
{

  /*! \brief Effect that displays sprite mirrored along vertical axis. */
  class CInvEffectSpriteMirror: public CInvEffect
  {
    public:

    CInvEffectSpriteMirror(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    ~CInvEffectSpriteMirror();

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


  };

} // namespace Inv

#endif
