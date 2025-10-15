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
     mMinScore( 100 ),
     mRaidScoreCoef( 5.0f ),
     mZeroExplosionV( false ),
     mSpeedupPerKill( 0.05f ),
     mDifficultyBuildup( 1.1f ),
     mQuickDeathTime( 60.0f ),
     mInitialLives( 3 ),
     mAmmo( 3 ),
     mReloadTime( 1.0f )
   {}

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

       mFullScreen = inCfg.GetValueBool( "graphics", "FullScreen", false );

       mScreenWidth = (uint32_t)inCfg.GetValueInteger( "graphics", "Width", 800 );
       mScreenHeight = (uint32_t)inCfg.GetValueInteger( "graphics", "Height", 600 );

       mImagePath = inCfg.GetValueStr( "graphics", "Images", "./images" );

       mHiscorePath = inCfg.GetValueStr( "game", "HighScore", "./hiscore.csv" );
       mMinScore = (uint32_t)inCfg.GetValueInteger( "game", "MinScore", 100 );
       mRaidScoreCoef = (float)inCfg.GetValueDouble( "game", "RaidScoreCoef", 5.0f );
       mZeroExplosionV = inCfg.GetValueBool( "game", "ZeroExplosionV", false );
       mSpeedupPerKill = (float)inCfg.GetValueDouble( "game", "SpeedupPerKill", 0.05f );
       mDifficultyBuildup = (float)inCfg.GetValueDouble( "game", "DifficultyBuildup", 1.1f );
       mQuickDeathTime = (float)inCfg.GetValueDouble( "game", "QuickDeathTime", 60.0f );

       mInitialLives = (uint32_t)inCfg.GetValueInteger( "player", "InitialLives", 3 );
       mAmmo = (uint32_t)inCfg.GetValueInteger( "player", "Ammo", 3 );
       mReloadTime = (float)inCfg.GetValueDouble( "player", "ReloadTime", 1.0f );

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
     PrpLine() << "MinScore:" << mMinScore;
     PrpLine() << "RaidScoreCoef:" << mRaidScoreCoef;
     PrpLine() << "ZeroExplosionV:" << ( mZeroExplosionV ? gTrueName : gFalseName );
     PrpLine() << "SpeedupPerKill:" << mSpeedupPerKill;
     PrpLine() << "DifficultyBuildup:" << mDifficultyBuildup;
     PrpLine() << "QuickDeathTime:" << mQuickDeathTime;
     LOG;

     PrpLine() << "InitialLives:" << mInitialLives;
     PrpLine() << "Ammo:" << mAmmo;
     PrpLine() << "ReloadTime:" << mReloadTime;
     LOG;

   } // CInvSettings::Preprint

} // namespace Inv
