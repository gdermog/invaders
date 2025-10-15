
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
#include <engine/CInvEntityFactory.h>
#include <CInvSettings.h>
#include <CInvSettingsRuntime.h>

namespace Inv
{

  //****** processor: setting of actors to specific states *******************************************

  procSpecialActorSpawner::procSpecialActorSpawner(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    CInvEntityFactory & entityFactory,
    uint32_t & aliensLeft,
    float saucerSize,
    float saucerSpawnPointY,
    float saucerSpawnPointXLeft,
    float saucerSpawnPointXRight ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mEntityFactory( entityFactory ),
    mAliensLeft( aliensLeft ),
    mSaucerSize( saucerSize ),
    mSaucerSpawnPointY( saucerSpawnPointY ),
    mSaucerSpawnPointXLeft( saucerSpawnPointXLeft ),
    mSaucerSpawnPointXRight( saucerSpawnPointXRight )
  {}

  //--------------------------------------------------------------------------------------------------

  void procSpecialActorSpawner::reset(
    LARGE_INTEGER refTick,
    float saucerSize,
    float saucerSpawnPointY,
    float saucerSpawnPointXLeft,
    float saucerSpawnPointXRight )
  {
    procEnTTBase::reset( refTick );
    mSaucerSize = saucerSize;
    mSaucerSpawnPointY = saucerSpawnPointY;
    mSaucerSpawnPointXLeft = saucerSpawnPointXLeft;
    mSaucerSpawnPointXRight = saucerSpawnPointXRight;
  } // procSpecialActorSpawner::reset

  //--------------------------------------------------------------------------------------------------

  void procSpecialActorSpawner::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
    if( probRoll < mSettingsRuntime.mSaucerProbability * mSettingsRuntime.mSceneLevelMultiplicator )
    {                   // Alien boss (flying saucer) is spawned on random event
      bool fromLeft = ( ( rand() % 2 ) == 0 );
      mEntityFactory.AddAlienBossEntity(
      "SAUCER",
      fromLeft ? mSaucerSpawnPointXLeft : mSaucerSpawnPointXRight,
      mSaucerSpawnPointY,
      fromLeft ? 1.0 : -1.0 , 0.0f,
      mSaucerSize );
      ++mAliensLeft;    // One more alien is present in the scene
    } // if

  } // procSpecialActorSpawner::update

  //****** processor: setting of actors to specific states *******************************************

  procActorStateSelector::procActorStateSelector(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime ):

    procEnTTBase( refTick, settings, settingsRuntime )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorStateSelector::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view<const cpAlienBehave, cpAlienStatus, cpGraphics>();
    view.each( [=]( const cpAlienBehave & behave, cpAlienStatus & status, cpGraphics &gph )
    {                   // Updating status for alien actors

        if( status.isDying )
          return;       // Alien is dying, no other status is possible

        if( !( status.isAnimating || status.isFiring ) )
        {               // Previous status must be resolved before any other is set

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
//           auto probRoll = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
//           if( probRoll < behave.raidProbability )
//             status.isInRaid = true;
//                         // Alien enters raid mode on random event.
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
        if( pStat.isInRaid || pStat.isReturningToFormation || pStat.isDying )
          return;       // Alien is in raid or dying, no group bounds guard possible or necessary

        float xPosNext = pPos.X + mVXGroup * mSettingsRuntime.mAlienSpeedupFactor;
        xChangeNeeded |=
          ( xPosNext - 0.5 * pGeo.width < mSceneTopLeftX ) ||
          ( mSceneBottomRightX < xPosNext + 0.5 * pGeo.width );

        float yPosNext = pPos.Y + mVYGroup * mSettingsRuntime.mAlienSpeedupFactor;
        yAtTheBottom |=
          ( yPosNext - 0.5 * pGeo.height < mSceneTopLeftY ) ||
          ( mSceneBottomRightY - bottomGuardedArea < yPosNext + 0.5 * pGeo.height );

    } );

    if( ! IsZero( mVXGroup ) )
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
      if( ! yAtTheBottom && 0 < mYGroupTranslationCounter )
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


} // namespace Inv
