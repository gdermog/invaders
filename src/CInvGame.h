//****************************************************************************************************
//! \file CInvGame.h
//! Module declares class CInvGame, which implements the main control loop of the game.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvGame
#define H_CInvGame

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>
#include <CInvSettings.h>
#include <CInvSettingsRuntime.h>
#include <CInvRandom.h>
#include <CInvAudio.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvSpriteStorage.h>
#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>
#include <graphics/CInvBackground.h>

#include <engine/CInvHiscoreList.h>
#include <engine/CInvInsertCoinScreen.h>
#include <engine/CInvPlayItScreen.h>

namespace Inv
{

  /*! \brief Class implements the main control loop of the game. The class manages the overall game
      state, including initialization, main loop, and cleanup. It uses other classes to handle specific
      aspects of the game, such as rendering, input handling, and game logic. The class provides
      methods to initialize the game, run the main loop, and clean up resources when the game ends.
      The class is designed to work with Direct3D 9 for rendering and uses a window created using
      the Win32 API. */
  class CInvGame
  {
  public:

    CInvGame( const CInvSettings & settings );

    CInvGame( const CInvGame & ) = delete;
    CInvGame & operator=( const CInvGame & ) = delete;
    ~CInvGame();

    bool Initialize();
    //!< Initializes game, returns true if successful

    bool Run();
    //!< Runs the game, returns true if successful

    bool Cleanup();
    //!< Releases resources of the game, returns true if successful

  private:

    HRESULT InitD3D();
    //!< Initializes Direct3D, returns true if successful

    HRESULT InitVB();
    //!< Initializes vertex buffer, returns true if successful

    bool IsKeyDown( int key );
    //!< Returns true if given key is currently pressed

    void ProcessInput( ControlStateFlags_t & controlState, ControlValue_t & controlValue );
    /*!< \brief Processes user input, updates control state and value
     *!<
         \param[out] controlState Current state of user controls, combination of ControlStateFlags_t
                                  values.
         \param[out] controlValue Value of key pressed */

    std::unique_ptr<CInvHiscoreList> mHiscoreKeeper;
    //!< \brief High score list object, used to access and modify high scores.

    std::unique_ptr <CInvPrimitive> mPrimitives;
    //!< \brief Primitive rendering object, used to draw basic shapes on screen.

    std::unique_ptr<CInvInsertCoinScreen> mInsertCoinScreen;
    //!< \brief "Insert Coin" screen object, used to manage the initial game screen.

    std::unique_ptr<CInvPlayItScreen> mPlayItScreen;
    //!< \brief "Play It" screen object, used to manage the game screen.

    std::unique_ptr<CInvSpriteStorage> mSpriteStorage;
    //!< \brief Sprite storage object, used to manage and access sprites.

    std::unique_ptr<CInvBackground> mBackgroundInsertCoin;
    //!< \brief Reference to background object, used to draw the "insert coin" background

    std::unique_ptr<CInvBackground> mBackgroundPlay;
    //!< \brief Reference to background object, used to draw the game background

    std::unique_ptr<CInvAudio> mAudio;
    //!< \brief Audio engine object, used to play sounds and music.

    std::unique_ptr<CInvSound> mInsertCoinMusic;
    //<! Sound object used to play "insert coin" screen music

    std::unique_ptr<CInvSound> mPlayItMusic;
    //<! Sound object used to play "play it" screen music

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings object, used to access configuration parameters.

    CInvSettingsRuntime mSettingsRuntime;
    //!< \brief Runtime settings object, used to access and modify parameters created and updated
    //     by game engine.

    WNDCLASSEX mWindowClass;
    //<! \brief Window class structure, used to create the game window.

    HWND mHWnd;
    //<! \brief Handle to the game window.

    LARGE_INTEGER mReferenceTick;
    //<! \brief Reference tick, used for timing and animations and actions.

    uint32_t mMillisecondsPerTick;
    //<! Demanded number of milliseconds per tick

    LPDIRECT3D9             mPD3D;
    //<! Direct3D interface, used to create device
    LPDIRECT3DDEVICE9       mPd3dDevice;
    //<! Direct3D device, used to draw on screen
    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //<! Vertex buffer, used to draw primitives

    DWORD mClearColor;
    //<! Color used to clear the screen each frame

    LONGLONG mLoopElapsedMicrosecondsMax;

    float mLoopElapsedMicrosecondsAvg;

    float mLoopWaitedMicrosecondsAvg;

    uint64_t mLoopElapsedMicrosecondsAvgCount;

    static const std::wstring mWindiwClassId;
    //<! Identifier of window class

    static const std::wstring mWindiwName;
    //<! Name of the window

  };

} // namespace Inv

#endif
