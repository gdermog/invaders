//****************************************************************************************************
//! \file CInvText.cpp                                                                         
//! Module contains class CInvText, which implements singleton pattern for global logging                                                                    
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
    DWORD color )
  {
    
    float x = xTopLeft + letterSize * 0.5;
    float y = xTopLeft + letterSize * 0.5;

    for( auto ltr : txt )
    {
      auto it = mLetterMap.find( std::tolower( ltr ) );
      if( it != mLetterMap.end() )
      {
        auto & sprite = it->second;
        sprite->Draw(
          0, x, y, 0.5*letterSize, 0.5*letterSize, 0, color );
      } // if

      x += letterSize;
    }


  } // CInvText::Draw

} // namespace Inv
