//****************************************************************************************************
//! \file CInvCollisionTest.cpp
//! Module defines class CInvCollisionTest that that tests whether two sprites are in collision.
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
  {}

  //----------------------------------------------------------------------------------------------

  CInvCollisionTest::~CInvCollisionTest() = default;

  //----------------------------------------------------------------------------------------------

  bool CInvCollisionTest::AreInCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const
  {
    //     if( ! CheckBoundingBoxCollision( sprite1, sprite2 ) )
    //       return false;

    if( !CheckPixelPerfectCollision( sprite1, sprite2 ) )
      return false;

    return true;

  } // CInvCollisionTest::AreInCollision

  //----------------------------------------------------------------------------------------------

//
//   bool CInvCollisionTest::CheckBoundingBoxCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const
//   {
//     float x1Min, y1Min, x1Max, y1Max;
//     sprite1.GetResultingBoundingBox( x1Min, x1Max, y1Min, y1Max );
//
//     float x2Min, y2Min, x2Max, y2Max;
//     sprite2.GetResultingBoundingBox( x2Min, x2Max, y2Min, y2Max );
//
//     if( ( x2Max < x1Min ) || ( x1Max < x2Min ) )
//       return false;
//
//     if( ( y2Max < y1Min ) || ( y1Max < y2Min ) )
//       return false;
//
//     return true;
//
//   } // CInvCollisionTest::CheckBoundingBoxCollision

  //----------------------------------------------------------------------------------------------

  void CInvCollisionTest::CalculateUV( int x, int y, RECT boundRect, const CUSTOMVERTEX vertices[], float * u, float * v )
  {

    float normalizedX = (float)( x - boundRect.left ) / (float)( boundRect.right - boundRect.left );
    float normalizedY = (float)( y - boundRect.top ) / (float)( boundRect.bottom - boundRect.top );
                        // Normalize the x and y coordinates relative to the bounding rectangle

    float uTop = vertices[0].u + ( vertices[1].u - vertices[0].u ) * normalizedX;
    float uBottom = vertices[2].u + ( vertices[3].u - vertices[2].u ) * normalizedX;
    *u = uTop + ( uBottom - uTop ) * normalizedY;
    float vLeft = vertices[0].v + ( vertices[2].v - vertices[0].v ) * normalizedY;
    float vRight = vertices[1].v + ( vertices[3].v - vertices[1].v ) * normalizedY;
    *v = vLeft + ( vRight - vLeft ) * normalizedX;
                        // Assume that the vertices are in the order: top left, top right,
                        // bottom left, bottom right, this calculates linear interpolation
                        // of input coordinate

  } // CInvCollisionTest::CalculateUV

  //----------------------------------------------------------------------------------------------

  D3DCOLOR CInvCollisionTest::GetPixelColor( D3DLOCKED_RECT lockedRect, float u, float v, IDirect3DTexture9 * texture )
  {

    D3DSURFACE_DESC desc;
    texture->GetLevelDesc( 0, &desc );
    int width = desc.Width;
    int height = desc.Height;
                        // Get texture dimensions

    int x = (int)( u * width );
    int y = (int)( v * height );
                        // Convert UV coordinates to pixel coordinates

    x = max( 0, min( x, width - 1 ) );
    y = max( 0, min( y, height - 1 ) );
                        // Edge treatment (clamp)

    BYTE * pPixel = (BYTE *)lockedRect.pBits + y * lockedRect.Pitch + x * 4;
                        // Calculate the offset into memory. We assume 32-bit (4 bytes) format

    return  *( (D3DCOLOR *)pPixel );

  } // CInvCollisionTest::GetPixelColor

  //----------------------------------------------------------------------------------------------

  bool CInvCollisionTest::CheckPixelPerfectCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const
  {
    float xMin, xMax, yMin, yMax;

    sprite1.GetResultingBoundingBox( xMin, xMax, yMin, yMax );
    RECT rect1{ (LONG)floorf( xMin ), (LONG)floorf( yMin ),  (LONG)ceilf( xMax ), (LONG)ceilf( yMax ) };

    sprite2.GetResultingBoundingBox( xMin, xMax, yMin, yMax );
    RECT rect2{ (LONG)floorf( xMin ), (LONG)floorf( yMin ), (LONG)ceilf( xMax ), (LONG)ceilf( yMax ) };

    RECT intersection;
    if( !IntersectRect( &intersection, &rect1, &rect2 ) )
      return false;     // First, a rough overlap of bouding rectangles is calculated. If the bounding
                        // rectangles of the two textures do not overlap at all, a collision cannot occur.

    D3DLOCKED_RECT lockedRect1, lockedRect2;
    HRESULT hr1 = sprite1.GetResultingTexture()->LockRect( 0, &lockedRect1, NULL, D3DLOCK_READONLY );
    HRESULT hr2 = sprite2.GetResultingTexture()->LockRect( 0, &lockedRect2, NULL, D3DLOCK_READONLY );
                        // Locking both textures to access pixel data directly.

    if( FAILED( hr1 ) || FAILED( hr2 ) )
    {
      LOG << "Error locking textures for pixel-perfect collision detection!";
      return false;
    } // if

    for( int y = intersection.top; y < intersection.bottom; ++y )
    {
      for( int x = intersection.left; x < intersection.right; ++x )
      {                 // Iterating through pixels in intersection rectangle.

        float u1, v1, u2, v2;
        CalculateUV( x, y, rect1, sprite1.GetResultingVertices(), &u1, &v1 );
        CalculateUV( x, y, rect2, sprite2.GetResultingVertices(), &u2, &v2 );
                        // Calculationg coordinates from absolute to relative for both textures.

        D3DCOLOR color1 = GetPixelColor( lockedRect1, u1, v1, sprite1.GetResultingTexture() );
        D3DCOLOR color2 = GetPixelColor( lockedRect2, u2, v2, sprite2.GetResultingTexture() );
                        // Getting pixel colors from both textures at calculated coordinates.

        BYTE alpha1 = ( color1 >> 24 ) & 0xFF;
        BYTE alpha2 = ( color2 >> 24 ) & 0xFF;
                        // Extracting alpha components from colors.

        if( alpha1 > mAlphaThreshold && alpha2 > mAlphaThreshold )
        {               // Non-transparent pixels (with some treshold) in both textures
                        // at calculated positions indicate a collision.
          sprite1.GetResultingTexture()->UnlockRect( 0 );
          sprite2.GetResultingTexture()->UnlockRect( 0 );
          return true;
        } // if
      } // for x
    } // for y

    sprite1.GetResultingTexture()->UnlockRect( 0 );
    sprite2.GetResultingTexture()->UnlockRect( 0 );
                        // Unlocking both textures after processing.

    return false;       // No collision detected after checking all pixels in intersection area.

  } // CInvCollisionTest::CheckPixelPerfectCollision

  //----------------------------------------------------------------------------------------------

} // namespace Inv
