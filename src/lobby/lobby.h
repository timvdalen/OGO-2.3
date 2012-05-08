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
	void (*onConnect) (std::string gameName);
	void (*onJoin)    (Net::Address player, std::string name);
	void (*onPart)    (Net::Address player);
	void (*onChat)    (Net::Address player, std::string line);
	void (*onReady)   (Net::Address player, bool ready);
	void (*onClose)   ();
	void (*onStart)   ();
	
	bool chat(const std::string &line);
	bool ready(bool ready);
	
	bool valid() { return !!data; }
	
	private:
	void *data;
};

//------------------------------------------------------------------------------

class ClientLobby : public GameLobby
{
	public:
	ClientLobby(std::string playerName, const Net::Address &server);
	~ClientLobby();
};

//------------------------------------------------------------------------------

class ServerLobby : public GameLobby
{
	public:
	ServerLobby(std::string gameName, std::string playerName, unsigned int port);
	~ServerLobby();
	
	void start();
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _LOBBY_H

//------------------------------------------------------------------------------
