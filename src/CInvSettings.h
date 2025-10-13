//****************************************************************************************************
//! \file CInvSettings.h
//! Module contains declaration of CInvSettings class, which contains all parameters entered
//! by the user from outside.
//****************************************************************************************************
//
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvSettings
#define H_CInvSettings

#include <iostream>

#include <CInvConfig.h>

namespace Inv
{

  //***** CInvSettings *****************************************************************************

  /*! \brief This class contains all parameters entered by the user from outside. They are given and
      constant, they cannot be changed during program execution. Values are imported through means of
      CInvConfig class. */
  class CInvSettings
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators
    //@{----------------------------------------------------------------------------------------------

    CInvSettings();

    virtual ~CInvSettings();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods  */
   //@{-----------------------------------------------------------------------------------------------

    std::vector<std::string> ImportSettings( const Inv::CInvConfig & inCfg );
    /*! \brief Imports settings from configuration object

        \param[in] inCfg Configuration object containing all parameters
        \return List of error messages. If empty, import was successful. */

    void Preprint();
    /*! \brief Sends all settings to given output stream

        \param[in,out] out Output stream, default is std::cout */

    //@{}---------------------------------------------------------------------------------------------
    //! @name Input data getters
    //@{----------------------------------------------------------------------------------------------

    const std::string & GetGameIdentifier() const { return mGameIdentifier; }
    //!< \brief Returns game identifier

    int32_t GetSeed() const { return mSeed; }
    //!< \brief Returns seed for random number generator, if -1, current time is used

    uint32_t GetTickPerSecond() const { return mTickPerSecond; }
    //!< \brief Returns number of ticks per second (updates of game logic and rendering)

    bool GetFullScreen() const { return mFullScreen; }
    //!< \brief Returns true if the game should run in fullscreen mode

    uint32_t GetWidth() const { return mScreenWidth; }
    //!< \brief Returns screen width in pixels

    uint32_t GetHeight() const { return mScreenHeight; }
    //!< \brief Returns screen height in pixels

    const std::string & GetImagePath() const { return mImagePath; }
    //!< \brief Returns path to image files

    const std::string & GetHiscorePath() const { return mHiscorePath; }
    //!< \brief Returns path to hiscore file

    bool GetZeroExplosionV() const { return mZeroExplosionV; }
    //!< \brief Returns true if actor should stop moving when destroyed

    float GetSpeedupPerKill() const { return mSpeedupPerKill; }
    //!< \brief Returns factor by which alien speed is increased after each killed alien

    float GetDifficultyBuildup() const { return mDifficultyBuildup; }
    //!< \brief Returns internal parameter, used to increase game difficulty with each level

    uint32_t GetInitialLives() const { return mInitialLives; }
    //!< \brief Returns number of player lives (ships) at the beginning of the game

    uint32_t GetAmmo() const { return mAmmo; }
    //!< \brief Returns number of player shots available (max rockets in the scene)

    float GetReloadTime() const { return mReloadTime; }
    //!< \brief Returns time (in seconds) to reload one rocket

  protected:

    //@}----------------------------------------------------------------------------------------------
    //! @name Protected input data
    //@{----------------------------------------------------------------------------------------------

    std::string mGameIdentifier;
                        //!< Game identifier

    int32_t mSeed;      //!< Seed for random number generator, if -1, current time is used

    uint32_t mTickPerSecond;
                        //!< Number of ticks per second (updates of game logic and rendering)

    bool mFullScreen;   //!< If true, game runs in fullscreen mode

    uint32_t mScreenWidth;
                        //!< Screen width in pixels
    uint32_t mScreenHeight;
                        //!< Screen height in pixels

    std::string mImagePath;
                        //!< Path to image files

    std::string mHiscorePath;
                        //!< Path to hiscore file

    bool mZeroExplosionV;
                        //!< If true, actor stops moving when destroyed

    float mSpeedupPerKill;
                        //!< Factor by which alien speed is increased after each killed alien

    float mDifficultyBuildup;
    //!< Internal parameter, used to increase game difficulty with each level

    uint32_t mInitialLives;
    //!< \brief Number of player lives (ships) at the beginning of the game

    uint32_t mAmmo;
    //!< \brief Number of player shots available (max rockets in the scene)

    float mReloadTime;
    //!< \brief Time (in seconds) to reload one rocket

    std::ostream & PrpLine();
    /*!< \brief Helper for formatting output in Preprint method */


    //@}

  }; // CInvSettings

} // namespace Inv

#endif
