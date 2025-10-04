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

		bool GetFullScreen() const { return mFullScreen; }
		//!< \brief Returns true if the game should run in fullscreen mode

    uint32_t GetWindowWidth() const { return mScreenWidth; }
    //!< \brief Returns screen width in pixels

    uint32_t GetWindowHeight() const { return mScreenHeight; }
    //!< \brief Returns screen height in pixels

    const std::string & GetImagePath() const { return mImagePath; }
    //!< \brief Returns path to image files

    const std::string & GetHiscorePath() const { return mHiscorePath; }
    //!< \brief Returns path to hiscore file

  protected:

    //@}----------------------------------------------------------------------------------------------
    //! @name Protected input data                                                                            
    //@{----------------------------------------------------------------------------------------------

    std::string mGameIdentifier;
                        //!< Game identifier

    bool mFullScreen;   //!< If true, game runs in fullscreen mode
   
    uint32_t mScreenWidth;   
                        //!< Screen width in pixels
    uint32_t mScreenHeight;  
                        //!< Screen height in pixels
    
    std::string mImagePath;
                        //!< Path to image files

    std::string mHiscorePath;
                        //!< Path to hiscore file

    std::ostream & PrpLine();
		/*!< \brief Helper for formatting output in Preprint method */
    

    //@}

  }; // CInvSettings

} // namespace Inv

#endif