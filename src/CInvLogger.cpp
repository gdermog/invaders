//****************************************************************************************************
//! \file CInvLoggger.cpp
//! Module contains class CInvLoggger, which implements singleton pattern for global logging
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <CInvLogger.h>

namespace Inv
{
  CInvLoggger::CInvLoggger( const std::string & fnam )
  {
    mLog.open( fnam, std::ofstream::out );
  } // CInvLoggger::CInvLoggger

  //-------------------------------------------------------------------------------------------------

  CInvLoggger::~CInvLoggger()
  {
    if( mLog.is_open() )
      mLog.close();
  } // CInvLoggger::~CInvLoggger

  //-------------------------------------------------------------------------------------------------

  CInvLoggger & CInvLoggger::GetInstance()
  {
    static Inv::CInvLoggger instance( "invaders.log" );
    return instance;
  } // CInvLoggger::GetInstance

} // namespace Inv
