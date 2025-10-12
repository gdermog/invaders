//****************************************************************************************************
//! \file InvENTTComponents.h
//! Module contains EnTT components declarations
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#ifndef H_InvENTTComponents
#define H_InvENTTComponents

#include <InvGlobals.h>

namespace Inv
{

  //****** component: entity full identifier *********************************************************

  struct cpId
  {

    uint64_t id;        //!< Full entity identifier (index)
    std::string typeId; //!< Type identifier (for logging and debugging purposes)
    bool active;        //!< \b true if the entity is active. Inactive entity is not processed nor
                        //!  displayed in game loop and it will be pruned in nearest possiblev time.
    bool noticeOnPruning;
                        //!< \b true if the entity should send notification when it is pruned
  };

  //****** component: position ***********************************************************************

  struct cpPosition
  {
    float X;            //!< X position (of centre of object) [px]
    float Y;            //!< Y position (of centre of object) [px]
    float Z;            //!< Z position (of centre of object) [px], usually constant as
    //!  the game is 2D
  };

  //****** component: velocity ***********************************************************************

  struct cpVelocity
  {
    float vX;           //!< X translation velocity (of centre of object) [px/tick]
    float vY;           //!< Y translation velocity (of centre of object) [px/tick]
    float vZ;           //!< Z translation velocity (of centre of object) [px/tick], usually
    //!  zero as the game is 2D
  };

  //****** component: geometry ***********************************************************************

  struct cpGeometry
  {
    float width;        //!< Width of object [px]
    float height;       //!< Height of object [px]
  };

  //****** component: ai behavior ********************************************************************

  /*! \brief This component determines the actual behavior of the computer-controlled element. */
  struct cpAlienBehave
  {
    float animationProbability;
                        //!< Speed of basic animation, usually a small positive number

    float shootProbability;
                        //!< Probability of shooting in each game tick, usually a small

    float startX;       //!< Starting X position (of centre of alien object) [px]
    float startY;       //!< Starting Y position (of centre of alien object) [px]

  };

  //****** component: alien status *******************************************************************

  /*! \brief This component determines the status of the alien computer-controlled element. */
  struct cpAlienStatus
  {

    bool isAnimating;   //!< \b true if the alien shoul play its own basic animation.

    bool isFiring;      //!< \b true if the alien is in firing state, false otherwise.

    bool isShootRequested;
                        //!< \b true if the alien requested to shoot in current tick.

    bool isDying;       //!< \b true if the alien is in dying state, false otherwise.
  };

  //****** component: player behavior ****************************************************************

  /*! \brief This component determines the actual behavior of the player-controlled element. */
  struct cpPlayBehave
  {
    int dummy;          //!< Placeholder, currently no behavior is defined.
  };

  //****** component: player status ******************************************************************

  /*! \brief This component determines the status of the player-controlled element. */
  struct cpPlayStatus
  {
    void InvulnerabilityCanceled( uint32_t )
    {
      isInvulnerable = false;
    }

    bool isInvulnerable;//!< \b true if the player is in invulnerable state, false otherwise.
                        //!  Invulnerability may be granted for short time after respawn or by
                        //!  picking up special power-up.

    bool isShootRequested;
                        //!< \b true if the player requested to shoot in current tick.

    bool isDying;       //!< \b true if the player is in dying state, false otherwise.
  };

  //****** component: entity health ******************************************************************

  /*! \brief This component determines that the entity can be destroyed. */
  struct cpHealth
  {
    uint32_t hitPoints; //!< Current hit points of the entity. When it reaches zero,
    //!  the entity is destroyed. Usually 1, but can be higher for
    //!  more resilient entities.

    uint32_t maxHitPoints;
    //!< Maximum hit points of the entity, used for  displaying health bars,
    //!  healing purposes and so on.  Remark: this has nothing to do with number
    //!  of player lives (ships), number of hitpoints defines how many hits the single
    //!  ship can take before it is destroyed.
  };

  //****** component: entity damage ******************************************************************

  /*! \brief This component determines that the entity can deal damage to other entities. */
  struct cpDamage
  {
    uint32_t damagePoints;
    //!< Damage points dealt to other entity when a collision occurs.

    bool dangerToPlayer;//!< \b true if the entity can deal damage to player, false otherwise.

    bool dangerToAliens;//!< \b true if the entity can deal damage to aliens (other than player),

    bool removeOnHit;   //!< \b true if the entity is removed from game when it hits another
    //!  entity, false otherwise. Only special ammo may have this set to
    //!  true (some form of piercing ammo)
  };

  //****** component: entity graphics *****************************************************************

  class CInvSprite;
  class CInvEffect;

  /*! \brief */
  struct cpGraphics
  {
    std::shared_ptr<CInvSprite> standardSprite;
    //!< Pointer to sprite object used to render the entity in standard
    //!  way on screen. Remark: should contein copy of CInvSprite from CInvSpriteStorage,
    //!  not a reference, as the sprite may have unique set of effect applied for
    //!  each entity.

    uint32_t staticStandardImageIndex;
    //!< Index of image in standard sprite to be used when no animation

    std::shared_ptr<CInvEffect> standardAnimationEffect;
    //!< Pointer to animation effect applied to standard sprite

    std::shared_ptr<CInvEffect> specificAnimationEffect;
    //!< Pointer to special animation effect applied, as firing invader sprite or similar

    std::shared_ptr<CInvEffect> dyingAnimationEffect;
    //!< Pointer to special animation effect applied,of entity is dying.

    LARGE_INTEGER diffTick;
    //!< Animation driver

  };


} // namespace Inv

#endif
