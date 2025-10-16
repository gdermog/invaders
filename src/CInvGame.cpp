//****************************************************************************************************
//! \file CInvLoggger.cpp
//! Module defines class CInvGame, which implements the main control loop of the game.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <CInvGame.h>
#include <CInvLogger.h>

static const std::string lModLogId( "GAMELOOP" );

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------

static bool lKeyDown[256];
//static int lKeyHit[256];
static int lMouseButton;

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch( msg )
  {
  case WM_LBUTTONDOWN:
    SetCapture( hWnd );
    lMouseButton |= 1;
    lKeyDown[VK_LBUTTON] = true;
 //   lKeyHit[VK_LBUTTON]++;
    break;

  case WM_RBUTTONDOWN:
    SetCapture( hWnd );
    lKeyDown[VK_RBUTTON] = true;
 //   lKeyHit[VK_RBUTTON]++;
    lMouseButton |= 2;
    break;

  case WM_MBUTTONDOWN:
    SetCapture( hWnd );
    lMouseButton |= 4;
    lKeyDown[VK_MBUTTON] = true;
 //   lKeyHit[VK_MBUTTON]++;
    break;

  case WM_LBUTTONUP:
    ReleaseCapture();
    lMouseButton &= ~1;
    lKeyDown[VK_LBUTTON] = false;
    break;

  case WM_RBUTTONUP:
    ReleaseCapture();
    lMouseButton &= ~2;
    lKeyDown[VK_RBUTTON] = false;
    break;

  case WM_MBUTTONUP:
    ReleaseCapture();
    lMouseButton &= ~4;
    lKeyDown[VK_MBUTTON] = false;
    break;

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    lKeyDown[wParam & 255] = true;
//    lKeyHit[wParam & 255]++;
    return 0;

  case WM_KEYUP:
  case WM_SYSKEYUP:
    lKeyDown[wParam & 255] = false;
    break;

  case WM_DESTROY:
    PostQuitMessage( 0 );
    return 0;

  case WM_ACTIVATEAPP:
    if( !wParam )
      memset( lKeyDown, 0, sizeof( lKeyDown ) );
    break;

  case WM_ACTIVATE:
    if( WA_INACTIVE != wParam )
    {
      // Make sure the device is acquired, if we are gaining focus.

    }
    break;
  }
  return DefWindowProc( hWnd, msg, wParam, lParam );
} // MsgProc

//****** CInvGame implementation ****************************************************************

namespace Inv
{
  CInvGame::CInvGame( const CInvSettings & settings ):
    mHiscoreKeeper( nullptr ),
    mPrimitives( nullptr ),
    mInsertCoinScreen( nullptr ),
    mPlayItScreen( nullptr ),
    mSpriteStorage( nullptr ),
    mBackgroundInsertCoin( nullptr ),
    mBackgroundPlay( nullptr ),
    mSettings( settings ),
    mSettingsRuntime(),
    mWindowClass{},
    mHWnd{},
    mReferenceTick{},
    mMillisecondsPerTick( 1000 / settings.GetTickPerSecond() ),
    mPD3D( nullptr ),
    mPd3dDevice( nullptr ),
    mPVB( nullptr ),
    mClearColor( D3DCOLOR_XRGB( 0, 0, 0 ) ),
    mLoopElapsedMicrosecondsMax( 0 ),
    mLoopElapsedMicrosecondsAvg( 0.0f ),
    mLoopWaitedMicrosecondsAvg( 0.0f ),
    mLoopElapsedMicrosecondsAvgCount( 0 )
  {}

  //-------------------------------------------------------------------------------------------------

  CInvGame::~CInvGame()
  {
    if( nullptr != mPD3D )
      mPD3D->Release();
    if( nullptr != mPd3dDevice )
      mPd3dDevice->Release();
    if( nullptr != mPVB )
      mPVB->Release();
  } // CInvGame::~CInvGame

  //-------------------------------------------------------------------------------------------------

  const std::wstring CInvGame::mWindiwClassId = L"Invaders";

  const std::wstring CInvGame::mWindiwName = L"Invaders Game";

  bool CInvGame::Initialize()
  {

    //------ Non-graphics initialization -------------------------------------------------------------

    auto randSeed = mSettings.GetSeed();
    if( randSeed < 0 )
      randSeed = (int32_t)time( NULL );
    CInvRandom::GetInstance().SetSeed( (uint32_t)randSeed );

    mHiscoreKeeper = std::make_unique<CInvHiscoreList>( mSettings.GetHiscorePath() );

    //------ Graphics initialization - system --------------------------------------------------------

    mWindowClass = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
                      mWindiwClassId.c_str(), NULL };
    RegisterClassEx( &mWindowClass );

    RECT r = { 0, 0, (LONG)mSettings.GetWidth(), (LONG)mSettings.GetHeight() };
    int style = mSettings.GetFullScreen() ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    style |= WS_VISIBLE;
    AdjustWindowRect( &r, style, false );

    mHWnd = CreateWindow( mWindiwClassId.c_str(), mWindiwName.c_str(),
      style, 0, 0, r.right - r.left, r.bottom - r.top,
      GetDesktopWindow(), NULL, mWindowClass.hInstance, NULL );

    if( !SUCCEEDED( InitD3D() ) )
      return false;

    // Create the vertex buffer
    if( !SUCCEEDED( InitVB() ) )
      return false;

    //SetWindowPos(hWnd,NULL,0,0,1024,768,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE|SWP_ASYNCWINDOWPOS);
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );

    // Show the window
    ShowWindow( mHWnd, SW_SHOWDEFAULT );
    UpdateWindow( mHWnd );

    //------ Graphics initialization - custom --------------------------------------------------------

    mPrimitives = std::make_unique<CInvPrimitive>( mSettings, mPd3dDevice );

    mSpriteStorage = std::make_unique<CInvSpriteStorage>( mSettings, mPd3dDevice );

    mBackgroundInsertCoin = std::make_unique<CInvBackground>( mSettings, mPd3dDevice );

    mBackgroundPlay = std::make_unique<CInvBackground>( mSettings, mPd3dDevice );

    //------ Graphics initialization - sprites ----------------------------------------------------------

    mSpriteStorage->AddSprite( "PINK", "invaderPink" );
    mSpriteStorage->AddSprite( "PINKEXPL", "explosionPink" );
    mSpriteStorage->AddSprite( "SPIT", "spit" );
    mSpriteStorage->AddSprite( "SAUCER", "saucer" );
    mSpriteStorage->AddSprite( "SAUCEREXPL", "explosionSaucer" );
    mSpriteStorage->AddSprite( "PACVADER", "pacvader" );
    mSpriteStorage->AddSprite( "PACVADEREXPL", "explosionPacvader" );

    mSpriteStorage->AddSprite( "FIGHT", "fighter" );
    mSpriteStorage->AddSprite( "LIVE", "fighter" );
    mSpriteStorage->AddSprite( "FIGHTEXPL", "explosionFighter" );
    mSpriteStorage->AddSprite( "ROCKET", "rocket" );
    mSpriteStorage->AddSprite( "AMMO", "rocketAmmo" );

    mBackgroundInsertCoin->AddBackgroundImage( "background/nebula.jpg" );
    mBackgroundInsertCoin->SetRollCoefficient( 0.0f );
                        // Static background

    mBackgroundPlay->AddBackgroundImage( "background/staryline.jpg" );
                        // Dynamic background, default rolling coefficient left

    //------ Main structures initialization ----------------------------------------------------------

    mInsertCoinScreen = std::make_unique<CInvInsertCoinScreen>(
      mSettings,
      *mSpriteStorage,
      *mBackgroundInsertCoin,
      *mHiscoreKeeper,
      *mPrimitives,
      mPD3D,
      mPd3dDevice,
      mPVB,
      mReferenceTick );

    mPlayItScreen = std::make_unique<CInvPlayItScreen>(
      mSettings,
      *mSpriteStorage,
      *mBackgroundPlay,
      *mPrimitives,
      mSettingsRuntime,
      mPD3D,
      mPd3dDevice,
      mPVB,
      mReferenceTick );


    return true;

  } // CInvGame::Initialize

  //-------------------------------------------------------------------------------------------------

  bool CInvGame::Run()
  {
    if( nullptr == mPD3D || nullptr == mPd3dDevice || nullptr == mPVB )
    {
      LOG << "DirectX is not initialized properly.";
      return false;
    } // if

    if( nullptr == mPrimitives )
    {
      LOG << "Primitives drawing device is not initialized properly.";
      return false;
    } // if

    if( nullptr == mInsertCoinScreen )
    {
      LOG << "Insert coin screen is not initialized properly.";
      return false;
    } // if

    if( nullptr == mPlayItScreen )
    {
      LOG << "Plqy screen is not initialized properly.";
      return false;
    } // if

    bool stillInLoop = true;
    uint32_t newScoreToEnter = 0;
    bool gameStartRequest = false;
    bool gameEndRequest = false;
    bool gameInProgress = false;

    ControlStateFlags_t controlState = 0;
    ControlValue_t controlValue = 0;

    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    mReferenceTick.QuadPart = 0;
    mInsertCoinScreen->Reset( mReferenceTick );

    while( stillInLoop )
    {
      QueryPerformanceCounter( &StartingTime );
      QueryPerformanceFrequency( &Frequency );

      MSG msg;
      while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
      {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        if( msg.message == WM_QUIT )
          stillInLoop = false;
      } // while

      if( IsKeyDown( VK_ESCAPE ) )
        stillInLoop = false;

      ProcessInput( controlState, controlValue );

      // Clear the backbuffer to a background color
      mPd3dDevice->Clear( 0, nullptr,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        mClearColor, 1.0f,  0 );

      // Begin the scene
      if( SUCCEEDED( mPd3dDevice->BeginScene() ) )
      {

        gameStartRequest = false;
        gameEndRequest = false;

        if( ! gameInProgress )
        {               // If no game is in progress, shows "insert coin" screen. It may either print
                        // high scores list or enter callsign for new high score, if there is a highscore
                        // value pending from previous game.
          if( !mInsertCoinScreen->MainLoop(
            newScoreToEnter, gameStartRequest, controlState, controlValue, mReferenceTick ) )
          {
            LOG << "Insert coin screen loop failed, quitting";
            stillInLoop = false;
          } // if
        } // if

        if( gameStartRequest && ! gameInProgress )
        {               // New game requested, initialization of hte game engine is necessary
          LOG << "Game start requested";

          mPlayItScreen->Reset( mReferenceTick );
          gameInProgress = true;
          gameStartRequest = false;
        } // if

        if( gameInProgress )
        {               // Game is in progress, engine is invoked here
          if( !mPlayItScreen->MainLoop(
            newScoreToEnter, gameEndRequest, controlState, controlValue, mReferenceTick ) )
          {
            LOG << "Play the game screen loop failed, quitting";
            stillInLoop = false;
          } // if
        } // if

        if( gameEndRequest && gameInProgress )
        {
          LOG << "Game ended";
          mInsertCoinScreen->Reset( mReferenceTick );
          gameInProgress = false;
          gameEndRequest = false;
        } // if

        // End the scene
        mPd3dDevice->EndScene();

      } // if


      mPd3dDevice->Present( NULL, NULL, NULL, NULL );
                        // Present the backbuffer contents to the display

      QueryPerformanceCounter( &EndingTime );
      ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
      ElapsedMicroseconds.QuadPart *= 1000000;
      ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
                        // We now have the elapsed number of ticks, along with the number
                        // of ticks-per-second. We use these values to convert to the number
                        // of elapsed microseconds. To guard against loss-of-precision, we
                        // convert to microseconds before dividing by ticks-per-second.

      auto ElapsedMilliseconds = (uint32_t)( ElapsedMicroseconds.QuadPart / 1000 );
      if( ElapsedMilliseconds < mMillisecondsPerTick )
        Sleep( mMillisecondsPerTick - ElapsedMilliseconds );
                        // Now we calculate how many millisecond left to demanded tick length
                        // and sleep this time if necessary.

      if( gameInProgress )
      {                 // Performance statistics are being collected only in game
                        // mode, not in insert-coin mode.
        if( mLoopElapsedMicrosecondsMax < ElapsedMicroseconds.QuadPart )
          mLoopElapsedMicrosecondsMax = ElapsedMicroseconds.QuadPart;

        if( IsZero( mLoopElapsedMicrosecondsAvg ) )
          mLoopElapsedMicrosecondsAvg = (float)ElapsedMicroseconds.QuadPart;
        else
          mLoopElapsedMicrosecondsAvg =
            ( mLoopElapsedMicrosecondsAvg * mLoopElapsedMicrosecondsAvgCount +
            (float)ElapsedMicroseconds.QuadPart ) / ( mLoopElapsedMicrosecondsAvgCount + 1 );

        if( ElapsedMilliseconds < mMillisecondsPerTick )
          mLoopWaitedMicrosecondsAvg =
            ( mLoopWaitedMicrosecondsAvg * mLoopElapsedMicrosecondsAvgCount +
            (float)( mMillisecondsPerTick - ElapsedMilliseconds ) ) / ( mLoopElapsedMicrosecondsAvgCount + 1 );

        ++mLoopElapsedMicrosecondsAvgCount;

      } // if

      mReferenceTick.QuadPart++;

    } // while

    return true;
  } // CInvGame::Run

  //-------------------------------------------------------------------------------------------------

  bool CInvGame::Cleanup()
  {
    LOG;
    LOG << "Maximal loop time: " << mLoopElapsedMicrosecondsMax << " us";
    LOG << "Average loop time: " << mLoopElapsedMicrosecondsAvg << " us";
    LOG << "Demanded tick time: " << mMillisecondsPerTick * 1000 << " us";
    LOG << "Average wait time: " << mLoopWaitedMicrosecondsAvg * 1000 << " us";

    return true;
  } // CInvGame::Cleanup

  //-------------------------------------------------------------------------------------------------

  HRESULT CInvGame::InitD3D()
  {
    // Create the D3D object.
    if( nullptr == ( mPD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
      return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = !mSettings.GetFullScreen();
    d3dpp.SwapEffect = mSettings.GetFullScreen() ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_DISCARD;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    d3dpp.BackBufferFormat = mSettings.GetFullScreen() ? D3DFMT_A8R8G8B8 : D3DFMT_UNKNOWN;
    d3dpp.BackBufferWidth = 800;
    d3dpp.BackBufferHeight = 600;
    d3dpp.FullScreen_RefreshRateInHz = mSettings.GetFullScreen() ? 60 : 0;
    d3dpp.EnableAutoDepthStencil = TRUE;                   // <-- Auto depth/stencil
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Create the D3DDevice
    if( FAILED( mPD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mHWnd,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mPd3dDevice ) ) )
      return E_FAIL;

    // Device state would normally be set here
    mPd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    mPd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    mPd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    //mPd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );        // zapne z-buffer
    mPd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );          // povolí zápis do Z bufferu
    //mPd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );     // typ porovnání (LESS, LEQUAL, ...)


    mPd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    mPd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    mPd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    mPd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    mPd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

    mPd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    mPd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    mPd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

    mPd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
    //mPd3dDevice->SetTexture(0,NULL);
    mPd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    mPd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

    return S_OK;
  } // CInvGame::InitD3D

  //-------------------------------------------------------------------------------------------------

  HRESULT CInvGame::InitVB()
  {
    if( nullptr == mPd3dDevice )
      return E_FAIL;

    if( FAILED( mPd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ),
                D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX,
                D3DPOOL_DEFAULT, &mPVB, NULL ) ) )
      return E_FAIL;

    return S_OK;
  } // CInvGame::InitVB

  //-------------------------------------------------------------------------------------------------

  bool CInvGame::IsKeyDown( int key )
  {
    return lKeyDown[key & 255];
  } // CInvGame::IsKeyDown

  //-------------------------------------------------------------------------------------------------

  void CInvGame::ProcessInput( ControlStateFlags_t & controlState, ControlValue_t & controlValue )
  {
    controlState = 0;
    if( IsKeyDown( 'A' ) || IsKeyDown( 'a' ) || IsKeyDown( VK_LEFT ) )
      controlState |= ControlState_t::kLeft;
    if( IsKeyDown( 'D' ) || IsKeyDown( 'd' ) || IsKeyDown( VK_RIGHT ) )
      controlState |= ControlState_t::kRight;
    if( IsKeyDown( 'W' ) || IsKeyDown( 'a' ) || IsKeyDown( VK_UP ) )
      controlState |= ControlState_t::kUp;
    if( IsKeyDown( 'S' ) || IsKeyDown( 'a' ) || IsKeyDown( VK_DOWN ) )
      controlState |= ControlState_t::kDown;
    if( IsKeyDown( 'C' ) || IsKeyDown( 'c' ) )
      controlState |= ControlState_t::kSpecial;
    if( IsKeyDown( VK_SPACE ) )
      controlState |= ControlState_t::kFire;
    if( IsKeyDown( VK_RETURN ) )
      controlState |= ControlState_t::kStart;

    controlValue = 0;
    for( int i = 0; i < 256; ++i )
    {
      if( lKeyDown[i] )
      {
        controlValue = i;
        break;
      } // if
    } // for

  } // CInvGame::ProcessInput


} // namespace Inv
