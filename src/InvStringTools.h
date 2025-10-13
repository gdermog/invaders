//****************************************************************************************************
//! \file InvStringTools.h
//! Module contains convenient function for working with c-strings and std::string objects
//****************************************************************************************************
//*
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_InvStringTools
#define H_InvStringTools

#include <InvGlobals.h>

namespace Inv
{

  extern const std::string gDefaultTrimChars;
  //!< Default characters (whitespaces, crlf and so on) to be trimmed by Trim() function

  extern const char * gEmptyCharPtr;
  //!< Pointer to empty c-string

  extern const char * gTrueName;
  //!< Constant strings for "true"

  extern const char * gFalseName;
  //!< Constant strings for "false"

  inline bool IsNumberChar( unsigned char ch ) { return ( 48 <= ch && ch <= 57 ); }
  //!< \brief Returns <b>true</b> if the input character is a number (0-9).

  inline bool IsStandardChar( unsigned char ch )
  { return ( 64 <= ch && ch <= 90 ) || ( 97 <= ch && ch <= 122 ) || ch == '_'; }
  //!< \brief Returns <b>true</b> if the input character is a standard character (A-Z, a-z, _).

  inline bool IsNormalChar( unsigned char ch ) { return IsNumberChar( ch ) || IsStandardChar( ch ); }
  //!< \brief Returns <b>true</b> if the input character is a normal character or digit (0-9, A-Z, a-z, _).

  inline bool IsEmpty( const char * str ) { return ( nullptr == str || 0 == *str ); }
  //!< Returns true if the string is empty (nullptr or starts with a terminator)

  inline const char * NullEmptyStr( const char * val ) { return( nullptr == val ? gEmptyCharPtr : val ); }
  //!< Returns constant c-string pointer \e val or "" if \e val is nullptr

  NumberType_t IsNumeric( const char * str );
  //!< \brief Returns the type of a number in string form (see possibilities in NumberType_t). For higher
  //!  performance, procedure does not use regular expressions, but only the character types in the string,
  //!  i.e. for example the strings "-10", "1-0" and "10-" will be evaluated as Integer. For more thorough
  //!  checking (e.g. when implementing validators on user input) some better method needs to be used.

  int IsSeparator( char character, const char * separators );
  //!< \brief  Determines whether the given character is contained in the given string. Returns -1 if not
  //!  found, otherwise its index

  bool IEquals( const std::string & a, const std::string & b );
  //!< \brief Case insensitive comparison of two strings

  void Trim(
    const char * strin,
    size_t & first,
    size_t & last,
    size_t len = 0,
    const std::string & chars = gDefaultTrimChars );
  /*!< \brief Trims the input c - string by finding all characters contained in \e strin from its beginning
       and end. Characters are not removed from the original string, only the indexes of the first and last
       non-trimmable characters are returned.

       \param[in] strin   Input c-string to be trimmed
       \param[out] first  Index of the first non-trimmable character
       \param[out] last   Index of the last non-trimmable character + 1
       \param[in] len     Length of the input string. If 0, the length is determined by strlen()
       \param[in] chars   String containing all characters to be trimmed */


  std::string & Trim( std::string & s, const std::string & chars = gDefaultTrimChars );
  /*!< \brief Trims the input std::string by removing all characters contained in \e chars from its beginning
       and end. The original string is altered and a reference to it is returned.

       \param[in,out] s     Input string to be trimmed
       \param[in] chars     String containing all characters to be trimmed
       \return Reference to the trimmed string (allowing chains of calls) */

  bool StartsWith( std::string const & haystack, std::string const & needle );
  /*!< \brief Returns <b>true</b> if the C++ string <i>haystack</i> starts with the C++ string <i>needle</i>.

       \param[in] haystack  The string to be searched
       \param[in] needle    The string to be found at the beginning of haystack
       \return <b>true</b> if <i>haystack</i> starts with <i>needle</i>, <b>false</b> otherwise */

  bool EndsWith( std::string const & haystack, std::string const & needle );
  /*!< \brief Returns <b>true</b> if the C++ string <i>haystack</i> ends with the C++ string <i>needle</i>.

       \param[in] haystack  The string to be searched
       \param[in] needle    The string to be found at the end of haystack
       \return <b>true</b> if <i>haystack</i> ends with <i>needle</i>, <b>false</b> otherwise */

  size_t Unescape(
    std::string & data,
    bool nlq = true,
    std::string * externalBuffer = nullptr );
  /*!< \brief Looks through input string for C/C++ escaped metacharacters (as \\t or \\n) and unescapes
       them (to \t or \n)

       \param[in] data  String to be altered
       \param[in] nlq   Quotes and newlines are ignored if false
       \returns         Celkový počet nahrazených znaků */

  void SplitLine(
    StrVect_t & output,
    const char * line,
    const char * separators,
    const char * glues = nullptr,
    bool leaveBlanks = false,
    bool trim = false );
  /*!< Splits a string into its components based on the specified delimiters and returns it as
       a vector of std::string strings.

  \param[out] output      Resulting array
  \param[in] line         Original text
  \param[in] separators   List of delimiters
  \param[in] glues        List of characters preceding non-separable strings
  \param[in] leaveBlanks  If <b>true</b>, leaves blank items in the output array
  \param[in] trim         If <b>true</b>, trims all items by whitespace before storing them in the output */

  std::string JoinStrings( const StrVect_t & elements, const char * glue );
  /*!< \brief Joins an array of strings into a single string, with the specified glue string inserted
       between each element.

       \param[in] elements  Array of strings to be joined
       \param[in] glue      Glue string to be inserted between elements
       \return Joined string */

  std::string FormatScoreNumber( uint32_t inScore );
  /*!< \brief Formats given score number into a string with leading spaces, so that the resulting
       string is always 11 characters long. If the score is higher than 999999999, it is capped to
       "999999999". Thousands and millions are separated by space.

       \param[in] inScore Score number to be formatted
       \param[in] formatRight If <b>true</b>, the score is right-aligned, otherwise left-aligned
       \return Formatted score string */

  //****************************************************************************************************

  inline char format_argument_typecast( char value ) noexcept { return value; }

  inline int8_t format_argument_typecast( int8_t value ) noexcept { return value; }
  inline int16_t format_argument_typecast( int16_t value ) noexcept { return value; }
  inline int32_t format_argument_typecast( int32_t value ) noexcept { return value; }
  inline int64_t format_argument_typecast( int64_t value ) noexcept { return value; }
  inline uint8_t format_argument_typecast( uint8_t value ) noexcept { return value; }
  inline uint16_t format_argument_typecast( uint16_t value ) noexcept { return value; }
  inline uint32_t format_argument_typecast( uint32_t value ) noexcept { return value; }
  inline uint64_t format_argument_typecast( uint64_t value ) noexcept { return value; }
  inline float_t format_argument_typecast( float_t value ) noexcept { return value; }
  inline double_t format_argument_typecast( double_t value ) noexcept { return value; }
  inline bool format_argument_typecast( bool value ) noexcept { return value; }

  inline long format_argument_typecast( long value ) noexcept { return value; }
  inline unsigned long format_argument_typecast( unsigned long value ) noexcept { return value; }

  inline const char * format_argument_typecast( const char * value ) noexcept { return value; }
  inline const wchar_t * format_argument_typecast( const wchar_t * value ) noexcept { return value; }

  inline char * format_argument_typecast( char * value ) noexcept { return value; }
  inline wchar_t * format_argument_typecast( wchar_t * value ) noexcept { return value; }

  template<std::size_t N>
  inline const char * format_argument_typecast( const char( &value )[N] ) noexcept { return (const char *)&value; }

  inline const char * format_argument_typecast( const std::string & value ) noexcept { return value.c_str(); }

  inline const wchar_t * format_argument_typecast( const std::wstring & value ) noexcept { return value.c_str(); }

  template <typename T>
  inline const char * format_argument_typecast( const T & value ) noexcept { return value; }

  /*! \brief Formats a C++ string object using printf-style formatting. This function is little bit slow, as
      it is neccessary to call snprintf twice (first to determine the size of the resulting string, second
      to actually format it), but it is used mainly for loggin and error messages, where speed is not
      that important.

       \param[in] format  Format string (as in printf)
       \param[in] args    Arguments to be formatted
       \return Formatted string */
  template<typename... Args>
  std::string FormatStr( const std::string & format, const Args&... args )
  {
    size_t size = snprintf( nullptr, 0, format.c_str(), format_argument_typecast( args )... ) + 1;
    // The resulting length is calculated and an extra space is added for the terminator.

    std::unique_ptr<char[]> buf( new char[size] );
    snprintf( buf.get(), size, format.c_str(), format_argument_typecast( args )... );
    // But now we don't want a terminator in the chain

    return std::string( buf.get(), buf.get() + size - 1 );
  } /*FormatStr*/


} // namespace Inv

#endif
