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
    mTextCreator( nullptr ),
    mHiscoreKeeper( nullptr ),
    mPrimitives( nullptr ),
    mInsertCoinScreen( nullptr ),
    mPlayItScreen( nullptr ),
    mSettings( settings ),
    mWindowClass{},
    mHWnd{},
    mReferenceTick{},
    mFreq{},
    mPD3D( nullptr ),
    mPd3dDevice( nullptr ),
    mPVB( nullptr ),
    mClearColor( D3DCOLOR_XRGB( 0, 0, 0 ) )
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
    srand( randSeed );

    mHiscoreKeeper = std::make_unique<CInvHiscoreList>( mSettings.GetHiscorePath() );

    //------ Graphics initialization - system --------------------------------------------------------

    mWindowClass = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
                      mWindiwClassId.c_str(), NULL };
    RegisterClassEx( &mWindowClass );

    RECT r = { 0, 0, mSettings.GetWindowWidth(), mSettings.GetWindowHeight() };
    int style = mSettings.GetFullScreen() ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    style |= WS_VISIBLE;
    AdjustWindowRect( &r, style, false );

    mHWnd = CreateWindow( mWindiwClassId.c_str(), mWindiwName.c_str(),
      style, 0, 0, r.right - r.left, r.bottom - r.top,
      GetDesktopWindow(), NULL, mWindowClass.hInstance, NULL );

    QueryPerformanceCounter( &mReferenceTick );
    QueryPerformanceFrequency( &mFreq );

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

    mTextCreator = std::make_unique<CInvText>( mSettings, mPd3dDevice, "/letters" );

    mPrimitives = std::make_unique<CInvPrimitive>( mSettings, mPd3dDevice );

    mSpriteStorage = std::make_unique<CInvSpriteStorage>( mSettings, mPd3dDevice );

    //------ Graphics initialization - sprites ----------------------------------------------------------

    mSpriteStorage->AddSprite( "PINK", "invader_pink" );
    mSpriteStorage->AddSprite( "SPIT", "spit" );

    mSpriteStorage->AddSprite( "FIGHT", "fighter" );
    mSpriteStorage->AddSprite( "FEXPL", "explosionFighter" );

    //------ Main structures initialization ----------------------------------------------------------

    mInsertCoinScreen = std::make_unique<CInvInsertCoinScreen>(
      mSettings,
      *mTextCreator,
      *mSpriteStorage,
      *mHiscoreKeeper,
      *mPrimitives,
      mPD3D,
      mPd3dDevice,
      mPVB,
      mReferenceTick );

    mPlayItScreen = std::make_unique<CInvPlayItScreen>(
      mSettings,
      *mTextCreator,
      *mSpriteStorage,
      *mPrimitives,
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

    if( nullptr == mTextCreator )
    {
      LOG << "Text creator is not initialized properly.";
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

    QueryPerformanceCounter( &mReferenceTick );
    mInsertCoinScreen->Reset( mReferenceTick );

    while( stillInLoop )
    {
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

          QueryPerformanceCounter( &mReferenceTick );
          mPlayItScreen->Reset( mReferenceTick );
          gameInProgress = true;
          gameStartRequest = false;
        } // if

        if( gameInProgress )
        {               // Game is in progress, engine is invoked here
          if( !mPlayItScreen->MainLoop(
            newScoreToEnter, gameEndRequest, controlState, controlValue, mReferenceTick ) )
          {
            LOG << "Insert coin screen loop failed, quitting";
            stillInLoop = false;
          } // if
        } // if

        if( gameEndRequest && gameInProgress )
        {
          LOG << "Game ended";
          QueryPerformanceCounter( &mReferenceTick );
          mInsertCoinScreen->Reset( mReferenceTick );
          gameInProgress = false;
          gameEndRequest = false;
        } // if

        // End the scene
        mPd3dDevice->EndScene();

      } // if

      // Present the backbuffer contents to the display
      mPd3dDevice->Present( NULL, NULL, NULL, NULL );

      Sleep( 10 );
      mReferenceTick.QuadPart++;

    } // while

    return true;
  } // CInvGame::Run

  //-------------------------------------------------------------------------------------------------

  bool CInvGame::Cleanup()
  {
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
