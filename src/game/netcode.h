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

#include "net.h"
#include "protocol.h"
#include "game.h"

namespace NetCode {

using namespace Net;
using namespace Protocol;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[]);

void Terminate();

void Frame();

//------------------------------------------------------------------------------

bool Connected();

void Disconnect();
bool Connect(std::string host);

//------------------------------------------------------------------------------

bool Send(const Message &, bool reliable = false);

//------------------------------------------------------------------------------

} // namespace NetCode

#endif // _NETCODE_H

//------------------------------------------------------------------------------
