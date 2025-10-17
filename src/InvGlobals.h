//****************************************************************************************************
//! \file InvGlobals.h
//! Module contains some basics types and global constants used in other modules.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_InvGlobals
#define H_InvGlobals

#include <cmath>
#include <functional>
#include <cstdint>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <set>
#include <map>

#include <windows.h>

namespace Inv
{

  //! \brief Case insensitive string comparison functor for use in STL containers.
  struct cLess
  {
    bool operator()( const std::string & str1, const std::string & str2 ) const
    {
      auto * s1 = (unsigned char *)str1.c_str();
      auto * s2 = (unsigned char *)str2.c_str();
      unsigned char c1, c2;

      do
      {
        c1 = (unsigned char)toupper( (int)*s1++ );
        c2 = (unsigned char)toupper( (int)*s2++ );
        if( c1 == '\0' ) return c1 < c2;
      } while( c1 == c2 );

      return c1 < c2;
    } // cLess
  };

  using StrVect_t = std::vector<std::string>;
  //!< Vector of strings

  using StrSet_t = std::set<std::string>;
  //!< Set of strings

  template<typename T>
  using StrMap_t = std::map<std::string, T, cLess>;
  //!< Map indexed by string

  //! Number types
  enum class NumberType_t: unsigned short
  {
    kNothing            = 0,
    kIndexNumeric       = 1,
    kIntegerNumeric     = 2,
    kHexaNumeric        = 3,
    kFloatNumeric       = 4,
    kScientificNumeric  = 5,
    kComplexNumeric     = 6,
  };

  constexpr double_t gAlmostZero = 1E-12;
  //!< Specifies the size of a number that is already considered zero

  inline bool IsZero( double_t val, double_t tol = gAlmostZero ) { return ( ( -tol <= val ) && ( val <= tol ) ); }
  //!< Returns true if the value is zero within given tolerance

  inline bool IsPositive(double_t val, double_t tol = gAlmostZero ) { return ( tol < val ); }
  //!< Returns true if the value is positive within given tolerance

  inline bool IsNegative(double_t val, double_t tol = gAlmostZero) { return ( val < -tol ); }
  //!< Returns true if the value is negative within given tolerance

  constexpr unsigned gPrintoutIdWidth = 20;
  //!< Standard width for printout identifiers

  constexpr unsigned gHelpMarginWidth = 5;
  //!< Standard left margin for commandline help printout

  constexpr unsigned gHelpItemWidth = 20;
  //!< Standard width for commandline help items

  constexpr double_t gPI = 3.141592653589793;
  //!< Value of pi constant - 3.1415926535897932384626433832795 ...

  constexpr double_t g2PI = ( 2.0 * 3.141592653589793 );
  //!< Value of constant 2 * pi

  constexpr double_t gRadPerDeg = ( 3.141592653589793 / 180.0 );
  //!< Value of constant for conversion from degrees to radians

  constexpr double_t gDegPerRad = ( 180.0 / 3.141592653589793 );
  //!< Value of constant for conversion from radians to degrees

  //! A structure for our custom vertex type
  struct CUSTOMVERTEX
  {
    FLOAT x, y, z, rhw; //!< The transformed position for the vertex
    DWORD color;        //!< The vertex color
    float u, v;         //!< Texture coordinates
  };

  // Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#define LVL_MISSILE     0.2f
#define LVL_ALIEN       0.3f
#define LVL_PLAYER      0.4f
#define LVL_EXPLOSION   0.5f

  /*! \brief Control states, used to represent state of user input controls like
      keyboard keysor joystick buttons. The states can be combined using bitwise
      OR operator. */
  enum class ControlState_t: uint16_t
  {
    kNone               = 0x00,
    kLeft               = 0x01,
    kRight              = 0x02,
    kUp                 = 0x04,
    kDown               = 0x08,
    kFire               = 0x10,
    kSpecial            = 0x20,
    kStart              = 0x40,
  };

  using ControlStateFlags_t = std::underlying_type<ControlState_t>::type;
  //!< Type used to hold combination of ControlState_t values

  //! \brief Bitwise OR operator for ControlState_t enum, returns combination of two states.
  inline ControlStateFlags_t operator|( ControlState_t a, ControlState_t b )
  {
    return static_cast<ControlStateFlags_t>(
      static_cast<ControlStateFlags_t>( a ) | static_cast<ControlStateFlags_t>( b ) );
  } // operator|

  //! \brief Bitwise OR assignment operator for ControlState_t enum, combines two states.
  inline ControlStateFlags_t operator|=( ControlStateFlags_t & a, ControlState_t b )
  {
    a = static_cast<ControlStateFlags_t>(
      a | static_cast<ControlStateFlags_t>( b ) );
    return a;
  } // operator|=

  //! \brief Checks whether the specified flag combination contains the specified values(all of them).
  inline bool ControlStateHave( ControlStateFlags_t val, ControlState_t testingFor )
  {
    return ( static_cast<ControlStateFlags_t>( testingFor ) ==
      ( val & static_cast<ControlStateFlags_t>( testingFor ) ) );
  } // ControlStateHave

  //! \brief Sets the specified flag(s) in the flag combination.
  inline void ControlStateSet( ControlStateFlags_t & val, ControlState_t setting )
  {
    val = static_cast<ControlStateFlags_t>(
      val | static_cast<ControlStateFlags_t>( setting ) );
  } // ControlStateSet

  //! \brief Clears the specified flag(s) from the flag combination.
  inline void ControlStateClear( ControlStateFlags_t & val, ControlState_t clearing )
  {
    val = static_cast<ControlStateFlags_t>(
      val & ~static_cast<ControlStateFlags_t>( clearing ) );
  } // ControlStateClear

  using ControlValue_t = char;
  //!< Type used to hold value of pressed key

} // namespace Inv

#endif
