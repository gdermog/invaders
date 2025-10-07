//****************************************************************************************************
//! \file CInvEffectSpriteShiftRotate.h                                                                          
//! Module contains class CInvEffectSpriteShiftRotate, which ..                                                                     
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvEffectSpriteShiftRotate
#define H_CInvEffectSpriteShiftRotate

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvEffectSpriteShift.h>

namespace Inv
{

  class CInvSprite;

  class CInvEffectSpriteShiftRotate: public CInvEffectSpriteShift
  {
    public:

    CInvEffectSpriteShiftRotate( 
      const CInvSettings & settings, 
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    CInvEffectSpriteShiftRotate( const CInvEffectSpriteShiftRotate & ) = delete;
    CInvEffectSpriteShiftRotate & operator=( const CInvEffectSpriteShiftRotate & ) = delete;
    virtual ~CInvEffectSpriteShiftRotate();

    virtual bool ApplyEffect(
      void *sprite,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick ) override;

    void SetPace( uint32_t pace ) { mPace = pace; }

    uint32_t GetPace() const { return mPace; }

//     void SetAngleRad( float angle ) { mAngleRad = angle; }
// 
//     float GetAngleRad() const { return mAngleRad; }

  protected:

    uint32_t mPace;     //!< Ticks per full rotation

 //   float mAngleRad;

  };

} // namespace Inv

#endif
