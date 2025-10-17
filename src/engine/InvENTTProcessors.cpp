//****************************************************************************************************
//! \file InvENTTProcessors.h
//! Module contains basic EnTT processors definitions
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <engine/InvENTTProcessors.h>
#include <engine/InvENTTComponents.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvCollisionTest.h>
#include <engine/CInvEntityFactory.h>
#include <CInvSettings.h>
#include <CInvSettingsRuntime.h>

namespace Inv
{

  static const std::string lModLogId( "PROC" );


  procEnTTBase::procEnTTBase(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime ):
    mSettings( settings ),
    mSettingsRuntime( settingsRuntime ),
    mRefTick( refTick ),
    mIsSuspended( false )
  {}

  //--------------------------------------------------------------------------------------------------

  void procEnTTBase::reset( LARGE_INTEGER refTick )
  {
    mRefTick = refTick;
  } // procEnTTBase::reset


  //****** processor: adding entities on request of special events ***********************************


  procEntitySpawner::procEntitySpawner(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    CInvEntityFactory & entityFactory,
    const CInvSoundsStorage & soundStorage ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mEntityFactory( entityFactory ),
    mSoundStorage( soundStorage )
  {}

  //--------------------------------------------------------------------------------------------------

  void procEntitySpawner::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    float xTopLeft, yTopLeft;
    float xBottomRight, yBottomRight;
    float xSize, ySize;
    size_t imageIndex;

    auto view = reg.view<cpAlienStatus, const cpPosition, const cpGeometry, const cpGraphics>();
    view.each( [&]( cpAlienStatus & stat, const cpPosition & pos, const cpGeometry & geo, const cpGraphics &gph )
    {                   // Check for aliens shooting requests

        if( stat.isDying || ! stat.isShootRequested )
          return;       // Alien did not request to shoot in this tick (or is dead and cannot shoot)

        gph.standardSprite->GetResultingPosition(
          xTopLeft, yTopLeft, xBottomRight, yBottomRight, xSize, ySize, imageIndex );

        mEntityFactory.AddMissileEntity(
          "SPIT", false,
          0.5f * ( xTopLeft + xBottomRight ),
          yBottomRight - 0.15f * ySize,
          0.33f * xSize );
        mSoundStorage.PlaySound( "SPIT" );

        stat.isShootRequested = false;
                        // Shoot request is processed
    } );

  } // procEntitySpawner::update

  //****** processor: updating speed of player actor ************************************************

  procPlayerSpeedUpdater::procPlayerSpeedUpdater(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime ):

    procEnTTBase( refTick, settings, settingsRuntime )
  {}

  //--------------------------------------------------------------------------------------------------

  void procPlayerSpeedUpdater::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    ControlStateFlags_t controlState,
    ControlValue_t controlValue )
  {

    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view<cpPlayBehave, cpPlayStatus, cpVelocity>();
    view.each( [=]( cpPlayBehave & pos, cpPlayStatus & pstat, cpVelocity & vel )
    {
        if( pstat.isDying )
          return;       // Player is dying, no control possible

        vel.vX = 0.0f;
        vel.vY = 0.0f;
        vel.vZ = 0.0f;

        float deltaV = mSettingsRuntime.mPlayerVelocity / mSettings.GetTickPerSecond();

        if( ControlStateHave( controlState, ControlState_t::kLeft  ) )
          vel.vX = -deltaV;

        if( ControlStateHave( controlState, ControlState_t::kRight ) )
          vel.vX = deltaV;

        if( ControlStateHave( controlState, ControlState_t::kUp) )
          vel.vY = -deltaV;

        if( ControlStateHave( controlState, ControlState_t::kDown ) )
          vel.vY = deltaV;

    } );
  } // procPlayerSpeedUpdater::update

    //****** processor: updating demads for offensive actions of player actor ****************


  procPlayerFireUpdater::procPlayerFireUpdater(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    CInvEntityFactory & entityFactory,
    const CInvSoundsStorage & soundStorage,
    uint32_t & ammoLeft ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mEntityFactory( entityFactory ),
    mSoundStorage( soundStorage ),
    mAmmoLeft( ammoLeft ),
    mShootCommenced( false )
  {}


  //--------------------------------------------------------------------------------------------------


  void procPlayerFireUpdater::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    ControlStateFlags_t controlState,
    ControlValue_t controlValue )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    if( ControlStateHave( controlState, ControlState_t::kFire ) )
    {
      if( ! mShootCommenced && 0u < mAmmoLeft )
      {

        float xTopLeft, yTopLeft;
        float xBottomRight, yBottomRight;
        float xSize, ySize;
        size_t imageIndex;

        auto view = reg.view<cpPlayStatus, const cpPosition, const cpGeometry, const cpGraphics>();
        view.each( [&]( cpPlayStatus & stat, const cpPosition & pos, const cpGeometry & geo, const cpGraphics & gph )
        {               // This should work as there is only one player entity. If there are more, all would shoot
                        // simultaneously, which is probably not desired. In such case more complex logic would be
                        // needed.

            if( stat.isDying )
              return;     // Player is dying, cannot shoot

            gph.standardSprite->GetResultingPosition(
              xTopLeft, yTopLeft, xBottomRight, yBottomRight, xSize, ySize, imageIndex );

            mEntityFactory.AddMissileEntity(
              "ROCKET", true,
              0.5f * ( xTopLeft + xBottomRight ),
              yBottomRight - 0.75f * ySize,
              0.1f * xSize,
              0.0f, -1.0f );
            mSoundStorage.PlaySound( "ROCKET" );
        } );

        mAmmoLeft--;
        mShootCommenced = true;
      } // if

    }
    else
      mShootCommenced = false;

  } // procPlayerFireUpdater::update


  //****** processor: bounds guard - player ************************************************


  procPlayerBoundsGuard::procPlayerBoundsGuard(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    float sceneTopLeftX,
    float sceneTopLeftY,
    float sceneBottomRightX,
    float sceneBottomRightY,
    float & playerActX,
    float & playerActY ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mSceneTopLeftX( sceneTopLeftX ),
    mSceneTopLeftY( sceneTopLeftY ),
    mSceneBottomRightX( sceneBottomRightX ),
    mSceneBottomRightY( sceneBottomRightY ),
    mPlayerActX( playerActX ),
    mPlayerActY( playerActY )
  {}

  //--------------------------------------------------------------------------------------------------

  void procPlayerBoundsGuard::reset(
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
  } // procActorMover::reset

  //--------------------------------------------------------------------------------------------------

  void procPlayerBoundsGuard::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view<cpPlayBehave, cpPlayStatus, cpPosition, cpVelocity, cpGeometry>();
    view.each( [=]( cpPlayBehave & pBehave, cpPlayStatus & pStat, cpPosition & pPos, cpVelocity & pVel, cpGeometry &pGeo )
    {
        float xPosNext = pPos.X + pVel.vX;
        if( xPosNext - 0.5*pGeo.width < mSceneTopLeftX )
          pVel.vX = 0.0f;
        else if( mSceneBottomRightX < xPosNext + 0.5*pGeo.width )
          pVel.vX = 0.0f;

        float yPosNext = pPos.Y + pVel.vY;
        if( yPosNext - 0.5 * pGeo.height < mSceneTopLeftY )
          pVel.vY = 0.0f;
        else if( mSceneBottomRightY < yPosNext + 0.5 * pGeo.height )
          pVel.vY = 0.0f;

        mPlayerActX = pPos.X + pVel.vX;
        mPlayerActY = pPos.Y + pVel.vY;

    } );

  } // procPlayerBoundsGuard::update

  //****** processor: moving of actors ************************************************************

  procActorMover::procActorMover(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    float & vXGroup,
    float & vYGroup ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mVXGroup( vXGroup ),
    mVYGroup( vYGroup ),
    mFormationFreeze( false )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorMover::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view<cpPosition, const cpVelocity>();
    view.each( [&]( entt::entity entity, cpPosition & pos, const cpVelocity & vel )
    {
        auto [ aBehave, aStat ] = reg.try_get<cpAlienBehave, cpAlienStatus>( entity );
        if( nullptr != aBehave && nullptr != aStat )
        {               // Alien entity

          if( !mFormationFreeze )
          {             // Position in formation where alien belongs is updated by group velocity.
            aStat->formationX += mVXGroup * mSettingsRuntime.mAlienSpeedupFactor;
            aStat->formationY += mVYGroup * mSettingsRuntime.mAlienSpeedupFactor;
          } // if

          if( aStat->isInRaid || aStat->isReturningToFormation )
          {             // Alien is in raid or returning to formation, moves by its own velocity
            pos.X += vel.vX * mSettingsRuntime.mAlienSpeedupFactor;
            pos.Y += vel.vY * mSettingsRuntime.mAlienSpeedupFactor;
          } // if
          else if( ! mFormationFreeze )
          {             // Alien is in formation, moves by group velocity
            pos.X += mVXGroup * mSettingsRuntime.mAlienSpeedupFactor;
            pos.Y += mVYGroup * mSettingsRuntime.mAlienSpeedupFactor;
          } // else
        } // if
        else
        {
          pos.X += vel.vX;
          pos.Y += vel.vY;
        } // else
    } );

  } // procActorMover::update


  //****** processor: colliding of actors ***************************************************************

  procCollisionDetector::procCollisionDetector(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    CInvCollisionTest & cTest ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mCTest( cTest )
  {}

  //--------------------------------------------------------------------------------------------------

  void procCollisionDetector::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {

    mCollidedPairs.clear();
    mCanDamage.clear();
    mCanBeDamagedAlien.clear();
    mCanBeDamagedPlayer.clear();

    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto viewDmg = reg.view<cpId, cpDamage, cpGraphics>();
    viewDmg.each( [&]( entt::entity entity, const auto & id, const auto & dmg, const auto & gph )
    {
        if( ! id.active || gph.isHidden)
          return;       // Hidden or inactive entity does not deal damage
        mCanDamage.insert( entity );
    } );

    auto viewHealth = reg.view<cpId, cpHealth, cpGraphics>();
    viewHealth.each( [&]( entt::entity entity, const auto & id, const auto & hlt, const auto & gph)
    {
        if( ! id.active || gph.isHidden )
          return;       // Hidden entity cannot be hit

      auto [ bAlien, sAlien ] = reg.try_get<cpAlienBehave, cpAlienStatus>( entity );
      if( nullptr != bAlien && nullptr != sAlien && ! sAlien->isDying )
        mCanBeDamagedAlien.insert( entity );

      auto [bBossAlien, sBossAlien] = reg.try_get<cpAlienBehave, cpAlienBossStatus>( entity );
      if( nullptr != bBossAlien && nullptr != sBossAlien && !sBossAlien->isDying )
        mCanBeDamagedAlien.insert( entity );

      auto [ bPlayer, sPlayer ] = reg.try_get<cpPlayBehave, cpPlayStatus>( entity );
      if( nullptr != bPlayer && nullptr != sPlayer && ! sPlayer->isDying && ! sPlayer->isInvulnerable )
        mCanBeDamagedPlayer.insert( entity );
    } );

    for( auto dangerousEntity : mCanDamage )
    {
      auto [dmgDanger, gphDanger] = reg.try_get<cpDamage, cpGraphics>( dangerousEntity );

      if( nullptr == dmgDanger || nullptr == gphDanger )
        continue;

      if( dmgDanger->dangerToAliens )
      {
        for( auto vulnerableEntity : mCanBeDamagedAlien )
        {
          if( dangerousEntity == vulnerableEntity )
            continue;
          auto [hltVulner, gphVulner] = reg.try_get<cpHealth, cpGraphics>( vulnerableEntity );
          if( nullptr == hltVulner || nullptr == gphVulner )
            continue;
          if( nullptr == gphDanger->standardSprite || nullptr == gphVulner->standardSprite )
            continue;
          if( mCTest.AreInCollision( *(gphDanger->standardSprite), *(gphVulner->standardSprite) ) )
            mCollidedPairs.push_back( { dangerousEntity, vulnerableEntity } );
        } // for
      }

      if( dmgDanger->dangerToPlayer )
      {
        for( auto vulnerableEntity : mCanBeDamagedPlayer )
        {
          if( dangerousEntity == vulnerableEntity )
            continue;
          auto [hltVulner, gphVulner] = reg.try_get<cpHealth, cpGraphics>( vulnerableEntity );
          if( nullptr == hltVulner || nullptr == gphVulner )
            continue;
          if( nullptr == gphDanger->standardSprite || nullptr == gphVulner->standardSprite )
            continue;
          if( mCTest.AreInCollision( *( gphDanger->standardSprite ), *( gphVulner->standardSprite ) ) )
            mCollidedPairs.push_back( { dangerousEntity, vulnerableEntity } );
        } // for
      } // if

    } // for

  } // procCollisionDetector::update


  //****** processor: searching for actor that are out of scene **************************************


  procActorOutOfSceneCheck::procActorOutOfSceneCheck(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    float sceneTopLeftX,
    float sceneTopLeftY,
    float sceneBottomRightX,
    float sceneBottomRightY ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mSceneTopLeftX( sceneTopLeftX ),
    mSceneTopLeftY( sceneTopLeftY ),
    mSceneBottomRightX( sceneBottomRightX ),
    mSceneBottomRightY( sceneBottomRightY )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorOutOfSceneCheck::reset(
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
  } // procActorOutOfSceneCheck::reset

  //--------------------------------------------------------------------------------------------------

  void procActorOutOfSceneCheck::update( entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view<cpId, const cpPosition, const cpGeometry>();
    view.each( [=]( cpId & id, const cpPosition & pos, const cpGeometry & geo )
    {
       if( ( pos.X + 0.5f * geo.width < mSceneTopLeftX )     ||
           ( mSceneBottomRightX < pos.X - 0.5f * geo.width ) ||
           ( pos.Y + 0.5f * geo.height < mSceneTopLeftY )    ||
           ( mSceneBottomRightY < pos.Y - 0.5f * geo.height ) )
       {
         id.active = false;
                        // Entity is out of scene, remove it from registry later
       } // if

    } );
  } // procActorOutOfSceneCheck::update


  //****** processor: garbage collector ***************************************************************


  procGarbageCollector::procGarbageCollector(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    FnEventCallbackEithEntityId_t pruneCallback ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mPruneCallback( pruneCallback )
  {}

  //--------------------------------------------------------------------------------------------------

  void procGarbageCollector::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick,
    bool allowCallbacks )
  {
    mEntities.clear();

    /* No suspended state for garbage collector! */

    auto view = reg.view<cpId>();
    for( auto entity : view )
    {
      auto & entId = view.get<cpId>( entity );
      if( !entId.active )
      {                 // Entity is marked as inactive and it will be remove from registry.
                        // If it should send notification on pruning, it is done now.
        if( allowCallbacks && entId.noticeOnPruning && nullptr != mPruneCallback )
          mPruneCallback( entity, (uint32_t)entId.id );
        mEntities.push_back( entity );
      } // if
    }  // for

    for( auto entity : mEntities )
      reg.destroy( entity );
                        // Remove all entities marked as inactive

  } // procGarbageCollector::update

  //****** processor: rendering of actors ************************************************************

  procActorRender::procActorRender(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime ):

    procEnTTBase( refTick, settings, settingsRuntime )
  {}

  //--------------------------------------------------------------------------------------------------

  void procActorRender::update(
    entt::registry & reg, LARGE_INTEGER actTick, LARGE_INTEGER diffTick )
  {
    for( auto & item : mZAxisSorting )
      item.second.clear();
                        // Clear sorting structure before use (possibly without deleting the memory structures
                        // from previous usage)

    if( mIsSuspended )
      return;           // Processor is suspended, no action is performed

    auto view = reg.view< cpGraphics, const cpPosition, const cpGeometry>();
    view.each( [=]( cpGraphics & gph, const cpPosition & pos, const cpGeometry & geo )
    {
        if( gph.isHidden )
          return;       // Entity is hidden, do not draw it

        mZAxisSorting[gph.standardSprite->GetLevel()].emplace_back( gph.standardSprite, gph, pos, geo );
        gph.diffTick.QuadPart++;
    } );                // Sprite animations are driven by tick count stored in cpGraphics component.
                        // It must not be dependent on global tick counter, because each entity starts
                        // its animations independently at random time.

    for( auto & item : mZAxisSorting )
    {
      for( auto & item2 : item.second )
      {
        item2.sprite->Draw(
          item2.pos.X, item2.pos.Y,
          item2.geo.width, item2.geo.height,
          actTick, actTick, item2.gph.diffTick,
          item2.gph.staticStandardImageIndex );
      } // for
    } // for

  } // procActorRender::update

  //****** processor: check if the player actor is in dangerous area **********************************


  procPlayerInDanger::procPlayerInDanger(
    LARGE_INTEGER refTick,
    const CInvSettings & settings,
    CInvSettingsRuntime & settingsRuntime,
    bool & isInDangerousArea ):

    procEnTTBase( refTick, settings, settingsRuntime ),
    mIsInDangerousArea( isInDangerousArea )
  {}


  //--------------------------------------------------------------------------------------------------

  void procPlayerInDanger::update(
    entt::registry & reg,
    LARGE_INTEGER actTick,
    LARGE_INTEGER diffTick )
  {
    float minAlienY = 1e25f;

    auto viewA = reg.view<const cpAlienBehave, cpAlienStatus, cpPosition>();
    viewA.each( [&]( const cpAlienBehave & behave, cpAlienStatus & status, cpPosition & pos )
    {                   // Searching for highest alien position

      if( status.isDying )
        return;         // Alien is dying, does not count

      if( pos.Y < minAlienY )
        minAlienY = pos.Y;
    } );

    mIsInDangerousArea = false;
    auto viewP = reg.view<const cpPlayBehave, cpPlayStatus, cpPosition>();
    viewP.each( [&]( const cpPlayBehave & behave, cpPlayStatus & status, cpPosition & pos )
    {
        if( status.isDying )
          return;       // Player is dying, does not count

        if( pos.Y < minAlienY )
          mIsInDangerousArea = true;
                        // Player is too high - risky position, risky ...
    } );


  } // procPlayerInDanger::update

} // namespace Inv
