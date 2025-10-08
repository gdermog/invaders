//****************************************************************************************************
//! \file CInvPrimitive.h
//! Module declares class CInvPrimitive, which provides methods to draw basic primitives such as
//! lines and rectangles.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvPrimitive
#define H_CInvPrimitive

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>


namespace Inv
{

  /*! \brief The class provides methods to draw basic primitives such as lines and rectangles.
      The primitives are drawn using Direct3D 9 device. The class is designed to work with
      Direct3D 9 and uses Direct3D device for drawing. */
  class CInvPrimitive
  {
    public:

    CInvPrimitive( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );
    CInvPrimitive( const CInvPrimitive & ) = delete;
    CInvPrimitive & operator=( const CInvPrimitive & ) = delete;
    ~CInvPrimitive();

    void DrawLine(
      float x1, float y1,
      float x2, float y2,
      D3DCOLOR color,
      bool pixelPerfect = false );
    /*!< \brief Draws a line between two points with given color.

         \param[in] x1, y1       Coordinates of the start point of the line
         \param[in] x2, y2       Coordinates of the end point of the line
         \param[in] color        Color of the line in D3DCOLOR format
         \param[in] pixelPerfect If true, applies half-pixel correction for sharper lines */

    void DrawSquare(
      float x1, float y1,
      float x2, float y2,
      D3DCOLOR color,
      bool pixelPerfect = false );
    /*!< \brief Draws a empty rectangle defined by two opposite corners with given color.

         \param[in] x1, y1       Coordinates of one corner of the rectangle
         \param[in] x2, y2       Coordinates of the opposite corner of the rectangle
         \param[in] color        Color of the rectangle in D3DCOLOR format
         \param[in] pixelPerfect If true, applies half-pixel correction for sharper edges */

  private:

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings

    LPDIRECT3DDEVICE9 mPd3dDevice;
    //!< \brief Pointer to Direct3D device

  };

} // namespace Inv

#endif
