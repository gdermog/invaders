//****************************************************************************************************
//! \file CInvEffect.h                                                                          
//! Module contains class CInvEffect, which ..                                                                     
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

  class CInvEffect
  {
    public:

    CInvEffect( 
      const CInvSettings & settings, 
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    CInvEffect( const CInvEffect & ) = delete;
    CInvEffect & operator=( const CInvEffect & ) = delete;
    virtual ~CInvEffect();

    uint32_t GetEffectPriority() const { return mEffectPriority; }

    virtual bool ApplyEffect(
      void * element,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick ) = 0;

    void Suspend() { mIsSuspended = true; }

    void Restore() { mIsSuspended = false; }

  protected:

    const CInvSettings & mSettings;

    LPDIRECT3DDEVICE9 mPd3dDevice;

    bool mIsSuspended;

    uint32_t mEffectPriority;
  };

} // namespace Inv

#endif
