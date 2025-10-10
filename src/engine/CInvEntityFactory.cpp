//****************************************************************************************************
//! \file CInvEntityFactory.cpp
//! Module defines class CInvEntityFactory, which implements
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <CInvLogger.h>

#include <engine/CInvEntityFactory.h>
#include <engine/InvENTTComponents.h>
#include <engine/CInvGameScene.h>

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
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB ):

    mSettings( settings ),
    mSpriteStorage( spriteStorage ),
    mEnTTRegistry( enttRegistry ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mGameScene( gScene )
  {
  } // CInvEntityFactory::CInvEntityFactory

  //-------------------------------------------------------------------------------------------------

  CInvEntityFactory::~CInvEntityFactory() = default;

  //-------------------------------------------------------------------------------------------------

  entt::entity CInvEntityFactory::AddAlienEntity(
    const std::string & entityType,
    float posX, float posY,
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


    mEnTTRegistry.emplace<cpId>( invader, (uint64_t)invader, entityType, true, false );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( invader, posX, posY, 0.0f );
                        // component: position

    mEnTTRegistry.emplace<cpVelocity>( invader, 0.0f, 0.0f, 0.0f );
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( invader, alienSizeX, alienSizeX * aspectRatio );
                        // component: geometry

    mEnTTRegistry.emplace<cpAlienBehave>( invader, 0.0025f, 0.001f );
                        // component: ai behavior

    mEnTTRegistry.emplace<cpAlienStatus>( invader, false, false, false, false );
                        // component: alien status (not animating, not firing, shoot not requested, not dying )

    mEnTTRegistry.emplace<cpHealth>( invader, 1u, 1u );
                        // component: health points (single hit will do)

    mEnTTRegistry.emplace<cpDamage>( invader, 1u, true, false, false );
                        // component: entity damage (can hit player, no friendly fire,
                        // not dying/removed on hit)

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* There must be some diferentiation of animation indices according to sprite type.   */
/* Now it is just "PINK", but later, with more enemies, it will be necessary to spe-  */
/* different animation points of interest and so on.                                  */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

    auto &aStat = mEnTTRegistry.get<cpAlienStatus>( invader );


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

    mEnTTRegistry.emplace<cpGraphics>( invader,
      entitySprite, 0u, standardAnimationEffect, firingAnimationEffect, LARGE_INTEGER{0} );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, firing animation sequence, animation driver is zeroed )

    return invader;

  } // CInvEntityFactory::AddEntity

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

    auto & pId = mEnTTRegistry.emplace<cpId>( fighter, (uint64_t)fighter, entityType, true, true );
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

    auto & pStat = mEnTTRegistry.emplace<cpPlayStatus>( fighter, true, false, false );
                        // component: player status (entering game invulnerable, shoot not requested, not dying )

    mEnTTRegistry.emplace<cpHealth>( fighter, 1u, 1u );
                        // component: health points (single hit will do)

    auto blinkAnimationEffect = std::make_shared<CInvEffectSpriteBlink>( mSettings, mPd3dDevice, 1u );
    blinkAnimationEffect->SetPace( 6 );
    blinkAnimationEffect->SetTicks( 150 );
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
      entitySprite, 0u, blinkAnimationEffect, shrinkAnimationEffect, LARGE_INTEGER{ 0 } );
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
    float velocityX, float velocityY )
  {

    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( entityType );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << entityType << "' does not exist, cannot create entity.";
      return {};
    } // if
    entitySprite->SetLevel( LVL_MISSILE );

    const auto missile = mEnTTRegistry.create();


    mEnTTRegistry.emplace<cpId>( missile, (uint64_t)missile, entityType, true, false );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( missile, posX, posY, 0.0f );
                        // component: position

    mEnTTRegistry.emplace<cpVelocity>( missile, velocityX, velocityY, 0.0f );
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
      entitySprite, 0u, standardAnimationEffect,  nullptr, LARGE_INTEGER{ 0 } );
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

    auto & explosionId = mEnTTRegistry.emplace<cpId>( explosion, (uint64_t)explosion, entityType, true, false );
                        // component: entity full identifier

    mEnTTRegistry.emplace<cpPosition>( explosion, posX, posY, 0.0f );
                        // component: position

    mEnTTRegistry.emplace<cpVelocity>( explosion, velocityX, velocityY, 0.0f );
                        // component: velocity

    auto baseSize = entitySprite->GetImageSize( 0 );
    auto aspectRatio = (float)baseSize.second / (float)baseSize.first;
    mEnTTRegistry.emplace<cpGeometry>( explosion, explosionSizeX, explosionSizeX * aspectRatio );
                        // component: geometry

    auto standardAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    standardAnimationEffect->SetPace( 6 );
    standardAnimationEffect->SetContinuous( false );
    standardAnimationEffect->AddEventCallback(
      BIND_MEMBER_EVENT_CALLBACK_ON( &mGameScene, CInvGameScene::CallbackUnsetActive, explosion ) );
    entitySprite->AddEffect( standardAnimationEffect );
                        // Explosion is animated once. After animation is finished, it is removed from game.

#ifdef _DEBUG
    standardAnimationEffect->SetDebugId( DEBUG_ID_FIGHTER_EXPLODE );
#endif

    mEnTTRegistry.emplace<cpGraphics>( explosion,
      entitySprite, 0u, standardAnimationEffect, nullptr, LARGE_INTEGER{ 0 } );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, no firing animation sequence, animation driver is zeroed )

    return explosion;

  } // CInvEntityFactory::AddExplosionEntity


} // namespace Inv
