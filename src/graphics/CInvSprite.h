//****************************************************************************************************
//! \file CInvSprite.h                                                                          
//! Module contains class CInvSprite, which ..                                                                     
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvSprite
#define H_CInvSprite

#include <d3d9.h>
#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>


namespace Inv
{

  class CInvSprite
  {
    public:

    CInvSprite( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );
    CInvSprite( const CInvSprite & ) = delete;
    CInvSprite & operator=( const CInvSprite & ) = delete;
    ~CInvSprite();

    void AddSpriteImage( const std::string & imagePath );

    void Draw(
      size_t imageIndex,
      float xCentre, 
      float yCentre, 
      float xSize,
      float ySize,
      float rotateRad = 0, 
      DWORD color = 0xffffffff );

    std::pair<size_t, size_t> GetImageSize( size_t imageIndex ) const;

  private:

    const CInvSettings & mSettings;

    LPDIRECT3DDEVICE9 mPd3dDevice;

    std::vector<IDirect3DTexture9 *> mTextures;
    std::vector<std::pair<size_t, size_t>> mTextureSizes;
  };

} // namespace Inv

#endif
