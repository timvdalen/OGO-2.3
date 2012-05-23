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
#include "games.h"

namespace Lobby {

//------------------------------------------------------------------------------

struct Player
{
	typedef unsigned int Id;
	enum State
	{
		stBusy,
		stReady,
		stHost
	};
	
	Id id;
	unsigned char team;
	State state;
	std::string name;
};

//------------------------------------------------------------------------------

class GameLobby
{
	public:
	void (*onConnect) (Player::Id pid, Game game);
	void (*onPlayer)  (Player player);
	void (*onJoin)    (Player::Id pid, std::string playerName);
	void (*onPart)    (Player::Id pid);
	void (*onTeam)    (Player::Id pid, unsigned char team);
	void (*onState)   (Player::Id pid, Player::State state);
	void (*onChat)    (Player::Id pid, std::string line);
	void (*onClose)   ();
	void (*onStart)   ();
	
	GameLobby();
	
	bool team(unsigned char team);
	bool chat(const std::string &line);
	
	bool valid() const;
	void close();
	
	protected:
	void *data;
};

//------------------------------------------------------------------------------

class ClientLobby : public GameLobby
{
	public:
	ClientLobby(std::string playerName, const Net::Address &server);
	~ClientLobby();
	
	bool state(bool ready);
	
	private:
	static void *listen(void *);
};

//------------------------------------------------------------------------------

class ServerLobby : public GameLobby
{
	public:
	ServerLobby(std::string gameName, std::string playerName, unsigned int port);
	~ServerLobby();
	
	bool team(unsigned char team);
	bool chat(const std::string &line);
	bool start();
	
	private:
	static void *listen(void *);
	static void *broadcast(void *);
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _LOBBY_H

//------------------------------------------------------------------------------
