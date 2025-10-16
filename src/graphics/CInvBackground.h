//****************************************************************************************************
//! \file CInvBackground.h
//! Module declares class CInvBackground that represents a 2D Background which can be drawn on screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvBackground
#define H_CInvBackground

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

namespace Inv
{


  /*! \brief Class represents a 2D Background that can be drawn on screen. The Background can hold multiple
      images, which can be switched to create simple animations. The Background can have multiple effects
      applied to it, which can modify its properties such as position, size, rotation, etc. The class
      is designed to work with Direct3D 9 and uses Direct3D textures for the images. */
  class CInvBackground
  {

    public:

    CInvBackground( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );

    CInvBackground( const CInvBackground & ) = delete;
    CInvBackground & operator=( const CInvBackground & ) = delete;

    ~CInvBackground();

    void AddBackgroundImage( const std::string & imageName );
    /*!< \brief Adds single image to Background

         \param[in] imageName Name of image file to be loaded as texture, relative
                    path to mSettings.GetImagePath() is expected. */

    void Draw(
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick,
      DWORD color = 0xffffffff ) const;
    /*!< \brief Draws the background onto the device.

         \param[in] referenceTick Reference tick, usually time when effect was started
         \param[in] actualTick    Current tick
         \param[in] diffTick      An artificially introduced correction to the current tick,
                                  used when one effect is applied to multiple objects and the
                                  results are required to differ somewhat from each other.
         \param[in] color         Color to modulate the Background with, default is white (no change) */


    std::pair<size_t, size_t> GetImageSize() const { return mTextureSize; }
    /*!< \brief Returns original size of the background image.

         \return Pair of width and height in pixels */

    IDirect3DTexture9 * GetTexture() const { return mTexture; }

    void SetRollCoefficient( float coef ) { mRollCoef = coef; }
    /*!< \brief Sets roll coefficient of background. Higher coefficient means faster rolling,
         default is 0.33f. Zero means stationary image. */

  private:

    float mVprWidth;

    float mVprHeight;

    float mTxtrWidth;

    float mTxtrHeight;

    float mLvl;

    float mRollCoef;

    mutable CUSTOMVERTEX mTea2[4];

    const CInvSettings & mSettings;
    //<! Reference to settings object, to access global settings

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //!< Direct3D device, used to create textures (Background images)

    IDirect3DTexture9 * mTexture;
    //!< List of textures (images) that make up the Background

    std::pair<size_t, size_t> mTextureSize;
    //!< List of sizes of individual images, in pixels

  };

} // namespace Inv

#endif
