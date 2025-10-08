//****************************************************************************************************
//! \file CInvInsertCoinScreen.h
//! Module declares class CInvInsertCoinScreen, which implements implements the "Insert Coin" screen.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvInsertCoinScreen
#define H_CInvInsertCoinScreen

#include <d3d9.h>
//#include <d3dx9.h>

#include <InvGlobals.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvText.h>
#include <graphics/CInvPrimitive.h>

#include <engine/CInvHiscoreList.h>

namespace Inv
{

  /*! \brief The class implements the "Insert Coin" screen, which is the initial screen of the game.
      The screen displays a welcome message, a prompt to "insert coin" (to press enter key), and a
      list of high scores. If the player has qualified for a high score, he is prompted to enter his
      callsign. The class manages the state of the screen, including the current callsign being
      entered, and handles user input to control the screen. It uses other graphics classes to render
      the screen elements. The class provides a main loop method that draws whole "Insert Coin" screen
      each tick until the player starts the game or exits. */
  class CInvInsertCoinScreen
  {
    public:

      CInvInsertCoinScreen(
        const CInvSettings & settings,
        const CInvText & textCreator,
        CInvHiscoreList & hiscoreKeeper,
        CInvPrimitive & primitives,
        LPDIRECT3D9 pD3D,
        LPDIRECT3DDEVICE9 pd3dDevice,
        LPDIRECT3DVERTEXBUFFER9 pVB,
        LARGE_INTEGER tickReferncePoint );

    CInvInsertCoinScreen( const CInvInsertCoinScreen & ) = delete;
    CInvInsertCoinScreen & operator=( const CInvInsertCoinScreen & ) = delete;
    ~CInvInsertCoinScreen();

    bool MainLoop(
      uint32_t & newScoreToEnter,
      bool &gameStart,
      ControlStateFlags_t controlState,
      ControlValue_t controlValue,
      LARGE_INTEGER actualTick );
    /*!< \brief Main loop of the "Insert Coin" screen, runs until player starts the game or exits.

         \param[out] newScoreToEnter  This parameter will contain the new score to be entered into
                                      high score list after game ends.
         \param[out] gameStart        Will be set to true if player started the game by pressing
                                      enter key.
         \param[in] controlState      Current state of user controls, combination of ControlStateFlags_t
                                      values.
         \param[in] controlValue      Value of key pressed
         \param[in] actualTick        Current tick count, used for timing and animations.
         \return Returns true if screen was drawn successfully, false if there was an error. */

    void Reset( LARGE_INTEGER newTickRefPoint );
    /*!< \brief Resets the screen state to initial conditions, ready for a new player. */

  private:

    void FormatHighScore( uint32_t inScore, const std::string & inName );
    /*!< \brief Formats given score and name into mHighScoreLineContent, which is then drawn
         on screen.

          \param[in] inScore Score to be included in formatted string
          \param[in] inName  Callsign to be included in formatted string */

    bool DrawHighScores( LARGE_INTEGER actualTick );
    /*!< \brief Draws high score area, including background rectangle and all high score lines.
         The area is drawn with a rolling effect, where lines move up and new lines appear at
         the bottom. The rolling is done in steps, where each step moves the area by a fraction
         of line height. The number of visible lines and the speed of rolling are configurable.

         \param[in] actualTick Current tick count, used for timing and animations.
         \return Returns true if area was drawn successfully, false if there was an error. */

    bool DrawEnterCallsign(
      LARGE_INTEGER actualTick,
      ControlValue_t controlValue,
      bool & isDone );
    /*!< \brief Draws the "Enter Callsign" prompt and the current callsign being entered.
         Handles user input to modify the callsign, including adding letters, deleting letters,
         and finishing the entry.

         \param[in] actualTick   Current tick count, used for timing and animations.
         \param[in] controlValue Value of key pressed, used to modify the callsign.
         \param[out] isDone      Will be set to true if the player has finished entering the
                                 callsign by pressing enter key.
         \return Returns true if area was drawn successfully, false if there was an error. */

    static const std::string mWelcomeHeader;
    //!< "Welcome to invaders" header text
    static const std::string mPressToStart;
    //!< "Press ENTER to start" prompt text
    static const std::string mYouQualified;
    //!< "You qualified for high score!" prompt text
    static const std::string mEnterCallsign;
    //!< "Enter your callsign:" prompt text

    static const uint32_t mHighScoreAreaVisibleLines;
    //!< Number of high score lines visible at once
    static const uint32_t mHighScoreRollingStepPerLine;
    //!< Number of rolling steps per single high score line height
    static const float mHighScoreAreaFontRelativeSize;
    //!< Relative size of font used in high score area, relative to mLetterSize
    static const float mPressEnterFontRelativeSize;
    //!< Relative size of font used in "Press ENTER to start" prompt, relative to mLetterSize

    LARGE_INTEGER mTickReferencePoint;
    //!< \brief Reference tick point, used to calculate elapsed time during animations.
    LARGE_INTEGER mDiffTick;
    //!< \brief Artificially introduced correction to the current tick, used when one effect is
    //!  applied to multiple objects and the results are required to differ somewhat from each
    //!  other. This value is "dummy", it is actually not used, only passed into methods that
    //!  require it.

    float mLetterSize;
    //!< \brief Base letter size in pixels, used for main header and other texts.
    float mHeaderXPos;
    //!< \brief X position of main header, calculated during construction.
    float mHeaderYPos;
    //!< \brief Y position of main header, calculated during construction.

    float mHighScoreLetterSize;
    //!< \brief Letter size in pixels used in high score area, calculated during construction.
    float mHighScoreTopLeftX;
    //!< \brief X coordinate of top left corner of high score area, calculated during construction.
    float mHighScoreTopLeftY;
    //!< \brief Y coordinate of top left corner of high score area, calculated during construction.
    float mHighScoreBottomRightX;
    //!< \brief X coordinate of bottom right corner of high score area, calculated during construction.
    float mHighScoreBottomRightY;
    //!< \brief Y coordinate of bottom right corner of high score area, calculated during construction.
    float mHighScoreWidth;
    //!< \brief Width of high score area, calculated during construction.
    float mHighScoreHeight;
    //!< \brief Height of high score area, calculated during construction.
    std::string mHighScoreLineContent;
    //!< \brief Formatted content of single high score line, used as a buffer to avoid
    //!  repeated allocations during drawing.

    float mPressEnterLetterSize;
    //!< \brief Letter size in pixels used in "Press ENTER to start" prompt, calculated during construction.
    float mPressEnterTopLeftX;
    //!< \brief X coordinate of top left corner of "Press ENTER to start" prompt, calculated during construction.
    float mPressEnterTopLeftY;
    //!< \brief Y coordinate of top left corner of "Press ENTER to start" prompt, calculated during construction.

    float mQualifiedLetterSize;
    //!< \brief Letter size in pixels used in "You qualified for high score!" prompt, calculated during construction.
    float mQualifiedTopLeftX;
    //!< \brief X coordinate of top left corner of "You qualified for high score!" prompt, calculated during construction.
    float mQualifiedTopLeftY;
    //!< \brief Y coordinate of top left corner of "You qualified for high score!" prompt, calculated during construction.
    float mEnterCallsignTopLeftX;
    //!< \brief X coordinate of top left corner of "Enter your callsign:" prompt, calculated during construction.
    float mEnterCallsignTopLeftY;
    //!< \brief Y coordinate of top left corner of "Enter your callsign:" prompt, calculated during construction.

    float mCallsignLetterSize;
    //!< \brief Letter size in pixels used for callsign being entered, calculated during construction.
    float mCallsignTopLeftX;
    //!< \brief X coordinate of top left corner of callsign being entered, calculated during construction.
    float mCallsignTopLeftY;
    //!< \brief Y coordinate of top left corner of callsign being entered, calculated during construction.

    uint64_t mRollState;
    //!< \brief Current state of high score area rolling effect, counts the number of steps taken.
    uint64_t mRollMax;
    //!< \brief Maximum number of rolling steps, calculated from number of visible lines and

    const CInvSettings & mSettings;
    //!< \brief Reference to global settings object, used to access configuration parameters.
    const CInvText & mTextCreator;
    //!< \brief Reference to text rendering object, used to draw text on screen.
    CInvHiscoreList & mHiscoreKeeper;
    //!< \brief Reference to high score list object, used to access and modify high scores.
    CInvPrimitive & mPrimitives;
    //!< \brief Reference to primitive rendering object, used to draw basic shapes on screen.

    std::string mCurrentCallsign;
    //!< \brief Current callsign being entered by player, modified by user input.
    ControlValue_t mLastControlValue;
    //!< \brief Last control value processed, used to avoid repeated processing of same input.

    LPDIRECT3D9             mPD3D;
    //!< \brief Pointer to Direct3D interface, used for graphics operations.
    LPDIRECT3DDEVICE9       mPd3dDevice;
    //!< \brief Pointer to Direct3D device, used for rendering.
    LPDIRECT3DVERTEXBUFFER9 mPVB;
    //!< \brief Pointer to Direct3D vertex buffer, used for rendering primitives.

    std::unique_ptr<CInvSprite> mTitleSprite;
    //!< \brief Sprite object used to display animated title sprites on screen.
    uint32_t mNrOfTitleSprites;
    //!< \brief Number of title sprites to be displayed, calculated during construction.
    uint32_t mTitleSpriteImageSequenceNr;
    //!< \brief Number of images in title sprite animation sequence, calculated during construction.
    float mTitleSpriteWidth;
    //!< \brief Width of each title sprite in pixels, calculated during construction.
    std::vector<float> mTitleSpriteX;
    //!< \brief X coordinates of title sprites, calculated during construction.
    float mTitleSpriteY;
    //!< \brief Y coordinate of title sprites, calculated during construction.

    std::shared_ptr<CInvEffect> mTitleSpriteAnimationEffect;
    //!< \brief Shared pointer to animation effect applied to title sprites.
    std::shared_ptr<CInvEffect> mTitleSpriteShiftRotateEffect;
    //!< \brief Shared pointer to shift and rotate effect applied to title sprites.
  };

} // namespace Inv

#endif
