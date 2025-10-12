//****************************************************************************************************
//! \file CInvEffectSpriteShiftRotate.h
//! Module contains class CInvEffectSpriteShiftRotate, which makes sprite circling around a center
//! position.
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

  /*! \brief The effect moves the sprite in the specified direction. However, this direction
      is rotated over time, resulting in the sprite circling around a center position. */
  class CInvEffectSpriteShiftRotate: public CInvEffectSpriteShift
  {
    public:

    CInvEffectSpriteShiftRotate(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      uint32_t ePriority );

    virtual ~CInvEffectSpriteShiftRotate();

    virtual bool ApplyEffect(
      void *sprite,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick ) override;

    void SetPace( uint32_t pace ) { mPace = pace; }
    //!< Sets pace of rotation, number of ticks per full rotation

    uint32_t GetPace() const { return mPace; }
    //!< Returns pace of rotation, number of ticks per full rotation

  protected:

    uint32_t mPace;
    //!< Ticks per full rotation

  };

} // namespace Inv

#endif
