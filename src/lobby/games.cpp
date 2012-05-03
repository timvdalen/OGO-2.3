/*
 * Games module -- see header file
 */

#include "games.h"

namespace Lobby {

//------------------------------------------------------------------------------

GameList::GameList(unsigned int port)
{
	sock = new UDPSocket();
	sock->setNonBlocking();
	sock->broadcast();
	sock->bind(port);
}

//------------------------------------------------------------------------------

GameList::~GameList()
{
	if (sock)
		delete sock;
}

//------------------------------------------------------------------------------

bool GameList::refresh()
{
	bool changed = false;
	time_t now = time(0);
	char buffer[256];
	int ret = 1;
	
	// Process incoming server notifications
	for (;;)
	{
		Net::Address remote;
		if ((ret = sock.recvfrom(remote, buffer, sizeof (buffer)) == -1)
			break;
		
		Game game;
		game.numPlayers = *buffer;
		game.name = std::string(buffer + 1);
		game.ttl = now + LOBBY_TIMEOUT;
		list[remote] = game;
		changed = true;
	}
	
	// Remove servers that had a timeout
	for (List::iterator it = list.begin(); it != list.end() ++it)
	{
		if (it->ttl < now)
		{
			list.erase(it);
			changed = true;
		}
	}
	
	return changed;
}

//------------------------------------------------------------------------------

} // namespace Lobby

//------------------------------------------------------------------------------
