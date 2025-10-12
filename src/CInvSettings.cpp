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
     mSeed( -1 ),
     mTickPerSecond( 60 ),
     mFullScreen( true ),
     mScreenWidth( 800 ),
     mScreenHeight( 600 ),
     mImagePath(),
     mHiscorePath( "./hiscore.csv" ),
     mZeroExplosionV( false ),
     mSpeedupPerKill( 0.05f ),
     mDifficultyBuildup( 1.1f )
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

       mSeed = (int32_t)inCfg.GetValueInteger( {}, "Seed", -1 );
       mTickPerSecond = (uint32_t)inCfg.GetValueInteger( {}, "TickPerSecond", 60 );

       mFullScreen = inCfg.GetValueBool( "graphics", "fullscreen", false );

       mScreenWidth = (uint32_t)inCfg.GetValueInteger( "graphics", "width", 800 );
       mScreenHeight = (uint32_t)inCfg.GetValueInteger( "graphics", "height", 600 );

       mImagePath = inCfg.GetValueStr( "graphics", "images", "./images" );

       mHiscorePath = inCfg.GetValueStr( "game", "highscore", "./hiscore.csv" );
       mZeroExplosionV = inCfg.GetValueBool( "game", "ZeroExplosionV", false );
       mSpeedupPerKill = (float)inCfg.GetValueDouble( "game", "SpeedupPerKill", 0.05f );
       mDifficultyBuildup = (float)inCfg.GetValueDouble( "game", "DifficultyBuildup", 1.1f );

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
     PrpLine() << "Seed:" << mSeed;
     PrpLine() << "TickPerSecond:" << mTickPerSecond;
     LOG;

     PrpLine() << "FullScreen:" << ( mFullScreen ? gTrueName : gFalseName );
     PrpLine() << "ScreenWidth:" << mScreenWidth;
     PrpLine() << "ScreenHeight:" << mScreenHeight;
     LOG;

     PrpLine() << "ImagePath:" << mImagePath;
     PrpLine() << "HighscorePath:" << mHiscorePath;
     PrpLine() << "ZeroExplosionV:" << ( mZeroExplosionV ? gTrueName : gFalseName );
     PrpLine() << "SpeedupPerKill:" << mSpeedupPerKill;
     PrpLine() << "DifficultyBuildup:" << mDifficultyBuildup;
     LOG;

   } // CInvSettings::Preprint

} // namespace Inv
