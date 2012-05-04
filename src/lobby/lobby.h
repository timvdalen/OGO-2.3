/*
 * Lobby module
 * 
 * Date: 26-04-12 12:29
 *
 * Description: Maintains a list of players in the selected game
 *
 */

#ifndef _LOBBY_H
#define _LOBBY_H

#include <string>
#include <vector>

#include "common.h"
#include "net.h"

namespace Lobby {

//------------------------------------------------------------------------------

struct Player
{
	std::string name;
	bool ready;
};

//------------------------------------------------------------------------------

class GameLobby
{
	public:
	typedef std::vector<Player>List;
	
	List list;
	std::string name;
};

//------------------------------------------------------------------------------

class ClientLobby : public GameLobby
{
	ClientLobby(std::string playerName, const Net::Address &server);
	~ClientLobby();
};

//------------------------------------------------------------------------------

class ServerLobby : public GameLobby
{
	ServerLobby(std::string gameName, unsigned int port);
	~ServerLobby();
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _LOBBY_H

//------------------------------------------------------------------------------
