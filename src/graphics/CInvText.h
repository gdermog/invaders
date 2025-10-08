//****************************************************************************************************
//! \file CInvText.h
//! Module declares class CInvText, which represents text that can be drawn on screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvText
#define H_CInvText

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>

#include <graphics/CInvSprite.h>


namespace Inv
{

  /*! \brief The class represents text that can be drawn on screen. The text is made up of individual
      letters, each represented by a sprite. The letters are loaded from individual image files, which
      are expected to be in a specified directory. The class provides methods to draw text at specified
      position and size, with optional color and effects. The class is designed to work with Direct3D 9
      and uses Direct3D textures for the letter images. */
  class CInvText
  {
    public:

    CInvText(
      const CInvSettings & settings,
      LPDIRECT3DDEVICE9 pd3dDevice,
      const std::string & lettersRelPath );

    CInvText( const CInvText & ) = delete;
    CInvText & operator=( const CInvText & ) = delete;
    ~CInvText();

    void Draw(
      const std::string &txt,
      float xTopLeft,
      float yTopLeft,
      float letterSize,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick,
      DWORD color = 0xffffffff ) const;
    /*!< \brief Draws the text at given position and size, applying all effects before drawing.

         \param[in] txt           Text to be drawn
         \param[in] xTopLeft      X coordinate of top left corner of the text
         \param[in] yTopLeft      Y coordinate of top left corner of the text
         \param[in] letterSize    Demanded width and height of letters in pixels
         \param[in] referenceTick Reference tick, usually time when effect was started
         \param[in] actualTick    Current tick
         \param[in] diffTick      An artificially introduced correction to the current tick,
                                  used when one effect is applied to multiple objects and the
                                  results are required to differ somewhat from each other.
         \param[in] color         Color to modulate the text with, default is white (no change) */

    void DrawFromRight(
      const std::string & txt,
      float xTopLeft,
      float yTopLeft,
      float width,
      float letterSize,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick,
      DWORD color = 0xffffffff ) const;
    /*!< \brief Draws the text right-aligned within given width at given position and size, applying
         all effects before drawing.

         \param[in] txt           Text to be drawn
         \param[in] xTopLeft      X coordinate of top left corner of the area in which text is drawn
         \param[in] yTopLeft      Y coordinate of top left corner of the area in which text is drawn
         \param[in] width         Width of area in which text is drawn, text will be right-aligned
         \param[in] letterSize    Demanded width and height of letters in pixels
         \param[in] referenceTick Reference tick, usually time when effect was started
         \param[in] actualTick    Current tick
         \param[in] diffTick      An artificially introduced correction to the current tick,
                                  used when one effect is applied to multiple objects and the
                                  results are required to differ somewhat from each other.
         \param[in] color         Color to modulate the text with, default is white (no change) */

  private:

    const CInvSettings & mSettings;
    //<! Reference to settings object, all parameters are taken from here

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //<! Direct3D device, used to set and get scissor rectangle

    std::map<char, std::unique_ptr<CInvSprite>> mLetterMap;
    //<! Map of letters, each represented by a sprite, indexed by character

  };

} // namespace Inv

#endif
