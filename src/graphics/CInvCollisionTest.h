//****************************************************************************************************
//! \file CInvCollisionTest.h
//! Module declares class CInvCollisionTest that ..
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvCollisionTest
#define H_CInvCollisionTest

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>
#include <graphics/CInvSprite.h>

namespace Inv
{


  /*! \brief Class represents a 2D CollisionTest that ... */
  class CInvCollisionTest
  {

    public:

    CInvCollisionTest( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );

    CInvCollisionTest( const CInvCollisionTest & ) = delete;
    CInvCollisionTest & operator=( const CInvCollisionTest & ) = delete;
    ~CInvCollisionTest();

    bool AreInCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const;

  private:

    bool CheckBoundingBoxCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const;

    bool CheckPixelPerfectCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const;

    const CInvSettings & mSettings;
    //<! Reference to settings object, to access global settings

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //!< Direct3D device, used to create textures (CollisionTest images)

  };

} // namespace Inv

#endif
