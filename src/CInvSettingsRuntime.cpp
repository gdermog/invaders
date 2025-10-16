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
    mSceneLevel( 1 ),
    mSceneLevelMultiplicator( 1.0f ),
    mAlienAnimationProbability( 0.0f ),
    mAlienShootProbability( 0.f ),
    mAlienRaidProbability( 0.0f),
    mAlienVelocity( 0.0f ),
    mAlienRaidShootProbability( 0.0f ),
    mAlienRaidVelocity( 0.0f ),
    mPlayerInvulnerabilityTicks( 1 ),
    mPlayerVelocity( 0.0f ),
    mAlienDescendTime( 0.f ),
    mAlienSpeedupFactor( 1.0f ),
    mAlienRaidMaxTime( 0.0f ),
    mAlienRaidMaxAnglePerSec( 0.0f ),
    mRaidTgtDistance( 0.0f ),
    mReturnTgtDistance( 0.0f ),
    mDangerAreaThreatCoefficient( 1.0f ),
    mSpitVelocity( 0.0f ),
    mRocketVelocity( 0.0f ),
    mRocketSupply( 0 ),
    mRocketSupplyReplenishRate( 0.0f ),
    mRocketSupplyReplenishTicks( 1 )
  {
    ResetToDefaults();
  } // CInvSettingsRuntime::CInvSettingsRuntime

  //-------------------------------------------------------------------------------------------------

  CInvSettingsRuntime::~CInvSettingsRuntime() = default;

  //-------------------------------------------------------------------------------------------------

  std::ostream & CInvSettingsRuntime::PrpLine()
  {
    return LOG << std::setw( Inv::gPrintoutIdWidth ) << std::left;
  } // HlpLine

  //-------------------------------------------------------------------------------------------------

  void CInvSettingsRuntime::Preprint()
  {

    LOG;
    PrpLine() << "------ DIFFICULTY PARAMETERS ----------------------";
    PrpLine() << "Level:" << mSceneLevel;
    PrpLine() << "LevelMultiplicator:" << mSceneLevelMultiplicator;

    PrpLine() << "------ ALIEN PARAMETERS ---------------------------";
    PrpLine() << "AnimationProb:" << mAlienAnimationProbability;
    PrpLine() << "ShootProb:" << mAlienShootProbability;
    PrpLine() << "RaidProb:" << mAlienRaidProbability;
    PrpLine() << "Velocity:" << mAlienVelocity;
    PrpLine() << "RaidShootProb:" << mAlienRaidShootProbability;
    PrpLine() << "RaidVelocity:" << mAlienRaidVelocity;
    PrpLine() << "DescendTime:" << mAlienDescendTime;
    PrpLine() << "SpeedupFactor:" << mAlienSpeedupFactor;
    PrpLine() << "RaidMaxTime:" << mAlienRaidMaxTime;
    PrpLine() << "RaidMaxAnglePerSec:" << mAlienRaidMaxAnglePerSec;
    PrpLine() << "RaidTgtDistance:" << mRaidTgtDistance;
    PrpLine() << "ReturnTgtDistance:" << mReturnTgtDistance;
    PrpLine() << "DangerAreaThreatCoef:" << mDangerAreaThreatCoefficient;
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
    mSceneLevel = 1u;
    mSceneLevelMultiplicator = 1.0f;

    mAlienAnimationProbability = 0.0025f;
    mAlienShootProbability = 0.0003f;
    mAlienRaidProbability = 0.00001f;
    mAlienVelocity = 37.5f;
    mAlienRaidShootProbability = 0.075f;
    mAlienRaidVelocity = 55.0f;
    mAlienDescendTime = 0.3f;
    mAlienSpeedupFactor = 1.0f;
    mAlienRaidMaxTime = 4.0f;
    mAlienRaidMaxAnglePerSec = gPI;
    mRaidTgtDistance = 10.0f;
    mReturnTgtDistance = 10.0f;
    mDangerAreaThreatCoefficient = 15.0f;

    mPlayerInvulnerabilityTicks = 150;
    mPlayerVelocity = 150.0f;

    mSpitVelocity = 75.0f;
    mRocketVelocity = 225.0f;
    mRocketSupplyReplenishRate = 1.0f;
    mRocketSupplyReplenishTicks = 60;

  } // CInvSettingsRuntime::ResetToDefaults

} // namespace Inv
