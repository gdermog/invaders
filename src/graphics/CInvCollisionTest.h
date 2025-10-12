//****************************************************************************************************
//! \file CInvCollisionTest.h
//! Module declares class CInvCollisionTest that tests whether two sprites are in collision.
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


  /*! \brief Class represents a 2D CollisionTest that tests whether two sprites are in collision.*/
  class CInvCollisionTest
  {

    public:

    CInvCollisionTest( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );

    CInvCollisionTest( const CInvCollisionTest & ) = delete;
    CInvCollisionTest & operator=( const CInvCollisionTest & ) = delete;
    ~CInvCollisionTest();

    bool AreInCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const;
    /*!< \brief Tests whether two sprites are in collision.

         \param[in] sprite1   First sprite to be tested
         \param[in] sprite2   Second sprite to be tested
         \return \b true if the sprites are in collision, false otherwise. */

  private:

    //bool CheckBoundingBoxCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const;

    bool CheckPixelPerfectCollision( const CInvSprite & sprite1, const CInvSprite & sprite2 ) const;

    static void CalculateUV( int x, int y, RECT boundRect, const CUSTOMVERTEX vertices[], float * u, float * v );
    /*!< \brief Calculates texture coordinates (u,v) for given pixel (x,y) on the game scene.

         \TODO: This function assumes that the rectangle is not rotated nor deformed. If rotation
                or any complex effect is needed, function calculation capabilities must be extended
                accordingly.

         \param[in] x           X coordinate of pixel, relative to top-left corner of rectangle
         \param[in] y           Y coordinate of pixel, relative to top-left corner of rectangle
         \param[in] boundRect   Bounding rectangle of the sprite on the game scene (min/max X and Y)
         \param[in] vertices    Array of 4 CUSTOMVERTEX structures, defining the area of the sprite
                                in which the texture is mapped.
         \param[out] u          Calculated U (relative texture) coordinate
         \param[out] v          Calculated V (relative texture) coordinate */

    static D3DCOLOR GetPixelColor( D3DLOCKED_RECT lockedRect, float u, float v, IDirect3DTexture9 * texture );
    /*!< \brief Gets color of pixel at given (u,v) coordinates from locked texture.

         \param[in] lockedRect Locked rectangle of the texture, providing access to pixel data
         \param[in] u          U (relative texture) coordinate of pixel
         \param[in] v          V (relative texture) coordinate of pixel
         \param[in] texture    Pointer to texture from which the pixel is read
         \return Color of the pixel at given (u,v) coordinates */

    static constexpr BYTE mAlphaThreshold = 10;
    //!< Alpha threshold for pixel-perfect collision detection, pixels with alpha below this
    //!  value are considered transparent

    const CInvSettings & mSettings;
    //<! Reference to settings object, to access global settings

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //!< Direct3D device, used to create textures (CollisionTest images)

  };

} // namespace Inv

#endif
