//****************************************************************************************************
//! \file CInvLoggger.h
//! Module contains class CInvLoggger, which implements singleton pattern for global logging
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_CInvLoggger
#define H_CInvLoggger

#include <InvGlobals.h>

#define LOG Inv::CInvLoggger::GetInstance().GetStream() << std::endl << "[" << lModLogId << "] "

namespace Inv
{

  /*! \brief The class implements singleton pattern for global logging. The class provides a single
      instance that can be accessed globally, and provides a stream for logging messages to a file. */
  class CInvLoggger
  {
    public:

    CInvLoggger( const CInvLoggger & ) = delete;
    CInvLoggger & operator=( const CInvLoggger & ) = delete;
    ~CInvLoggger();

    static CInvLoggger & GetInstance();

    std::ofstream & GetStream() { return mLog; }
    //!< Returns reference to global log output stream

    bool IsOpen() const { return mLog.is_open(); }
    //!< Returns true if log file is opened

    template<typename T>
    std::ofstream & operator<<( const T & item )
    {
      return mLog << item;
    } // operator<<

  private:

    CInvLoggger( const std::string & fnam );

    std::ofstream mLog;
    //<! Output file stream for logging

  };

} // namespace Inv

#endif
