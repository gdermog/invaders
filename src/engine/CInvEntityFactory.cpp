//****************************************************************************************************
//! \file CInvEntityFactory.cpp
//! Module defines class CInvEntityFactory, which implements generator of in-game actors and objects.
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <CInvLogger.h>

#include <engine/CInvEntityFactory.h>
#include <engine/InvENTTComponents.h>
#include <engine/CInvGameScene.h>

#include <graphics/CInvSprite.h>
#include <graphics/CInvEffectSpriteAnimation.h>
#include <graphics/CInvEffectSpriteBlink.h>
#include <graphics/CInvEffectSpriteShrink.h>

namespace Inv
{
  static const std::string lModLogId( "ENTITYFACTORY" );

  //-------------------------------------------------------------------------------------------------

  CInvEntityFactory::CInvEntityFactory(
    const CInvSettings & settings,
    const CInvSpriteStorage & spriteStorage,
    entt::registry & enttRegistry,
    CInvGameScene & gScene,
    CInvSettingsRuntime & settingsRuntime,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB ):

    mSettings( settings ),
    mSpriteStorage( spriteStorage ),
    mEnTTRegistry( enttRegistry ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mGameScene( gScene ),
    mSettingsRuntime( settingsRuntime )
  {
  } // CInvEntityFactory::CInvEntityFactory

  //-------------------------------------------------------------------------------------------------

  CInvEntityFactory::~CInvEntityFactory() = default;

  //-------------------------------------------------------------------------------------------------

  entt::entity CInvEntityFactory::AddAlienEntity(
    const std::string & entityType,
    float posX, float posY,
    float vXGroup, float vYGroup,
    float alienSizeX )
  {
    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( entityType );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << entityType << "' does not exist, cannot create entity.";
      return {};
    } // if
    entitySprite->SetLevel( LVL_ALIEN );

    const auto invader = mEnTTRegistry.create();

    mEnTTRegistry.emplace<cpId>( invader, 1u, entityType, true, true );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( invader, posX, posY, 0.0f );
                        // component: position

    mEnTTRegistry.emplace<cpVelocity>( invader, vXGroup, vYGroup, 0.0f);
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( invader, alienSizeX, alienSizeX * aspectRatio );
                        // component: geometry

    mEnTTRegistry.emplace<cpAlienBehave>(
      invader,
      mSettingsRuntime.mAlienAnimationProbability,
      mSettingsRuntime.mAlienShootProbability * mSettingsRuntime.mSceneLevelMultiplicator,
      mSettingsRuntime.mAlienRaidProbability * mSettingsRuntime.mSceneLevelMultiplicator,
      mSettingsRuntime.mAlienRaidShootProbability * mSettingsRuntime.mSceneLevelMultiplicator,
      posX, posY, 100u );     // component: ai behavior /*??? SCORE ???*/

    mEnTTRegistry.emplace<cpAlienStatus>
    (                   // component: alien status
      invader,          // entity
      false,            // isAnimating
      false,            // isFiring
      false,            // isShootRequested
      false,            // isDying
      false,            // isInRaid
      false,            // isReturningToFormation
      0u,               // raidTicksLeft
      posX,             // formationX
      posY              // formationY
    );

    mEnTTRegistry.emplace<cpHealth>( invader, 1u, 1u );
                        // component: health points (single hit will do)

    mEnTTRegistry.emplace<cpDamage>( invader, 1u, true, false, false );
                        // component: entity damage (can hit player, no friendly fire,
                        // not dying/removed on hit)

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* There must be some diferentiation of animation indices according to sprite type.   */
/* Now it is just "PINK", but later, with more enemies, it will be necessary to       */
/* specify different animation points of interest and so on.                          */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

    auto standardAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    standardAnimationEffect->SetPace( 6 );
    standardAnimationEffect->SetImageRange( 16u, 23u );
    standardAnimationEffect->SetContinuous( false );
    standardAnimationEffect->Suspend();
    standardAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackAlienAnimationDone, invader )  );
    entitySprite->AddEffect( standardAnimationEffect );
                        // Standard animation effect starts suspended, it will be
                        // activated on random event.

    auto firingAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    firingAnimationEffect->SetPace( 6 );
    firingAnimationEffect->SetImageRange( 0u, 23u );
    firingAnimationEffect->SetContinuous( false );
    firingAnimationEffect->Suspend();
    firingAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackAlienFiringDone, invader ) );
    firingAnimationEffect->AddEventCallback(
      8u, BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackAlienShootRequested, invader ) );
    entitySprite->AddEffect( firingAnimationEffect );
                        // Firing animation effect starts suspended, it will be
                        // activated on random event.

    auto shrinkAnimationEffect = std::make_shared<CInvEffectSpriteShrink>( mSettings, mPd3dDevice, 11u );
    shrinkAnimationEffect->SetPace( 6 );
    shrinkAnimationEffect->SetContinuous( false );
    shrinkAnimationEffect->Suspend();
    shrinkAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackUnsetActive, invader ) );
    entitySprite->AddEffect( shrinkAnimationEffect );
                        // Shrink animation effect starts suspended (dying effect, not needed for now), it will
                        //  be activated on external event.

    mEnTTRegistry.emplace<cpGraphics>( invader,
      entitySprite, 0u, standardAnimationEffect, firingAnimationEffect, shrinkAnimationEffect, LARGE_INTEGER{0}, false );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, firing animation sequence, animation driver is zeroed )

    return invader;

  } // CInvEntityFactory::AddEntity

  //-------------------------------------------------------------------------------------------------

  entt::entity CInvEntityFactory::AddAlienBossEntity(
    AlienBossDescriptor_t & bossType,
    float posX, float posY,
    float vX, float vY, float alienSizeX )
  {
    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( bossType.mSpriteId );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << bossType.mSpriteId << "' does not exist, cannot create entity.";
      return {};
    } // if
    entitySprite->SetLevel( LVL_ALIEN );

    const auto boss = mEnTTRegistry.create();

    mEnTTRegistry.emplace<cpId>( boss, (uint64_t)bossType.mBossTypeId, bossType.mSpriteId, true, true );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( boss, posX, posY, 0.0f );
                        // component: position

    float div = std::sqrt( vX * vX + vY * vY );
    if( IsZero( div ) )
      div = mSettingsRuntime.mAlienRaidVelocity;
    else
      div = mSettingsRuntime.mAlienRaidVelocity / div;
    div *= mSettingsRuntime.mSceneLevelMultiplicator;
    div /= (float)mSettings.GetTickPerSecond();
    mEnTTRegistry.emplace<cpVelocity>( boss, vX * div, vY * div, 0.0f );
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( boss, alienSizeX, alienSizeX * aspectRatio );
                        // component: geometry

    mEnTTRegistry.emplace<cpAlienBehave>(
      boss, 0.0f,
      mSettingsRuntime.mAlienRaidShootProbability * mSettingsRuntime.mSceneLevelMultiplicator,
      0.0f, 0.0f, posX, posY, 5000u );
                        // component: ai behavior /*??? SCORE ???*/

    mEnTTRegistry.emplace<cpAlienBossStatus>( boss, false, false, false );
                        // component: alien status (not animating, not firing, shoot not requested, not dying )

    mEnTTRegistry.emplace<cpHealth>( boss, 1u, 1u );
                        // component: health points (single hit will do)

    mEnTTRegistry.emplace<cpDamage>( boss, 1u, true, false, false );
                        // component: entity damage (can hit player, no friendly fire,
                        // not dying/removed on hit)

    auto standardAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    standardAnimationEffect->SetPace( 6 );
    standardAnimationEffect->SetContinuous( true );
    entitySprite->AddEffect( standardAnimationEffect );
                        // Standard animation effect starts running in case of the bos and is continuous

    auto shrinkAnimationEffect = std::make_shared<CInvEffectSpriteShrink>( mSettings, mPd3dDevice, 11u );
    shrinkAnimationEffect->SetPace( 6 );
    shrinkAnimationEffect->SetContinuous( false );
    shrinkAnimationEffect->Suspend();
    shrinkAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackUnsetActive, boss ) );
    entitySprite->AddEffect( shrinkAnimationEffect );
                        // Shrink animation effect starts suspended (dying effect, not needed for now), it will
                        //  be activated on external event.

    mEnTTRegistry.emplace<cpGraphics>( boss,
      entitySprite, 0u, standardAnimationEffect, nullptr, shrinkAnimationEffect, LARGE_INTEGER{ 0 }, false );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, firing animation sequence, animation driver is zeroed )

  } // CInvEntityFactory::AddAlienBossEntity

  //-------------------------------------------------------------------------------------------------

  entt::entity CInvEntityFactory::AddPlayerEntity(
    const std::string & entityType,
    float posX, float posY,
    float playerSizeX )
  {
    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( entityType );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << entityType << "' does not exist, cannot create entity.";
      return {};
    } // if
    entitySprite->SetLevel( LVL_PLAYER );


#ifdef _DEBUG
    entitySprite->SetDebugId( DEBUG_ID_FIGHTER );
#endif

    const auto fighter = mEnTTRegistry.create();

    auto & pId = mEnTTRegistry.emplace<cpId>( fighter, 2u, entityType, true, true );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( fighter, posX, posY, 0.0f );
                        // component: position

    mEnTTRegistry.emplace<cpVelocity>( fighter, 0.0f, 0.0f, 0.0f );
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( fighter, playerSizeX, playerSizeX * aspectRatio );
                        // component: geometry

    mEnTTRegistry.emplace<cpPlayBehave>( fighter, -1 );
                        // component: player behavior is presently empty

    auto & pStat = mEnTTRegistry.emplace<cpPlayStatus>( fighter, true, false );
                        // component: player status (entering game invulnerable, shoot not requested, not dying )

    mEnTTRegistry.emplace<cpHealth>( fighter, 1u, 1u );
                        // component: health points (single hit will do)

    auto blinkAnimationEffect = std::make_shared<CInvEffectSpriteBlink>( mSettings, mPd3dDevice, 1u );
    blinkAnimationEffect->SetPace( 6 );
    blinkAnimationEffect->SetTicks( mSettingsRuntime.mPlayerInvulnerabilityTicks );
    blinkAnimationEffect->SetContinuous( false );
    blinkAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackPlayerInvulnerabilityCanceled, fighter ) );
    entitySprite->AddEffect( blinkAnimationEffect );
                        // Blinking animation effect starts running, as player is invulnerable on spawn.

   auto shrinkAnimationEffect = std::make_shared<CInvEffectSpriteShrink>( mSettings, mPd3dDevice, 11u );
   shrinkAnimationEffect->SetPace( 6 );
   shrinkAnimationEffect->SetContinuous( false );
   shrinkAnimationEffect->Suspend();
   shrinkAnimationEffect->AddEventCallback(
     BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackUnsetActive, fighter ) );
   entitySprite->AddEffect( shrinkAnimationEffect );
                        // Shrink animation effect starts suspended (dying effect, not needed for now), it will
                        //  be activated on external event.

#ifdef _DEBUG
   shrinkAnimationEffect->SetDebugId( DEBUG_ID_FIGHTER );
#endif

    mEnTTRegistry.emplace<cpGraphics>( fighter,
      entitySprite, 0u, blinkAnimationEffect, nullptr, shrinkAnimationEffect, LARGE_INTEGER{ 0 }, false );
                        // component: graphics (sprite, invulnerability and dying effects are stored, animation
                        // driver is zeroed )

    return fighter;

  } // CInvEntityFactory::AddPlayerEntity

  //-------------------------------------------------------------------------------------------------

  entt::entity CInvEntityFactory::AddMissileEntity(
    const std::string & entityType,
    bool fromPlayer,
    float posX, float posY,
    float missileSizeX,
    float directionX,
    float directionY )
  {

    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( entityType );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << entityType << "' does not exist, cannot create entity.";
      return {};
    } // if
    entitySprite->SetLevel( LVL_MISSILE );

    const auto missile = mEnTTRegistry.create();


    mEnTTRegistry.emplace<cpId>( missile, 3u, entityType, true, false );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( missile, posX, posY, 0.0f );
                        // component: position

    float vDiv = std::sqrt( directionX * directionX + directionY * directionY );
    vDiv = ( IsZero( vDiv ) ? 1.0f : 1.0f / vDiv );
    float vSize = ( fromPlayer ? mSettingsRuntime.mRocketVelocity : mSettingsRuntime.mSpitVelocity );
    vSize /= (float)mSettings.GetTickPerSecond();
    mEnTTRegistry.emplace<cpVelocity>( missile, directionX * vSize * vDiv, directionY * vSize * vDiv, 0.0f );
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( missile, missileSizeX, missileSizeX * aspectRatio );
                        // component: geometry

    mEnTTRegistry.emplace<cpDamage>( missile, 1u, !fromPlayer, fromPlayer, true );
                        // component: entity damage

    auto standardAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    standardAnimationEffect->SetPace( 6 );
    standardAnimationEffect->SetContinuous( true );
    entitySprite->AddEffect( standardAnimationEffect );
                        // Missile is animated continuously and have no event bound to animation

    mEnTTRegistry.emplace<cpGraphics>( missile,
      entitySprite, 0u, standardAnimationEffect,  nullptr, nullptr, LARGE_INTEGER{ 0 }, false );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, no firing animation sequence, animation driver is zeroed )

    return missile;

  } // CInvEntityFactory::AddMissileEntity

  //-------------------------------------------------------------------------------------------------

  entt::entity CInvEntityFactory::AddExplosionEntity(
    const std::string & entityType,
    float posX, float posY,
    float explosionSizeX,
    float velocityX, float velocityY )
  {
    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( entityType );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << entityType << "' does not exist, cannot create entity.";
      return {};
    } // if
    entitySprite->SetLevel( LVL_EXPLOSION );

#ifdef _DEBUG
    entitySprite->SetDebugId( DEBUG_ID_FIGHTER_EXPLODE );
#endif

    const auto explosion = mEnTTRegistry.create();

    auto & explosionId = mEnTTRegistry.emplace<cpId>( explosion, 4u, entityType, true, false );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( explosion, posX, posY, 0.0f );
                        // component: position

    mEnTTRegistry.emplace<cpVelocity>( explosion, velocityX, velocityY, 0.0f );
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( explosion, explosionSizeX, explosionSizeX * aspectRatio );
                        // component: geometry

    auto explosionTicks = (uint32_t)( mExplosionTime * (float)mSettings.GetTickPerSecond() );
    auto explosionPace = (uint32_t)( explosionTicks / entitySprite->GetNumberOfImages() );
    if( 0u == explosionPace )
      explosionPace = 1u;

    auto standardAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    standardAnimationEffect->SetPace( explosionPace );
    standardAnimationEffect->SetContinuous( false );
    standardAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackUnsetActive, explosion ) );
    entitySprite->AddEffect( standardAnimationEffect );
                        // Explosion is animated once. After animation is finished, it is removed from game.

#ifdef _DEBUG
    standardAnimationEffect->SetDebugId( DEBUG_ID_FIGHTER_EXPLODE );
#endif

    mEnTTRegistry.emplace<cpGraphics>( explosion,
      entitySprite, 0u, standardAnimationEffect, nullptr, nullptr, LARGE_INTEGER{ 0 }, false );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, no firing animation sequence, animation driver is zeroed )

    return explosion;

  } // CInvEntityFactory::AddExplosionEntity


} // namespace Inv
