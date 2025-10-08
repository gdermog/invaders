//****************************************************************************************************
//! \file CInvText.h
//! Module contains class CInvText, which ..
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

  private:

    const CInvSettings & mSettings;

    LPDIRECT3DDEVICE9 mPd3dDevice;

    std::map<char, std::unique_ptr<CInvSprite>> mLetterMap;

  };

} // namespace Inv

#endif
