//****************************************************************************************************
//! \file CInvSettingsRuntime.h
//! Module contains declaration of CInvSettingsRuntime class, which contains all parameters created
//! and updated by game engine
//****************************************************************************************************
//
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvSettingsRuntime
#define H_CInvSettingsRuntime

#include <iostream>

namespace Inv
{

  //***** CInvSettingsRuntime *****************************************************************************

  /*! \brief This class contains all parameters created and updated by game engine.  */
  class CInvSettingsRuntime
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators
    //@{----------------------------------------------------------------------------------------------

    CInvSettingsRuntime();

    virtual ~CInvSettingsRuntime();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods  */
   //@{-----------------------------------------------------------------------------------------------

    void Preprint();
    /*! \brief Sends all SettingsRuntime to given output stream

        \param[in,out] out Output stream, default is std::cout */

    void ResetToDefaults();
    /*!< \brief Resets all settings to default values */

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data concerning game difficulty
    //@{----------------------------------------------------------------------------------------------

    uint32_t mSceneLevel;
    //!< Current game level, starting from 1. Affects game difficulty.

    float mSceneLevelMultiplicator;

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data concerning aliens
    //@{----------------------------------------------------------------------------------------------

    float mAlienAnimationProbability;
    //!< Speed of basic animation, usually a small positive number

    float mAlienShootProbability;
    //!< Probability of shooting in each game tick, usually a small. Affects game difficulty.

    float mAlienRaidProbability;

    float mAlienVelocity;
    //!< Speed of aliens in pixels per second

    float mAlienRaidShootProbability;

    float mAlienRaidVelocity;
    //!< Speed of aliens during raid in pixels per second

    float mAlienDescendTime;
    //!<Time period for alien group descend (when hit th scene edge)

    float mAlienSpeedupFactor;
    //!< Factor by which alien speed is increased

    float mSaucerProbability;
    //!< Probability of saucer appearance in each game tick, usually a small. Affects game

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data concerning player
    //@{----------------------------------------------------------------------------------------------

    uint32_t mPlayerInvulnerabilityTicks;
    //!< Number of ticks the player is invulnerable after being spawned

    float mPlayerVelocity;
    //!< Speed of the player

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data concerning missiles
    //@{----------------------------------------------------------------------------------------------

    float mSpitVelocity;
    //!< Speed of standard missiles (alien) in pixels per second

    float mRocketVelocity;
    //!< Speed of standard missiles (player) in pixels per second

    uint32_t mRocketSupply;
    //!< Number of rockets (player missiles) available to player. No more rockets can be fired when
    //!  supply is zero.

    float mRocketSupplyReplenishRate;
    //!< Rate at which rocket supply is replenished - one rocket is added to supply every
    //!  given number of seconds, until maximum supply is reached.

    uint32_t mRocketSupplyReplenishTicks;
    //!< Rate at which rocket supply is replenished - one rocket is added to supply every
    //!  given number of ticks, until maximum supply is reached. This value is calculated
    //!  from mRocketSupplyReplenishRate and CInvSettings::mTickPerSecond.

  private:

    std::ostream & PrpLine();
    /*!< \brief Helper for formatting output in Preprint method */


    //@}

  }; // CInvSettingsRuntime

} // namespace Inv

#endif
