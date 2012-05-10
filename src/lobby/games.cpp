/*
 * Games module -- see header file
 */

#include <time.h>
#include <pthread.h>

#include <map>

#include "protocol.h"
#include "games.h"

#ifdef WIN32
#define sleep Sleep
#endif

namespace Lobby {
	
//------------------------------------------------------------------------------

struct GameData : public Game
{
	time_t ttl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

struct GameListData
{
	typedef std::map<Net::Address,GameData> List;
	
	Net::UDPSocket *sock;
	List list;
	pthread_t thread;
};

//------------------------------------------------------------------------------

GameList::GameList(unsigned int port)
{
	data = (void *) new GameListData;
	if (!data)
		return;
	
	GameListData *p = (GameListData *)data;
	
	p->sock = new Net::UDPSocket();
	p->sock->setNonBlocking();
	if (!p->sock->bind(port))
	{
		delete p->sock;
		delete p;
		data = 0;
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&p->thread, &attr, listen, (void *) this))
	{
		delete p->sock;
		delete p;
		data = 0;
		return;
	}
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

GameList::~GameList()
{
	if (!data)
		return;
	
	GameListData *p = (GameListData *) data;
	
	pthread_cancel(p->thread);
	
	void *status;
	pthread_join(p->thread, &status);
	
	delete p->sock;
	delete p;
	data = 0;
}

//------------------------------------------------------------------------------

void *GameList::listen(void *arg)
{
	GameList *gamelist = (GameList *)arg;
	GameListData *p = (GameListData *) gamelist->data;
	
	if (!gamelist)
		pthread_exit(0);
	
	for (;;)
	{
		time_t now = time(NULL);
		time_t min = 0;
		
		GameListData::List::iterator it;
		for (it = p->list.begin(); it != p->list.end(); ++it)
		{
			time_t ttl = it->second.ttl;
			
			if ((ttl <= now) && gamelist->onPart)
				gamelist->onPart(it->first);
			else if ((!min) || (ttl < min))
				min = ttl;
		}
		
		Net::Socket::List read, write, error;
		read.push_back(p->sock);
		
		Net::Socket::select(read, write, error, min);
		
		if (!read.empty())
		{
			Net::Address remote;
			char buf[256];
			size_t len;
			while (p->sock->recvfrom(remote, buf, len = sizeof (buf)))
			{
				Protocol::Message msg(buf);
				if (msg.size() < 4)
					continue;
				
				if ((std::string) msg[0] != "GOTO")
					continue;
				
				GameData game;
				game.numPlayers = (int) msg[2];
				game.name = (std::string) msg[3];
				game.ttl = now + LOBBY_BC_TIMEOUT;
				
				if (!p->list.count(remote) && gamelist->onJoin)
					gamelist->onJoin(remote, game);
				else if (((p->list[remote].name != game.name)
				     || (p->list[remote].numPlayers != game.numPlayers))
				     && gamelist->onChange)
					gamelist->onChange(remote, game);
				
				p->list[remote] = game;
			}
		}
	}
	
	pthread_exit(0);
}

//------------------------------------------------------------------------------

} // namespace Lobby

//------------------------------------------------------------------------------
