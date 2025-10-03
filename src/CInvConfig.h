//****************************************************************************************************
//! \file CInvConfig.h                                                                         
//! Module contains declaration of CInvConfig class, which handles application configuration 
//! parsing.                                
//****************************************************************************************************
//*                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                   
//****************************************************************************************************

#ifndef H_CInvConfig
#define H_CInvConfig

#include <fstream>
#include <regex>

#include <InvGlobals.h>

namespace Inv
{

  //***** CInvConfig *****************************************************************************

	/*!  \brief This class, which handles application configuration parsing (commandline and INI file). */
  class CInvConfig
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CInvConfig();

    virtual ~CInvConfig();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods 
   //@{-----------------------------------------------------------------------------------------------

    void Preprint();
    /*! \brief Sends all settings to given output stream
     
        \param[in,out] out Output stream, default is std::cout */

    bool ParseCommandLine( int argc, char * const argv[] );
    /*! \brief Parses command line arguments
    
        \param[in] argc Number of command line arguments
        \param[in] argv Array of command line argument strings
				\return true if parsing was successful, false otherwise */

    bool ParseINIFile( std::ifstream & in, size_t &lastLineRead );
    /*! \brief Parses INI file content from given input stream
    
        \param[in]  in           Input stream containing INI file content
				\param[out] lastLineRead Number of last line read from input stream*/

    std::string GetValueStr(
      const std::string & inSect,
      const std::string & inItem,
      const std::string & inDefault = {} ) const;
    /*!< \brief Returns string value of given item in given section.
         If item or section does not exist, returns default value.

         \param[in] inSect    Section name (empty string for global section)
         \param[in] inItem    Item name
         \param[in] inDefault Default value to return if item or section does not exist
				 \return Value of given item in given section or default value */

    bool GetValueBool(
      const std::string & inSect,
      const std::string & inItem,
      bool  inDefault = false ) const;
    /*!< \brief Returns boolean value of given item in given section.
         If item or section does not exist, returns default value.

         \param[in] inSect    Section name (empty string for global section)
         \param[in] inItem    Item name
				 \param[in] inDefault Default value to return if item or section does not exist */

    double_t GetValueDouble(
      const std::string & inSect,
      const std::string & inItem,
      double_t  inDefault = 0.0 ) const;
    /*!< \brief Returns double value of given item in given section.
         If item or section does not exist, returns default value.

         \param[in] inSect    Section name (empty string for global section)
				 \param[in] inItem    Item name */

    int64_t GetValueInteger(
      const std::string & inSect,
      const std::string & inItem,
      int64_t  inDefault = 0 ) const;
    /*!< \brief Returns integer value of given item in given section.
         If item or section does not exist, returns default value.

         \param[in] inSect    Section name (empty string for global section)
				 \param[in] inItem    Item name */

    uint64_t GetValueUnsigned(
      const std::string & inSect,
      const std::string & inItem,
      uint64_t  inDefault = 0 ) const;
    /*!< \brief Returns unsigned integer value of given item in given section.
         If item or section does not exist, returns default value
         .
				 \param[in] inSect    Section name (empty string for global section) */

  protected:

    //@{}---------------------------------------------------------------------------------------------
    //! @name Protected data                                                                         
    //@{----------------------------------------------------------------------------------------------

    //! Value of configuration item
    using CfgItem_t = struct
    {
      StrVect_t Value;
    };

    using CfgItemList_t = StrMap_t<CfgItem_t>;
		//!< List of configuration items in one section

    StrMap_t<CfgItemList_t> mCfgContent;
		//!< Whole configuration content (map of sections, each section is map of items)

    const char * mSeparators;
    const char * mSeparatorsSpace;
    const char * mGlues;

    std::regex mRegexINILineEmptyCommented;
    //!< INI file content: regex for empty line with comment

    std::regex mRegexLineEmpty;
    //!< regex for empty variable value or ini file line without comment

    std::regex mRegexINISectionCommented;
    //!< INI file content: regex for section definition with comment

    std::regex mRegexINISection;
    //!< INI file content: regex for section definition without comment

    //@{}---------------------------------------------------------------------------------------------
    //! @name Protected methods
    //@{----------------------------------------------------------------------------------------------

    bool ParseINIKeyValuePair(
      const std::string & inLine, const std::string & actSection );
    /*!< \brief Parses one line of INI file containing key=value pair and inserts 
         it into configuration content.

         \param[in] inLine      Line to be parsed
         \param[in] actSection  Name of actual section (empty string for global section)
				 \return true if parsing was successful, false otherwise */

    bool InsertItem(
      const std::string & in_ai8Sect,
      const std::string & in_ai8Item,
      const StrVect_t & in_ai8ItemVal );
    /*!< \brief Inserts given item into configuration content.
    
         \param[in] in_ai8Sect     Section name (empty string for global section)
         \param[in] in_ai8Item     Item name
				 \param[in] in_ai8ItemVal  Item value */
 
    //@}

  }; // CInvConfig

} // namespace Inv

#endif
