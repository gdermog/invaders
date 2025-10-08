//****************************************************************************************************
//! \file CInvPlayItScreen.h
//! Module defines class CInvPlayItScreen, which implements the "Play It" screen, where the actual
//! game is played.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvPlayItScreen
#define H_CInvPlayItScreen

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvSpriteStorage.h>
#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>

#include <engine/CInvGameScene.h>

namespace Inv
{
  /*! \brief Class implements the "Play It" screen, where the actual game is played.

      The class manages the game state, including the current score, and handles user input
      to control the game. It uses other graphics classes to render the game elements on screen.
      The class provides a main loop method that runs the game until it ends, either by user
      quitting or by game over condition. */
  class CInvPlayItScreen
  {
  public:

    CInvPlayItScreen(
      const CInvSettings & settings,
      const CInvText & textCreator,
      const CInvSpriteStorage & spriteStorage,
      CInvPrimitive & primitives,
      LPDIRECT3D9 pD3D,
      LPDIRECT3DDEVICE9 pd3dDevice,
      LPDIRECT3DVERTEXBUFFER9 pVB,
      LARGE_INTEGER tickReferencePoint );

    CInvPlayItScreen( const CInvPlayItScreen & ) = delete;
    CInvPlayItScreen & operator=( const CInvPlayItScreen & ) = delete;
    ~CInvPlayItScreen();

    bool MainLoop(
      uint32_t & newScoreToEnter,
      bool & gameEnd,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue,
      LARGE_INTEGER actualTickPoint );
    /*!< \brief Main loop of the "Play It" screen, runs until game ends or user quits.

         \param[out] newScoreToEnter  This parameter will contain the new score to be entered into
                                      high score list after game ends.
         \param[out] gameEnd          Will be set to true if game ended (either by user quitting
                                      or by game over condition).
         \param[in] controlState      Current state of user controls, combination of ControlStateFlags_t
                                      values.
         \param[in] controlValue      Value of key pressed
         \param[in] actualTickPoint   Current tick count, used for timing and animations and gameplay.
         \return Returns true if screen was drawn successfully, false if there was an error. */

    void Reset( LARGE_INTEGER newTickRefPoint );
    /*!< \brief Resets the game state to initial conditions, ready for a new game.

         \param[in] newTickRefPoint New reference tick point, usually current time */

  private:


    LARGE_INTEGER mTickReferencePoint;
    //!< \brief Reference tick point, used to calculate elapsed time during the game.

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings

    const CInvText & mTextCreator;
    //!< \brief Reference to text creator, used to draw text on screen

    const CInvSpriteStorage & mSpriteStorage;
    //!< \brief Reference to sprite storage object, used to access sprites.

    CInvPrimitive & mPrimitives;
    //!< \brief Reference to primitive drawer, used to draw basic shapes on screen

    CInvGameScene mGameScene;
    //!< \brief Game scene object, used to manage the game entities and logic.

    LPDIRECT3D9             mPD3D;
    //<! Direct3D interface, used to create device

    LPDIRECT3DDEVICE9       mPd3dDevice;
    //<! Direct3D device, used to draw on screen

    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //<! Vertex buffer, used to draw primitives

    uint32_t mActualScore;
    //<! Current score of the player

  };

} // namespace Inv

#endif
