//****************************************************************************************************
//! \file CInvSettingsRuntime.h
//! Module contains declaration of CInvSettingsRuntime class, which contains all parameters created and
//! updated by game engine
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
    //! @name Public data concerning aliens
    //@{----------------------------------------------------------------------------------------------

    float mAlienAnimationProbability;
    //!< Speed of basic animation, usually a small positive number

    float mAlienShootProbability;
    //!< Probability of shooting in each game tick, usually a small. Affects game difficulty.

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

    float mMissileVelocity;
    //!< Speed of standard missiles (both alien and player)

  private:

    std::ostream & PrpLine();
    /*!< \brief Helper for formatting output in Preprint method */


    //@}

  }; // CInvSettingsRuntime

} // namespace Inv

#endif
