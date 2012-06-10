/*
 * Network code module
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "netalg.h"
#include "netcode.h"

namespace NetCode {

using namespace std;

TokenRing *tokenring = NULL;
uword port = GAME_PORT;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[])
{
	string host;
	
	for (int i = 0; i < argc - 1; ++i)
	{
		if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--connect"))
			host = argv[++i];
		if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "--port"))
			port = atoi(argv[++i]);
	}
	
	tokenring = new TokenRing(port);
	
	if (host.empty())   // Server
		Game::Notice(string("Listening on port ") + Argument(port).str + string("..."));
	else Connect(host); //Client
}

//------------------------------------------------------------------------------

void Terminate()
{
	if (tokenring) delete tokenring;
	tokenring = NULL;
}

//------------------------------------------------------------------------------

void Frame()
{
}

//==============================================================================

bool Connected()
{
	return (tokenring && tokenring->connected());
}

//------------------------------------------------------------------------------

void Disconnect()
{
	if (tokenring) delete tokenring;
	tokenring = new TokenRing(port);
	Game::Notice(string("Disconnected: listening on port ") + Argument(port).str + string("..."));
}

//------------------------------------------------------------------------------

bool Connect(std::string host)
{
	if (Connected()) Disconnect();
	Game::Notice(string("Connecting to ") + host + string("..."));
	Address remote(host.c_str());
	if (!tokenring->connect(remote), 30);
		Game::Notice(string("Unable to connect to " + host + string("!")));
}

//==============================================================================

bool Send(const Message &msg, bool reliable)
{
	if (!Connected()) return false;
	return tokenring->shout(msg, reliable);
}

//------------------------------------------------------------------------------

} // namespace NetCode

//------------------------------------------------------------------------------
