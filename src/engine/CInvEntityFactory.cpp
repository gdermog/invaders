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


namespace Inv
{
  static const std::string lModLogId( "ENTITYFACTORY" );

  //-------------------------------------------------------------------------------------------------

  CInvEntityFactory::CInvEntityFactory(
    const CInvSettings & settings,
    const CInvSpriteStorage & spriteStorage,
    entt::registry & enttRegistry,
    LPDIRECT3D9 pD3D,
    LPDIRECT3DDEVICE9 pd3dDevice,
    LPDIRECT3DVERTEXBUFFER9 pVB ):

    mSettings( settings ),
    mSpriteStorage( spriteStorage ),
    mEnTTRegistry( enttRegistry ),
    mPD3D( pD3D ),
    mPd3dDevice( pd3dDevice ),
    mPVB( pVB ),
    mNextEntityId( 1ul )
  {
  } // CInvEntityFactory::CInvEntityFactory

  //-------------------------------------------------------------------------------------------------

  CInvEntityFactory::~CInvEntityFactory() = default;

  //-------------------------------------------------------------------------------------------------

  void CInvEntityFactory::AddAlienEntity(
    const std::string & entityType,
    float posX, float posY,
    float alienSizeX )
  {
    std::shared_ptr<CInvSprite> entitySprite = mSpriteStorage.GetSprite( entityType );
    if( nullptr == entitySprite )
    {
      LOG << "Error: Sprite with ID '" << entityType << "' does not exist, cannot create entity.";
      return;
    } // if

    const auto invader = mEnTTRegistry.create();


    mEnTTRegistry.emplace<cpId>( invader, mNextEntityId++, entityType, true );
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

    mEnTTRegistry.emplace<cpDamage>( invader, 1u, false, true );
                        // component: entity damage (can hit player, no friendly fire,
                        // dying/removed on hit)

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* There must be some diferentiation of animation indexex according to sprite type.   */
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
    standardAnimationEffect->AddEventCallback( BIND_MEMBER_EVENT_CALLBACK( &aStat, cpAlienStatus::AnimationDone ) );
    entitySprite->AddEffect( standardAnimationEffect );
                        // Standard animation effect starts suspended, it will be
                        // activated on random event.

    auto firingAnimationEffect = std::make_shared<CInvEffectSpriteAnimation>(
      mSettings, mPd3dDevice, 1u );
    firingAnimationEffect->SetPace( 6 );
    firingAnimationEffect->SetImageRange( 0u, 23u );
    firingAnimationEffect->SetContinuous( false );
    firingAnimationEffect->Suspend();
    firingAnimationEffect->AddEventCallback( BIND_MEMBER_EVENT_CALLBACK( &aStat, cpAlienStatus::FiringDone ) );
    firingAnimationEffect->AddEventCallback( 8u, BIND_MEMBER_EVENT_CALLBACK( &aStat, cpAlienStatus::FiringDone ) );
    entitySprite->AddEffect( firingAnimationEffect );
                        // Firing animation effect starts suspended, it will be
                        // activated on random event.

    mEnTTRegistry.emplace<cpGraphics>( invader,
      entitySprite, 0u, standardAnimationEffect, entitySprite, firingAnimationEffect, LARGE_INTEGER{0} );
                        // component: graphics (sprite, static image index, standard animation
                        // sequence, firing animation sequence, animation driver is zeroed )

  } // CInvEntityFactory::AddEntity


} // namespace Inv
