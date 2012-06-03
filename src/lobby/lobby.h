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
#include "games.h"

namespace Lobby {

//------------------------------------------------------------------------------
//! Representation of a player in a lobby session
struct Player
{
	typedef unsigned int Id; //!< Player Id type
	//! Player state definition
	enum State
	{
		stBusy,  //!< Player is not ready
		stReady, //!< Player is ready
		stHost   //!< Player is hosting the lobby session
	};
	
	Id id;              //!< Player id
	unsigned char team; //!< Team id of the team  the player is in
	State state;        //!< Player's state
	std::string name;   //!< PLayer's name
};

//------------------------------------------------------------------------------
//! A lobby session
class GameLobby
{
	public:
	//! Callback: Successfully connected to a lobby session
	void (*onConnect) (Player::Id pid, Game game);
	//! Callback: Recieved player info
	void (*onPlayer)  (Player player);
	//! Callback: A player has joined the lobby
	void (*onJoin)    (Player::Id pid, std::string playerName);
	//! Callback: A player has left the lobby
	void (*onPart)    (Player::Id pid);
	//! Callback: A player has switched teams
	void (*onTeam)    (Player::Id pid, unsigned char team);
	//! Callback: A player has changed its ready state
	void (*onState)   (Player::Id pid, Player::State state);
	//! Callback: A player sent a chat message
	void (*onChat)    (Player::Id pid, std::string line);
	//! Callback: Lobby session was terminated
	void (*onClose)   ();
	//! Callback: The host wants to start the game
	void (*onStart)   ();
	
	GameLobby();
	
	//! Sends a request to change the local player's team
	bool team(unsigned char team);
	//! Lets the local player send a chat message
	bool chat(const std::string &line);
	
	bool valid() const; //!< Returns the validity of the lobby session
	void close();       //!< Closes the lobby session
	
	protected:
	void *data;
};

//------------------------------------------------------------------------------
//! A joining lobby session
class ClientLobby : public GameLobby
{
	public:
	//! Tries to join the lobby session at the specified server address
	ClientLobby(std::string playerName, const Net::Address &server);
	~ClientLobby();
	
	//! Sends a request to change the state of the local player
	bool state(bool ready);
	
	private:
	static void *listen(void *);
};

//------------------------------------------------------------------------------
//! A hosting lobby session
class ServerLobby : public GameLobby
{
	public:
	//! Creates a new lobby session listening for clients on specified port
	ServerLobby(std::string gameName, std::string playerName, unsigned int port);
	~ServerLobby();
	
	//! Changes the local player's team
	bool team(unsigned char team);
	//! Lets the local player send a chat message
	bool chat(const std::string &line);
	//! Sends a start game notification
	bool start();
	
	private:
	static void *listen(void *);
	static void *broadcast(void *);
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _LOBBY_H

//------------------------------------------------------------------------------
