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
  std::map<char, std::unique_ptr<CInvSprite>> CInvText::mLetterMap;

  //----------------------------------------------------------------------------------------------

  CInvText::CInvText(
    const std::string & txt,
    const CInvSettings & settings,
    LPDIRECT3DDEVICE9 pd3dDevice ):
    mSettings( settings ),
    mPd3dDevice( pd3dDevice ),
    mText( txt )
  {

    if( mLetterMap.empty() )
    {

      std::string imgName;

      for( char ch = 'a'; ch <= 'z'; ++ch )
      {
        auto sprite = std::make_unique<CInvSprite>( mSettings, mPd3dDevice );
        imgName = FormatStr( "letters/%clet.png", ch );
        sprite->AddSpriteImage( imgName );
        mLetterMap[ch] = std::move( sprite );
      } // for

      for( char ch = '0'; ch <= '9'; ++ch )
      {
        auto sprite = std::make_unique<CInvSprite>( mSettings, mPd3dDevice );
        imgName = FormatStr( "letters/num%c.png", ch );
        sprite->AddSpriteImage( imgName );
        mLetterMap[ch] = std::move( sprite );
      } // for
    } // if

  } // CInvText::CInvText

  //----------------------------------------------------------------------------------------------

  CInvText::~CInvText()
  {}
  //----------------------------------------------------------------------------------------------

  void CInvText::AddEffect( std::shared_ptr<CInvEffect> effect )
  {
    if( nullptr == effect )
      return;

    auto & efList = mEffects[effect->GetEffectPriority()];
    if( std::find( efList.begin(), efList.end(), effect ) == efList.end() )
      efList.push_back( effect );

    mLocalLetterMap.clear();

  } // CInvSprite::AddEffect

  //----------------------------------------------------------------------------------------------

  void CInvText::RemoveEffect( std::shared_ptr<CInvEffect> effect )
  {
    if( nullptr == effect )
      return;
    auto & efList = mEffects[effect->GetEffectPriority()];

    auto it = std::find( efList.begin(), efList.end(), effect );
    if( it != efList.end() )
      efList.erase( it );

    mLocalLetterMap.clear();

  } // CInvSprite::RemoveEffect

  //----------------------------------------------------------------------------------------------

  void CInvText::Draw(
    float xTopLeft,
    float yTopLeft,
    float letterSize,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick,
    DWORD color ) const
  {

    RefillLocalLetterMap();

    float x = xTopLeft + letterSize * 0.5f;
    float y = yTopLeft + letterSize * 0.5f;
                        // Sprites are drawn from center, one letter - one sprite

    CInvSprite * sprite = nullptr;

    for( auto rit : mText )
    {
      auto ltr = std::tolower( rit );

      auto itloc = mLocalLetterMap.find( std::tolower( ltr ) );
      if( itloc != mLocalLetterMap.end() )
         sprite = itloc->second.get();

      if( nullptr == sprite )
      {
        auto itglob = mLetterMap.find( std::tolower( ltr ) );
        if( itglob != mLetterMap.end() )
          sprite = itglob->second.get();
      } // if

      if( nullptr != sprite )
      {
        sprite->Draw(
          x, y, letterSize, letterSize,
          referenceTick, actualTick, diffTick,
          color );
        diffTick.QuadPart++;
      } // if

      x += letterSize;
      sprite = nullptr;
    } // for


  } // CInvText::Draw

  //----------------------------------------------------------------------------------------------

  void CInvText::DrawFromRight(
    float xTopLeft,
    float yTopLeft,
    float width,
    float letterSize,
    LARGE_INTEGER referenceTick,
    LARGE_INTEGER actualTick,
    LARGE_INTEGER diffTick,
    DWORD color ) const
  {
    RefillLocalLetterMap();

    float x = xTopLeft + width - letterSize * 0.5f;
    float y = yTopLeft + letterSize * 0.5f;
                        // Sprites are drawn from center, one letter - one sprite

    CInvSprite * sprite = nullptr;

    for( auto rit = mText.rbegin(); rit != mText.rend(); ++rit )
    {
      auto ltr = std::tolower( *rit );

      auto itloc = mLocalLetterMap.find( ltr );
      if( itloc != mLocalLetterMap.end() )
        sprite = itloc->second.get();

      if( nullptr == sprite )
      {
        auto itglob = mLetterMap.find( ltr );
        if( itglob != mLetterMap.end() )
          sprite = itglob->second.get();
      } // if

      if( nullptr != sprite )
      {
        sprite->Draw(
          x, y, letterSize, letterSize,
          referenceTick, actualTick, diffTick,
          color );
        diffTick.QuadPart++;
      } // if

      x -= letterSize;
      if( x < xTopLeft )
        break;
      sprite = nullptr;

    } // for

  } // CInvText::DrawFromRight

  //----------------------------------------------------------------------------------------------

  void CInvText::RefillLocalLetterMap() const
  {
    if( !mEffects.empty() && mLocalLetterMap.empty() )
    {
      for( auto letter : mText )
      {
        auto lttr = std::tolower( letter );
        if( 0 < mLocalLetterMap.count( lttr ) )
          continue;

        auto it = mLetterMap.find( std::tolower( lttr ) );
        auto localSprite = std::make_unique<CInvSprite>( *it->second );
        for( auto & effectCategory : mEffects )
        {
          for( auto & ef : effectCategory.second )
            localSprite->AddEffect( ef );
        } // for
        mLocalLetterMap[lttr] = std::move( localSprite );
      } // for
    }

  }

} // namespace Inv
