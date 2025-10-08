//****************************************************************************************************
//! \file CInvSprite.h
//! Module declares class CInvSprite that represents a 2D sprite which can be drawn on screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvSprite
#define H_CInvSprite

#include <d3d9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>
#include <graphics/CInvEffectSpriteAnimation.h>

namespace Inv
{


  /*! \brief Class represents a 2D sprite that can be drawn on screen. The sprite can hold multiple
      images, which can be switched to create simple animations. The sprite can have multiple effects
      applied to it, which can modify its properties such as position, size, rotation, etc. The class
      is designed to work with Direct3D 9 and uses Direct3D textures for the images. */
  class CInvSprite
  {
    //------ List of allowed effect classes that can access sprite internals -------------------------

    friend class CInvEffectSpriteAnimation;
    friend class CInvEffectSpriteShift;
    friend class CInvEffectSpriteShiftRotate;

    public:

    CInvSprite( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );
    CInvSprite( const CInvSprite & ) = delete;
    CInvSprite & operator=( const CInvSprite & ) = delete;
    ~CInvSprite();

    void AddSpriteImage( const std::string & imageName );
    /*!< \brief Adds single image to sprite

         \param[in] imageName Name of image file to be loaded as texture, relative
                    path to mSettings.GetImagePath() is expected. */

    void AddMultipleSpriteImages( const std::string & imageNameTemplate );
    /*!< \brief Adds multiple images to sprite, according to given template. The template should
         contain a single '%d' format specifier, which will be replaced by consecutive numbers
         starting from 1. The function will attempt to load images until it finds a number for
         which the corresponding file does not exist.

         \param[in] imageNameTemplate Template for image file names, relative path to
                                      mSettings.GetImagePath() is expected. Example: "sprite_%03d.png"
                                      will load files "sprite_001.png", "sprite_002.png", ... until
                                      a file is not found. */

    size_t GetNumberOfImages() const { return mTextures.size(); }
    /*!< \brief Returns number of images currently loaded in the sprite. */

    void Draw(
      float xCentre,
      float yCentre,
      float xSize,
      float ySize,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick,
      DWORD color = 0xffffffff );
    /*!< \brief Draws the sprite at given position and size, applying all effects before drawing.

         \param[in] xCentre       X coordinate of sprite center
         \param[in] yCentre       Y coordinate of sprite center
         \param[in] xSize         Demanded width of sprite
         \param[in] ySize         Demanded height of sprite
         \param[in] referenceTick Reference tick, usually time when effect was started
         \param[in] actualTick    Current tick
         \param[in] diffTick      An artificially introduced correction to the current tick,
                                  used when one effect is applied to multiple objects and the
                                  results are required to differ somewhat from each other.
         \param[in] color         Color to modulate the sprite with, default is white (no change) */

    void AddEffect( std::shared_ptr<CInvEffect> effect );
    /*!< \brief Adds effect to sprite, if not already present

         \param[in] effect Shared pointer to effect to be added */

    void RemoveEffect( std::shared_ptr<CInvEffect> effect );
    /*!< \brief Removes effect from sprite, if present

         \param[in] effect Shared pointer to effect to be removed */

    std::pair<size_t, size_t> GetImageSize( size_t imageIndex ) const;
    /*!< \brief Returns size of image at given index, in pixels. If index is out of range,
         (0,0) is returned.

         \param[in] imageIndex Index of image whose size is requested
         \return Pair of width and height in pixels */

  protected:

    CUSTOMVERTEX mTea2[4];
    //!< Vertices of the sprite. They are protected to allow effects to modify them directly.

    size_t mImageIndex;
    //!< Index of image to be drawn, can be modified by effects

    float mHalfSizeX;
    //!< Half of the size in X direction, can be modified by effects

    float mHalfSizeY;
    //!< Half of the size in Y direction, can be modified by effects

    std::map<uint32_t, std::vector<std::shared_ptr<CInvEffect>>> mEffects;
    //!< Map of effects applied to the sprite, indexed by effect category

  private:

    const CInvSettings & mSettings;
    //<! Reference to settings object, to access global settings

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //!< Direct3D device, used to create textures (sprite images)

    std::vector<IDirect3DTexture9 *> mTextures;
    //!< List of textures (images) that make up the sprite

    std::vector<std::pair<size_t, size_t>> mTextureSizes;
    //!< List of sizes of individual images, in pixels

  };

} // namespace Inv

#endif
