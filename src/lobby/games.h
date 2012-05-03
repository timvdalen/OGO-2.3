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

#include <time.h>
#include <string>

#include "common.h"
#include "net.h"

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
	typedef std::map<Net::Address,Game> List; //!< Maps ip address to games.
	
	//! List with received servers.
	List list;
	
	//! Creates a gamelist that listens for games on the specified port.
	GameList(unsigned int port);
	~GameList();
	
	//! Update the game list. \return whether the list has changed
	bool refresh();
	
	private:
	Net::UDPSocket *sock;
};

//------------------------------------------------------------------------------

} // namespace Games

#endif // _GAMES_H

//------------------------------------------------------------------------------
