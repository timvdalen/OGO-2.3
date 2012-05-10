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

#ifdef WIN32
	#define sleep Sleep
#endif

namespace Lobby {

//------------------------------------------------------------------------------

struct Player
{
	typedef unsigned int Id;
	Id id;
	Net::Address addr;
	std::string name;
	bool ready;
	unsigned char team;
};

//------------------------------------------------------------------------------

class GameLobby
{
	public:
	void (*onConnect) (std::string gameName, Player::Id id);
	void (*onJoin)    (Player data);
	void (*onPart)    (Player::Id player);
	void (*onChat)    (Player::Id player, std::string line);
	void (*onReady)   (Player::Id player, bool ready);
	void (*onClose)   ();
	void (*onStart)   ();
	
	bool chat(const std::string &line);
	bool ready(bool ready);
	
	bool valid() { return !!data; }
	
	protected:
	void *data;
};

//------------------------------------------------------------------------------

class ClientLobby : public GameLobby
{
	public:
	ClientLobby(std::string playerName, const Net::Address &server);
	~ClientLobby();
	
	private:
	static void *listen(void *);
};

//------------------------------------------------------------------------------

class ServerLobby : public GameLobby
{
	public:
	ServerLobby(std::string gameName, std::string playerName, unsigned int port);
	~ServerLobby();
	
	bool start();
	
	private:
	static void *listen(void *);
	static void *broadcast(void *);
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _LOBBY_H

//------------------------------------------------------------------------------
