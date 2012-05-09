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

struct Game
{
	std::string name;        //!< Game name
	unsigned int numPlayers; //!< Number of players currently on the game
	time_t ttl;              //!< Time to live. Time before game is removed form the list.
};

//------------------------------------------------------------------------------

class GameList
{
	public:
	void (*onJoin)   (Net::Address server, Game game);
	void (*onChange) (Net::Address server, unsigned int numPlayers);
	void (*onPart)   (Net::Address server);
	
	//! Creates a gamelist that listens for games on the specified port.
	GameList(unsigned int port);
	~GameList();
	
	bool valid() { return !!data; }
	
	private:
	void *data;
	static void *listen(void *arg);
};

//------------------------------------------------------------------------------

} // namespace Games

#endif // _GAMES_H

//------------------------------------------------------------------------------
