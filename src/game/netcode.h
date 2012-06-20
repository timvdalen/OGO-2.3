/*
 * Network code module
 *
 * Date: 04-06-12 14:03
 *
 * Description: Defines the network protocol used by the game
 *
 */

#ifndef _NETCODE_H
#define _NETCODE_H

#include "base.h"
#include "net.h"
#include "protocol.h"
#include "game.h"
#include "player.h"

namespace NetCode {

using namespace Core;
using namespace Net;
using namespace Protocol;
using namespace Base::Alias;
using namespace Objects;

extern string MessageOfTheDay;

//------------------------------------------------------------------------------
//! Initializes the networking code
void Initialize(int argc, char *argv[]);

//! Cleans up the networking code
void Terminate();

void Frame();   //!< Processes network messages

bool TryLock(); //!< Checks if reliable messages are allowed at this time
void Unlock();  //!< Pass the abillity to do reliable messages

//------------------------------------------------------------------------------

bool Connected();    //!< Returns whether wether networking is in use
bool Reliable();     //!< Returns whether the network consistency is reliable

double CurrentCPS(); //!< Returns the current cycle rate of the token ring

void Disconnect();   //!< Resets the current connection
bool Connect(std::string host); //!< Connects to an external host

bool Send(const Message &, bool reliable = false); //!< Sends a raw message

//------------------------------------------------------------------------------

/* Connection control */

//! A player wants to enter the game (this is called automatically)
void Enter(unsigned char team, string name);
//! A player wants to re-enter the game after connection loss.
//! \note currently not supported!
void ReEnter(Player::Id pid);
//! A host confirms a player enter request
void Welcome(Player::Id pid);
//! A host send his game state to a player
void Sync(Player::Id pid);

/* Unreliable communication */

//! Sends a chat message to everyone
void Chat(std::string line);
//! Sends a chat message to team members
void TeamChat(std::string line);
//! Updates current player position
void Move(Pd position, Vd velocity);
//! Updates current player direction
void Look(Qd rotation);
//! Updates team resources
void Team(unsigned char team, Resource gold);
//! Replicates a laserbeam (visuals only)
void Fire(const LaserBeam &);

/* RELIABLE communication, can ONLY be used in the critical section !!! */

//! Inflicts damage on another player
//! \note when self is false one of the owned towers will be the cause
void Hit(Player::Id, double damage, bool self);
//! A player notes he just died
//! \note id is zero for tower kills
void Died(Player::Id);
//! A player builds a building or resets a structure
void Build(GridPoint, Structure *);
//! A player picks up a droppable
void Take(unsigned long id);
//! A player drops a droppable
void Drop(Droppable *);
//! A player inflicts damage on a tower
//! \note when self is false one of the owned towers will be the cause
void Attack(GridPoint, double damage, bool self);
//! A player notes one of his owned towers was destroyed
//! \note also sends the player that caused it or zero when it was a tower
void Destroy(GridPoint, Player::Id);
//! A player notes the game is over and sends the winning team
void End(unsigned char team);

void Debug();

//------------------------------------------------------------------------------

} // namespace NetCode

#endif // _NETCODE_H

//------------------------------------------------------------------------------
