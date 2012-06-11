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

namespace NetCode {

using namespace Core;
using namespace Net;
using namespace Protocol;
using namespace Base::Alias;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[]);

void Terminate();

void Frame();

bool TryLock();
void Unlock();

//------------------------------------------------------------------------------

bool Connected();
double CurrentCPS();

void Disconnect();
bool Connect(std::string host);

bool Send(const Message &, bool reliable = false);

//------------------------------------------------------------------------------

void Chat(std::string line);
void Move(Pd position, Vd velocity);

void Debug();

//------------------------------------------------------------------------------

} // namespace NetCode

#endif // _NETCODE_H

//------------------------------------------------------------------------------
