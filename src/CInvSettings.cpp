//****************************************************************************************************
//! \file CInvSettings.cpp                                                                       
//! Module contains definitions of CInvSettings class, declared in CInvSettings.h, which
//! contains all parameters entered  by the user from outside.                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <iomanip>

#include <CInvSettings.h>
#include <InvStringTools.h>
#include <CInvLogger.h>

namespace Inv
{

  static const std::string lModLogId( "SETTINGS" );

   //****** CCInvSettings **************************************************************************

   CInvSettings::CInvSettings():
     mGameIdentifier( "Invaders"),
     mFullScreen( true ),
     mImagePath()
   {

   } // CInvSettings::CInvSettings 

   //-------------------------------------------------------------------------------------------------

   CInvSettings::~CInvSettings() = default;

   //-------------------------------------------------------------------------------------------------

   std::vector<std::string> CInvSettings::ImportSettings( const Inv::CInvConfig& inCfg )
   {
     std::vector<std::string> vErrors;

     try
     {

       mGameIdentifier = inCfg.GetValueStr( {}, "Identifier", "" );
       if( mGameIdentifier.empty() )
       {
         vErrors.emplace_back( "Identifier value not found" );
       } // if
       else
       {
         for( auto& ch : mGameIdentifier )
         {
           if( !IsNormalChar( ch ) )
           {
             vErrors.emplace_back( "Identifier contains unsupported characters" );
             break;
           } // if
         } // for
       } // else

       mFullScreen = inCfg.GetValueBool( "graphics", "fullscreen", false );
       mImagePath = inCfg.GetValueStr( "graphics", "images", "./images" );
      
     }
     catch( std::exception& e )
     {
       vErrors.emplace_back( e.what() );
     } // catch
     catch( ... )
     {
       vErrors.emplace_back( "Unknown exception during settings import" );
     } // catch

     return vErrors;
   } // ImportSettings

   //-------------------------------------------------------------------------------------------------

   std::ostream & CInvSettings::PrpLine()
   {
     return LOG << std::setw(Inv::gPrintoutIdWidth) << std::left;
   } // HlpLine

   //-------------------------------------------------------------------------------------------------

   void CInvSettings::Preprint()
   { 
     PrpLine() << "GameIdentifier:" << mGameIdentifier;
     LOG;

     PrpLine() << "FullScreen:" << ( mFullScreen ? gTrueName : gFalseName );
     PrpLine() << "ImagePath:" << mImagePath;
     LOG;

   } // CInvSettings::Preprint

} // namespace Inv
