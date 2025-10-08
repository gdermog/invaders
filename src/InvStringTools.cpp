//****************************************************************************************************
//! \file InvStringTools.cpp                                                                         *
//! Module contains convenient function for working with c-strings and std::string objects           *                                                                           *
//****************************************************************************************************
//*                                                                                                  *
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                       *
//****************************************************************************************************


#include <cctype>
#include <algorithm>

#include <InvStringTools.h>

namespace Inv
{

  const std::string gDefaultTrimChars = "\t\n\v\f\r \x11";
  const char * gEmptyCharPtr = "";
  const char * gTrueName = "true";
  const char * gFalseName = "false";

  //****************************************************************************************************

  void Trim( const char * strin, size_t & first, size_t & last, size_t len, const std::string & chars )
  {
    if( strin == nullptr || *strin == 0 ) { first = 0; last = 0; return; }
    const char * lastC;
    if( len > 0 ) lastC = strin + len - 1;
    else lastC = strin + strlen( strin ) - 1;
    const char * firstC = strin;
    while( IsSeparator( *firstC, chars.c_str() ) >= 0 && firstC < lastC ) ++firstC;
    while( IsSeparator( *lastC, chars.c_str() ) >= 0 && firstC <= lastC ) --lastC;
    first = (unsigned)( firstC - strin ); last = (unsigned)( lastC - strin + 1 );
    if( first == last && IsSeparator( *lastC, chars.c_str() ) >= 0 ) last = first - 1;
  } // Trim

  //****************************************************************************************************

  std::string & Trim( std::string & s, const std::string & chars )
  {
    s.erase( 0, s.find_first_not_of( chars ) );
    s.erase( s.find_last_not_of( chars ) + 1 );
    return s;
  } // Trim

  //****************************************************************************************************

  bool StartsWith( std::string const & haystack, std::string const & needle )
  {
    if( needle.size() > haystack.size() ) return false;
    return std::equal( needle.begin(), needle.end(), haystack.begin() );
  } // StartsWith

  //****************************************************************************************************

  bool EndsWith( std::string const & haystack, std::string const & needle )
  {
    if( needle.size() > haystack.size() ) return false;
    return std::equal( needle.rbegin(), needle.rend(), haystack.rbegin() );
  } // EndsWith

  //****************************************************************************************************

  size_t Unescape( std::string & data, bool nlq, std::string * externalBuffer )
  {
    std::string internalBuffer;
    std::string & buffer = ( externalBuffer == nullptr ? internalBuffer : *externalBuffer );
    buffer.reserve( data.size() );
    size_t escapedChars = 0;

    for( size_t pos = 0; pos < data.size(); ++pos )
    {
      if( data[pos] == '\\' )
      {
        switch( data[pos + 1] )
        {
        case 'n': if( nlq ) buffer.append( "\n" ); ++pos; ++escapedChars; continue;
        case '\'': if( nlq ) buffer.append( "\'" ); ++pos; ++escapedChars; continue;
        case '\"': if( nlq ) buffer.append( "\"" ); ++pos; ++escapedChars; continue;
        case 'a': buffer.append( "\a" ); ++pos; ++escapedChars; continue;
        case 'b': buffer.append( "\b" ); ++pos; ++escapedChars; continue;
        case 'f': buffer.append( "\f" ); ++pos; ++escapedChars; continue;
        case 'r': buffer.append( "\r" ); ++pos; ++escapedChars; continue;
        case 't': buffer.append( "\t" ); ++pos; ++escapedChars; continue;
        case 'v': buffer.append( "\v" ); ++pos; ++escapedChars; continue;
        case '\\': buffer.append( "\\" ); ++pos; ++escapedChars; continue;
        case '\?': buffer.append( "\?" ); ++pos; ++escapedChars; continue;
        } /*switch*/
      } /*if*/

      buffer.append( 1, data[pos] );
    } /*for*/

    data.swap( buffer );

    return escapedChars;

  } // Unescape

  //****************************************************************************************************

  NumberType_t IsNumeric( const char * str )
  {
    if( str == nullptr )
      return NumberType_t::kNothing;
    const char * ptr = str;

    if( *ptr == '0' && ( *( ptr + 1 ) == 'x' || *( ptr + 1 ) == 'X' ) )
    {
      ptr += 2;
      while( *ptr != 0 )
      {
        if( !( ( '0' <= *ptr && *ptr <= '9' ) ||
               ( 'A' <= *ptr && *ptr <= 'F' ) ||
               ( 'a' <= *ptr && *ptr <= 'f' ) ) )
          return NumberType_t::kNothing;
        ++ptr;
      } // while
      return NumberType_t::kHexaNumeric;
    } // if

    auto retVal = NumberType_t::kIndexNumeric;
    while( *ptr != 0 )
    {
      if( isdigit( (int)*ptr ) )
      {
        ++ptr;
        continue;
      } // if

      if( *ptr == '+' || *ptr == '-' )
      {
        if( retVal == NumberType_t::kIndexNumeric )
          retVal = NumberType_t::kIntegerNumeric;
        ++ptr;
        continue;
      } // if

      if( *ptr == '.' || *ptr == 'e' || *ptr == 'E' )
      {
        if( retVal != NumberType_t::kComplexNumeric )
        {
          if( *ptr == '.' && retVal != NumberType_t::kScientificNumeric )
            retVal = NumberType_t::kFloatNumeric;
          else
            retVal = NumberType_t::kScientificNumeric;
        } // if
        ++ptr;
        continue;
      } // if

      if( *ptr == 'i' )
      {
        retVal = NumberType_t::kComplexNumeric;
        ++ptr;
        continue;
      } // if

      return NumberType_t::kNothing;
    } // while

    return retVal;
  } // IsNumeric

  //****************************************************************************************************

  int IsSeparator( char character, const char * separators )
  {
    if( separators == nullptr ) return -1;
    const char * sepPtr = separators;
    while( *sepPtr != 0 )
    {
      if( character == *sepPtr )
        return (int)( sepPtr - separators );
      sepPtr++;
    }
    return -1;
  } // IsSeparator

  //****************************************************************************************************

  bool IEquals( const std::string & a, const std::string & b )
  {
    return
      std::equal( a.begin(), a.end(), b.begin(), b.end(),
        []( char a, char b )
        {
            return std::tolower( static_cast<unsigned char>( a ) ) ==
                   std::tolower( static_cast<unsigned char>( b ) );
        } );
  } // IEquals

  //****************************************************************************************************

  void SplitLine(
    StrVect_t & output,
    const char * line,
    const char * separators,
    const char * glues,
    bool leaveBlanks,
    bool trim )
  {
    output.clear();

    if( line == nullptr )
      return;

    if( separators == nullptr || *separators == 0 )
    {
      output.emplace_back( line );
      return;
    } // if

    size_t lineLength = strlen( line );
    if( lineLength < 1 )
      return;

    size_t beg = 0;     // Start of the item being loaded (index in the string)
    size_t strLoop;     // Search cycle control variable
    size_t len;         // Length of the found word
    bool isItem = false;// Item currently being read/not being read
    int isUnseparable = -1;
                        // Currently ignoring/not ignoring item separators (due to quotes)

    size_t first, last;

    for( strLoop = 0; strLoop < lineLength; strLoop++ )
    {
      if( isUnseparable >= 0 && line[strLoop] == '\\' &&
        IsSeparator( line[strLoop + 1], glues ) >= 0 ) continue;
                        // If part of a string is in quotes, the characters inside are expected
                        // to be escaped, i.e., for example, a double character \" must not end
                        // the sequence with quotes

      if( isUnseparable < 0 && IsSeparator( line[strLoop], separators ) >= 0 )
      {                 // Separator character found
        if( isItem )
        {               // An item is currently reading - it is therefore necessary to stop read.
          len = strLoop - beg;

          if( trim )
          {             // Finds the beginning and end of a string without whitespace characters
            Trim( line + beg, first, last, len );
            beg += first; len = last - first;
          } // if

          output.emplace_back( line + beg, len );
                        // The item string is stored.

          isItem = false;
                        // Item loading is complete.
        } // if
        else
        {
          if( leaveBlanks ) output.emplace_back();
          continue;
        } // else
                        // Skips separator character
      } // if
      else
      {                 // Common character found

        auto tmpSep = IsSeparator( line[strLoop], glues );
        if( tmpSep >= 0 )
        {               // If the loaded character is a glue character, it starts
                        //or ends a non-separable sequence (but it must be the same
                        // as the leading one).
          if( isUnseparable < 0 ) isUnseparable = tmpSep;
          else if( isUnseparable == tmpSep ) isUnseparable = -1;
        } // if

        if( isItem ) continue;
                        // Loading item - next item character found.
        else
        {               // The item is not loading - you need to start loading it.
          beg = strLoop;
          isItem = true;
          continue;
        } // else
      } // else
    } // for

    if( isItem )
    {                   // If the line ended when the last item was loaded, it needs to be written out.
      len = strLoop - beg;

      if( trim )
      {                 // Finds the beginning and end of a string without whitespace characters
        Trim( line + beg, first, last, len );
        beg += first; len = last - first;
      } // if

      output.emplace_back( line + beg, len );

    } // if

  } // SplitLine

  //****************************************************************************************************

  std::string JoinStrings( const StrVect_t & elements, const char * glue )
  {
    if( elements.empty() ) return {};
    if( glue == nullptr || *glue == 0 ) glue = "";

    std::string retVal;
    for( auto & element : elements )
    {
      if( !retVal.empty() )
        retVal.append( glue );
      retVal.append( element );
    } // for

    return retVal;

  } // JoinStrings

  //****************************************************************************************************

} // namespace Inv
