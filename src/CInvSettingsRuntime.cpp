//****************************************************************************************************
//! \file CInvSettingsRuntime.cpp
//! Module contains definitions of CInvSettingsRuntime class, declared in CInvSettingsRuntime.h, which
//! contains all parameters created and updated by game engine.
//****************************************************************************************************
//
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <iomanip>

#include <CInvSettingsRuntime.h>
#include <CInvLogger.h>

namespace Inv
{

  static const std::string lModLogId( "SettingsRuntime" );

   //****** CCInvSettingsRuntime **************************************************************************

   CInvSettingsRuntime::CInvSettingsRuntime():
     mAlienAnimationProbability( 0.0025f ),
     mAlienShootProbability( 0.001f ),
     mPlayerInvulnerabilityTicks( 150 ),
     mPlayerVelocity( 2.0f ),
     mSpitVelocity( 1.0f ),
     mRocketVelocity( 3.0f ),
     mRocketSupply( 3 ),
     mRocketSupplyReplenishRate( 1.0f ),
     mRocketSupplyReplenishTicks( 60 )
   {

   } // CInvSettingsRuntime::CInvSettingsRuntime

   //-------------------------------------------------------------------------------------------------

   CInvSettingsRuntime::~CInvSettingsRuntime() = default;

   //-------------------------------------------------------------------------------------------------


   //-------------------------------------------------------------------------------------------------

   std::ostream & CInvSettingsRuntime::PrpLine()
   {
     return LOG << std::setw(Inv::gPrintoutIdWidth) << std::left;
   } // HlpLine

   //-------------------------------------------------------------------------------------------------

   void CInvSettingsRuntime::Preprint()
   {
      PrpLine() << "------ ALIEN PARAMETERS ---------------------------";
      PrpLine() << "AnimationProbability:" << mAlienAnimationProbability;
      PrpLine() << "ShootProbability:" << mAlienShootProbability;
      LOG;

      PrpLine() << "------ PLAYER PARAMETERS --------------------------";
      PrpLine() << "InvulnerabilityTicks:" << mPlayerInvulnerabilityTicks;
      PrpLine() << "Velocity:" << mPlayerVelocity;
      LOG;

      PrpLine() << "------ MISSILE PARAMETERS -------------------------";
      PrpLine() << "SpitVelocity:" << mSpitVelocity;
      PrpLine() << "RocketVelocity:" << mRocketVelocity;
      PrpLine() << "RocketSupply:" << mRocketSupply;
      PrpLine() << "RocketSupplyReplenishRate:" << mRocketSupplyReplenishRate;
      PrpLine() << "RocketSupplyReplenishTicks:" << mRocketSupplyReplenishTicks;
      LOG;

   } // CInvSettingsRuntime::Preprint

   //-------------------------------------------------------------------------------------------------

   void CInvSettingsRuntime::ResetToDefaults()
   {
     mAlienAnimationProbability = 0.0025f;
     mAlienShootProbability = 0.001f;

     mPlayerInvulnerabilityTicks = 150;
     mPlayerVelocity = 2.0f;

     mSpitVelocity = 1.0f;
     mRocketVelocity = 3.0f;
     mRocketSupplyReplenishRate = 1.0f;
     mRocketSupplyReplenishTicks = 60;

   } // CInvSettingsRuntime::ResetToDefaults

} // namespace Inv
