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

void Enter(unsigned char team, string name);
void ReEnter(Player::Id pid);
void Welcome(Player::Id pid);
void Sync(Player::Id pid);

void Chat(std::string line);
void TeamChat(std::string line);
void Move(Pd position, Vd velocity);
void Look(Qd rotation);
void Team(unsigned char team, Resource gold);

void Debug();

//------------------------------------------------------------------------------

} // namespace NetCode

#endif // _NETCODE_H

//------------------------------------------------------------------------------
