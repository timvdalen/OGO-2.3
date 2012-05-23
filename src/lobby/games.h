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
	unsigned int numPlayers; //!< Number of players currently in the game
};

//------------------------------------------------------------------------------

class GameList
{
	public:
	void (*onJoin)   (Net::Address server, Game game);
	void (*onChange) (Net::Address server, Game game);
	void (*onPart)   (Net::Address server);
	
	//! Creates a gamelist that listens for games on the specified port.
	GameList(unsigned int port);
	~GameList();
	
	bool valid() const { return !!data; }
	
	private:
	void *data;
	static void *listen(void *arg);
};

//------------------------------------------------------------------------------

} // namespace Lobby

#endif // _GAMES_H

//------------------------------------------------------------------------------
