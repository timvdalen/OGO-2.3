/*
 * Games list module
 * 
 * Date: 26-04-12 12:33
 *
 * Description: Maintains a list of active servers and possible create one
 *
 */

#ifndef _GAMES_H
#define _GAMES_H

#include <string>

#include "net.h"
#include "common.h"

//! Lobby module
namespace Lobby {

//------------------------------------------------------------------------------

//! Representation of a game in the lobby's server list
struct Game
{
	std::string name;        //!< Game name
	unsigned int numPlayers; //!< Number of players currently in the game
};

//------------------------------------------------------------------------------

//! The lobby's server list: maintains a list of local servers.
//! On creation this class will spawn a thread that listens for lobby servers
//! on the local subnet.
class GameList
{
	public:
	//! Callback: a new server was found
	void (*onJoin)   (Net::Address server, Game game);
	//! Callback: server info has changed
	void (*onChange) (Net::Address server, Game game);
	//! Callback: a server was lost
	void (*onPart)   (Net::Address server);
	
	//! Creates a gamelist that listens for games on the specified port.
	GameList(unsigned int port);
	~GameList();
	
	//! Returns the validity of the game polling mechanism
	bool valid() const { return !!data; }
	
	private:
	void *data;
	static void *listen(void *arg);
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _GAMES_H

//------------------------------------------------------------------------------
