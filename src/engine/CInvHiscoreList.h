//****************************************************************************************************
//! \file CInvHiscore.h                                                                          
//! Module contains class CInvHiscore, which implements ...                                                                      
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvHiscore
#define H_CInvHiscore

#include <InvGlobals.h>

namespace Inv
{

  class CInvHiscoreList
  {
    public:

    CInvHiscoreList( const std::string & inFileName );
    CInvHiscoreList( const CInvHiscoreList & ) = delete;
    CInvHiscoreList & operator=( const CInvHiscoreList & ) = delete;
    ~CInvHiscoreList();
    
    void Load();
    /*!< \brief Loads hiscore list from file */

    void Save();
    /*!< \brief Saves hiscore list to file */

    const std::vector<std::pair<uint32_t, std::string>> & GetHiscoreList() const { return mHiscoreList; }
    /*!< \brief Returns reference to hiscore list */

    void AddNewHiscore( uint32_t inScore, const std::string & inName );
    /*!< \brief Adds new hiscore to the list. 

         \param[in] inScore New hiscore value
         \param[in] inName  Name of player achieving the score */

  private:

    constexpr static size_t mMaxHiscoreCount = 100;
    constexpr static size_t mMaxHiscoreNameLen = 6;

    std::vector<std::pair<uint32_t, std::string>> mHiscoreList;
    std::string  mFileName;
  };

} // namespace Inv

#endif
