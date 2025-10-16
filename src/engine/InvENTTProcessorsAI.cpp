
//****************************************************************************************************
//! \file InvENTTProcessorsAI.h
//! Module contains definitions of EnTT processors concernig alien behavior
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/InvENTTProcessorsAI.h>
#include <engine/InvENTTComponents.h>

#include <graphics/CInvSprite.h>
#include <CInvSettings.h>
#include <CInvLogger.h>
#include <CInvSettingsRuntime.h>

namespace Inv
{

  static const std::string lModLogId( "PROCAI" );

  //****** processor: setting of actors to specific states *******************************************

  procSpecialActorSpawner::procSpecialActorSpawner(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    CInvEntityFactory & entityFactory,
    uint32_t & aliensLeft,
    std::map<uint32_t, AlienBossDescriptor_t> & bossDescriptor ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mEntityFactory( entityFactory ),
    mAliensLeft( aliensLeft ),
    mBossDescriptor( bossDescriptor )
  {}


  //--------------------------------------------------------------------------------------------------

  void procSpecialActorSpawner::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    float playerYPos )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    for( auto & bossIt : mBossDescriptor )
    {
      auto & boss = bossIt.second;

      if( boss.mMaxSpawned <= boss.mIsSpawned )
        continue;       // Maximum number of this alien boss type is already present in the scene

      auto probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
      if( probRoll < boss.mSpawnProbability * mSettingsRuntime.mSceneLevelMultiplicator )
      {                   // Alien boss (like flying saucer) is spawned on random event
        bool fromLeft = ( ( rand() % 2 ) == 0 );
        mEntityFactory.AddAlienBossEntity(
          boss, fromLeft, IsNegative( boss.mSpawnY ) ? playerYPos : boss.mSpawnY,
          fromLeft ? 1.0f : -1.0f, 0.0f, boss.mSize );

        ++boss.mIsSpawned;
        ++mAliensLeft;  // One more alien of given type is present in the scene

        LOG << "Boss alien of type " << boss.mSpriteId << " spawned, numbers are now "
            << boss.mIsSpawned << "/" << boss.mMaxSpawned;

      } // if
    }
  } // procSpecialActorSpawner::update

  //****** processor: setting of actors to specific states *******************************************

  procActorStateSelector::procActorStateSelector(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    bool & isInDangerousArea ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mIsInDangerousArea( isInDangerousArea )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorStateSelector::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    uint32_t quickDeathTicksLeft )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view<const cpAlienBehave, cpAlienStatus, cpGraphics>();
    view.each( [&]( const cpAlienBehave & behave, cpAlienStatus & status, cpGraphics & gph )
    {                   // Updating status for alien actors

      if( status.isDying )
        return;         // Alien is dying, no other status is possible

      if( !( status.isAnimating || status.isFiring ) )
      {                 // Previous status must be resolved before any other is set

        auto probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
        if( probRoll < behave.animationProbability )
        {
          status.isAnimating = true;
          gph.diffTick.QuadPart = 0ul;
          gph.standardAnimationEffect->Restore();
                        // Animation is started on random event. Effect is restored, runs once (as it is not
                        // continuous) and then suspends itself, sending event message by appropriate callback,
                        // which sets isAnimating flag to false again.
        } // if
        else
        {
          probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
          if( probRoll < ( status.isInRaid ? behave.raidShootProbability : behave.shootProbability ) )
          {
            status.isFiring = true;
            gph.diffTick.QuadPart = 0ul;
            gph.specificAnimationEffect->Restore();
                        // Fire animation is started on random event. Effect is restored, runs once (as it is not
                        // continuous) and then suspends itself, sending event message by appropriate callback,
                        // which sets isFiring flag to false again.
          } // if
        } // else
      } // if

      if( !( status.isInRaid || status.isReturningToFormation ) )
      {
        float prob = behave.raidProbability * 2.0f * mSettingsRuntime.mAlienSpeedupFactor;
        if( mIsInDangerousArea )
          prob *= mSettingsRuntime.mDangerAreaThreatCoefficient;
                        // Invaders are more probable to enter raid if player is in dangerous area
                        // (above the alien formation).

        float probRoll;
        if( 0u == quickDeathTicksLeft )
          probRoll = 0.0f;
        else
          probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
                        // On quick deat mode all aliens are raiding

        if( probRoll < prob )
        {             // Alien enters raid mode on random event.
          status.isInRaid = true;
          status.raidTicksLeft = UINT32_MAX;
        } // if
      } // if

    } );

  } // procActorStateSelector::update

  //****** processor: bounds guard - aliens ************************************************


  procAlienBoundsGuard::procAlienBoundsGuard(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    float & vXGroup,
    float & vYGroup,
    float sceneTopLeftX,
    float sceneTopLeftY,
    float sceneBottomRightX,
    float sceneBottomRightY ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mVXGroup( vXGroup ),
    mVYGroup( vYGroup ),
    mYGroupTranslationCounter( 0 ),
    mTranslatingDown( false ),
    mNextVXGroup( 0.0f ),
    mSceneTopLeftX( sceneTopLeftX ),
    mSceneTopLeftY( sceneTopLeftY ),
    mSceneBottomRightX( sceneBottomRightX ),
    mSceneBottomRightY( sceneBottomRightY )
  {}

  //--------------------------------------------------------------------------------------------------

  void procAlienBoundsGuard::reset(
    LARGE_INTEGER refTick,
    float sceneTopLeftX,
    float sceneTopLeftY,
    float sceneBottomRightX,
    float sceneBottomRightY )
  {
    procEnTTBase::reset( refTick );
    mSceneTopLeftX = sceneTopLeftX;
    mSceneTopLeftY = sceneTopLeftY;
    mSceneBottomRightX = sceneBottomRightX;
    mSceneBottomRightY = sceneBottomRightY;
  } // procAlienBoundsGuard::reset

  //--------------------------------------------------------------------------------------------------

  void procAlienBoundsGuard::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    float bottomGuardedArea )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    bool xChangeNeeded = false;
    bool yAtTheBottom = false;

    auto view = reg.view<cpAlienBehave, cpAlienStatus, cpPosition, cpGeometry>();
    view.each( [&]( cpAlienBehave & pBehave, cpAlienStatus & pStat, cpPosition & pPos, cpGeometry & pGeo )
    {
      if( pStat.isDying )
        return;         // Alien is dying, no group bounds guard possible or necessary

      float xPosNext = pStat.formationX + mVXGroup * mSettingsRuntime.mAlienSpeedupFactor;
      xChangeNeeded |=
        ( xPosNext - 0.5 * pGeo.width < mSceneTopLeftX ) ||
        ( mSceneBottomRightX < xPosNext + 0.5 * pGeo.width );

      float yPosNext = pStat.formationY + mVYGroup * mSettingsRuntime.mAlienSpeedupFactor;
      yAtTheBottom |=
        ( yPosNext - 0.5 * pGeo.height < mSceneTopLeftY ) ||
        ( mSceneBottomRightY - bottomGuardedArea < yPosNext + 0.5 * pGeo.height );
                        // Coo-eee, do not check pos.X and pos.Y there, use formation positions!!! While
                        // the lowest alien is on raid, rest of the formation could drop too low and after
                        // the return of the raider there would be not enough space for player ship at the
                        // bottom of the screen!
    } );

    if( !IsZero( mVXGroup ) )
      mNextVXGroup = -mVXGroup;

    if( xChangeNeeded )
    {
      if( yAtTheBottom )
      {                 // If aliens are at the bottom of the scene, they just change horizontal direction
        mVXGroup = mNextVXGroup;
        mTranslatingDown = false;
        mVYGroup = 0.0f;
      } // if
      else
      {                 // Aliens stops change horizontal direction and starts to move down
        mVXGroup = 0.0f;
        mYGroupTranslationCounter = (uint32_t)( mSettingsRuntime.mAlienDescendTime * (float)mSettings.GetTickPerSecond() );
        mTranslatingDown = true;
      } // else
    } // if

    if( mTranslatingDown )
    {
      if( !yAtTheBottom && 0 < mYGroupTranslationCounter )
      {                 // Aliens are moving down
        --mYGroupTranslationCounter;
        mVYGroup = mSettingsRuntime.mAlienVelocity / mSettings.GetTickPerSecond();
      }
      else
      {                 // Aliens finished moving down, continue horizontal movement in opposite direction
        mVYGroup = 0.0f;
        mVXGroup = mNextVXGroup;
        mTranslatingDown = false;
      } // else
    } // if

  } // procAlienBoundsGuard::update

    //****** processor: setting of actors to specific states *******************************************

  procAlienRaidDriver::procAlienRaidDriver(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime ):

    procEnTTBase( refTick, settings, settingsRuntime )
  {}


  //--------------------------------------------------------------------------------------------------

  void procAlienRaidDriver::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    uint32_t quickDeathTicksLeft )
  {

    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    bool isPlayerDead = true;
    float actPlayerX = 0.5f * mSettings.GetWidth();
    float actPlayerY = 0.5f * mSettings.GetHeight();

    float maxAlienTurningAngle =
      mSettingsRuntime.mAlienRaidMaxAnglePerSec / mSettings.GetTickPerSecond();

    auto viewP = reg.view<cpPlayBehave, cpPlayStatus, cpPosition, cpGraphics>();
    viewP.each( [&]( cpPlayBehave & pBehave, cpPlayStatus & pStat, cpPosition & pPos, cpGraphics & pGph )
    {
        isPlayerDead = pStat.isDying;
        actPlayerX = pPos.X;
        actPlayerY = pPos.Y;
    } );

    auto viewA = reg.view<cpAlienBehave, cpAlienStatus, cpPosition, cpVelocity, cpGeometry>();
    viewA.each( [&]( cpAlienBehave & pBehave, cpAlienStatus & pStat, cpPosition & pPos, cpVelocity &pVel, cpGeometry & pGeo )
    {
        if( !( pStat.isInRaid || pStat.isReturningToFormation ) || pStat.isDying )
          return;       // Alien is not in raid or is dying, it does not concern this processor

        if( isPlayerDead || 0u == pStat.raidTicksLeft )
        {               // Player is dead or time is up => alien returns to formation
          pStat.isInRaid = false;
          pStat.isReturningToFormation = true;
        } // if

        float xTgt = pStat.isReturningToFormation ? pStat.formationX : actPlayerX;
        float yTgt = pStat.isReturningToFormation ? pStat.formationY : actPlayerY;
                        // Target position is either player position (in raid) or formation
                        // position (returning to formation)

        float deltaX = xTgt - pPos.X;
        float deltaY = yTgt - pPos.Y;
                        // Vector from actual alien position to target position, basis
                        // for the pursuit curve.

        float distanceToTarget = sqrt( deltaX * deltaX + deltaY * deltaY );

        if( 0u < quickDeathTicksLeft )
        {               // On quick death mode all aliens gone for raind and never returns to formation
          if( pStat.isInRaid &&
            ( distanceToTarget < mSettingsRuntime.mRaidTgtDistance || 0u == pStat.raidTicksLeft ) )
          {               // Alien reached target distance in raid, returns to formation
            pStat.isInRaid = false;
            pStat.isReturningToFormation = true;
            pStat.raidTicksLeft = 0u;
          } // if
        } // if

        if( pStat.isReturningToFormation && distanceToTarget < mSettingsRuntime.mReturnTgtDistance )
        {               // Alien reached target distance when returning to formation,
                        // it is back in formation
          pStat.isInRaid = false;
          pStat.isReturningToFormation = false;
          pStat.raidTicksLeft = 0u;
          pPos.X = pStat.formationX;
          pPos.Y = pStat.formationY;
          pVel.vX = 0.0f;
          pVel.vY = 0.0f;
          return;
        } // if

        float velocitySize =
          mSettingsRuntime.mAlienRaidVelocity / mSettings.GetTickPerSecond();
                        // Alien velocity magnitude (per tick). Do not multiply it by
                        // mSettingsRuntime.mAlienSpeedupFactor, it is done in procActorMover
                        // processor.

        if( UINT32_MAX == pStat.raidTicksLeft )
        {               // Raid just started, movement must be initiated
          pStat.raidTicksLeft =
            (uint32_t)( mSettingsRuntime.mAlienRaidMaxTime * mSettings.GetTickPerSecond() );
          pVel.vX = velocitySize * deltaX / distanceToTarget;
          pVel.vY = velocitySize * deltaY / distanceToTarget;
          return;
        } // if

        float phi = atan2( pVel.vX * deltaY - pVel.vY * deltaX, pVel.vX * deltaX + pVel.vY * deltaY );
                        // Oriented angle between current velocity vector and vector to target is
                        // calculated according to definition of scalar and pseudoskálární product
                        // of two vectors.

        if( phi < -maxAlienTurningAngle )
          phi = -maxAlienTurningAngle;
        else if( maxAlienTurningAngle < phi )
          phi = maxAlienTurningAngle;
                        // Angle change is limited by maximum turning angle per tick

        float cosPhi = cos( phi );
        float sinPhi = sin( phi );
                        // Cosine and sine of limited angle change is calculated

        float newVX = cosPhi * pVel.vX - sinPhi * pVel.vY;
        float newVY = sinPhi * pVel.vX + cosPhi * pVel.vY;
        pVel.vX = newVX;
        pVel.vY = newVY;// New velocity vector is calculated by rotation of old vector
                        // by limited angle change

        if( 0u < pStat.raidTicksLeft )
          --pStat.raidTicksLeft;
                        // One tick in raid mode is consumed

    });



  } // procAlienRaidDriver::update


  //--------------------------------------------------------------------------------------------------

} // namespace Inv
