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
//#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>
#include <graphics/CInvEffectSpriteAnimation.h>
#include <Windows.h>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Inv
{

  

  class CInvSprite
  {
    friend class CInvEffectSpriteAnimation;
    friend class CInvEffectSpriteShift;

    public:

    CInvSprite( const CInvSettings & settings, LPDIRECT3DDEVICE9 pd3dDevice );
    CInvSprite( const CInvSprite & ) = delete;
    CInvSprite & operator=( const CInvSprite & ) = delete;
    ~CInvSprite();

    void AddSpriteImage( const std::string & imageName );

    void AddMultipleSpriteImages( const std::string & imageNameTemplate );

    size_t GetNumberOfImages() const { return mTextures.size(); }

    void Draw(
      float xCentre, 
      float yCentre, 
      float xSize,
      float ySize,
      LARGE_INTEGER referenceTick,
      LARGE_INTEGER actualTick,
      LARGE_INTEGER diffTick,
      DWORD color = 0xffffffff );

    void AddEffect( std::shared_ptr<CInvEffect> effect );
    void RemoveEffect( std::shared_ptr<CInvEffect> effect );

    std::pair<size_t, size_t> GetImageSize( size_t imageIndex ) const;

  protected:

    CUSTOMVERTEX mTea2[4];

    size_t mImageIndex;

    float mHalfSizeX;
    float mHalfSizeY;

    std::map<uint32_t, std::vector<std::shared_ptr<CInvEffect>>> mEffects;

  private:

    const CInvSettings & mSettings;

    LPDIRECT3DDEVICE9 mPd3dDevice;

    std::vector<IDirect3DTexture9 *> mTextures;
    std::vector<std::pair<size_t, size_t>> mTextureSizes;
  };

} // namespace Inv

#endif
