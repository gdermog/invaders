//****************************************************************************************************
//! \file CInvText.cpp
//! Module defines class CInvText, which represents text that can be drawn on screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <filesystem>

#include <CInvLogger.h>
#include <InvStringTools.h>
#include <graphics/CInvText.h>

static const std::string lModLogId( "Text" );

namespace Inv
{
  CInvText::CInvText(
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice,
    const std::string & lettersRelPath ):
    mSettings( settings ),
    mPd3dDevice( pd3dDevice )
  {

    std::string imgName;

    for( char ch = 'a'; ch <= 'z'; ++ch )
    {
      auto sprite = std::make_unique<CInvSprite>( mSettings, mPd3dDevice );
      imgName = FormatStr( "%s/%clet.png", lettersRelPath.c_str(),ch );
      sprite->AddSpriteImage( imgName );
      mLetterMap[ch] = std::move( sprite );
    } // for

    for( char ch = '0'; ch <= '9'; ++ch )
    {
      auto sprite = std::make_unique<CInvSprite>( mSettings, mPd3dDevice );
      imgName = FormatStr( "%s/num%c.png",lettersRelPath.c_str(), ch );
      sprite->AddSpriteImage( imgName );
      mLetterMap[ch] = std::move( sprite );
    } // for

  } // CInvText::CInvText

  //----------------------------------------------------------------------------------------------

  CInvText::~CInvText()
  {}

  //----------------------------------------------------------------------------------------------

  void CInvText::Draw(
    const std::string & txt,
    float xTopLeft,
    float yTopLeft,
    float letterSize,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick,
    DWORD color ) const
  {

    float x = xTopLeft + letterSize * 0.5f;
    float y = yTopLeft + letterSize * 0.5f;
                        // Sprites are drawn from center, one letter - one sprite

    for( auto ltr : txt )
    {
      auto it = mLetterMap.find( std::tolower( ltr ) );
      if( it != mLetterMap.end() )
      {
        auto & sprite = it->second;
        sprite->Draw(
          x, y, letterSize, letterSize,
          referenceTick, actualTick, diffTick,
          color );
      } // if

      x += letterSize;
    } // for


  } // CInvText::Draw

  //----------------------------------------------------------------------------------------------

  void CInvText::DrawFromRight(
    const std::string & txt,
    float xTopLeft,
    float yTopLeft,
    float width,
    float letterSize,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick,
    DWORD color ) const
  {

    float x = xTopLeft + width - letterSize * 0.5f;
    float y = yTopLeft + letterSize * 0.5f;
                        // Sprites are drawn from center, one letter - one sprite

    for( auto rit = txt.rbegin(); rit != txt.rend(); ++rit )
    {
      auto ltr = *rit;
      auto it = mLetterMap.find( std::tolower( ltr ) );
      if( it != mLetterMap.end() )
      {
        auto & sprite = it->second;
        sprite->Draw(
          x, y, letterSize, letterSize,
          referenceTick, actualTick, diffTick,
          color );
      } // if

      x -= letterSize;
      if( x < xTopLeft )
        break;

    } // for

  } // CInvText::DrawFromRight

} // namespace Inv
