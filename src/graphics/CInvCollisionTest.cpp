//****************************************************************************************************
//! \file CInvCollisionTest.cpp
//! Module defines class CInvCollisionTest that represents a 2D CollisionTest which can be drawn on screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <filesystem>

#include <d3dx9.h>

#include <graphics/CInvCollisionTest.h>

#include <CInvLogger.h>


static const std::string lModLogId( "CollisionTest" );

namespace Inv
{
  CInvCollisionTest::CInvCollisionTest( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice ):
    mSettings( settings ),
    mPd3dDevice( pd3dDevice )
  {
  }

  //----------------------------------------------------------------------------------------------

  CInvCollisionTest::~CInvCollisionTest()
  {}

  //----------------------------------------------------------------------------------------------

  bool CInvCollisionTest::AreInCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const
  {
    if( ! CheckBoundingBoxCollision( sprite1, sprite2 ) )
      return false;

    if( ! CheckPixelPerfectCollision( sprite1, sprite2 ) )
      return false;

    return true;

  } // CInvCollisionTest::AreInCollision

  //----------------------------------------------------------------------------------------------

  bool CInvCollisionTest::CheckBoundingBoxCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const
  {
    float x1Min, y1Min, x1Max, y1Max;
    sprite1.GetResultingBoundingBox( x1Min, x1Max, y1Min, y1Max );

    float x2Min, y2Min, x2Max, y2Max;
    sprite2.GetResultingBoundingBox( x2Min, x2Max, y2Min, y2Max );

    if( ( x2Max < x1Min ) || ( x1Max < x2Min ) )
      return false;

    if( ( y2Max < y1Min ) || ( y1Max < y2Min ) )
      return false;

    return true;

  } // CInvCollisionTest::CheckBoundingBoxCollision

  //----------------------------------------------------------------------------------------------

  bool CInvCollisionTest::CheckPixelPerfectCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const
  {
    return true;
  } // CInvCollisionTest::CheckPixelPerfectCollision

  //----------------------------------------------------------------------------------------------

} // namespace Inv
