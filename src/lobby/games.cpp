/*
 * Games module -- see header file
 */

#include <time.h>
#include <pthread.h>

#include <map>

#include "games.h"

#ifdef WIN32
#define sleep Sleep
#endif

static void *GameList_thread(void *arg);

namespace Lobby {
	
//------------------------------------------------------------------------------

struct GameListData
{
	typedef std::map<Net::Address,Game> List;
	
	Net::UDPSocket *sock;
	List list;
	pthread_t *thread;
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
		delete p;
		p = 0;
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(p->thread, &attr, listen, (void *) this))
	{
		delete p;
		p = 0;
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
	
	pthread_cancel(*p->thread);
	
	void *status;
	pthread_join(*p->thread, &status);
	
	delete p;
	p = 0;
}

//------------------------------------------------------------------------------

void *GameList::listen(void *arg)
{
	GameList *gamelist = (GameList *)arg;
	GameListData *p = (GameListData *) gamelist->data;
	
	if (!gamelist)
		pthread_exit(0);
	
	time_t now = time(0);
	char buffer[256];
	int ret = 1;
	
	for (;;)
	{
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		
		// Process incoming server notifications
		for (;;)
		{
			Net::Address remote;
			size_t length;
			if ((ret = p->sock->recvfrom(remote, buffer, length = sizeof (buffer))) == -1)
				break;
			
			Game game;
			game.numPlayers = *buffer;
			game.name = std::string(buffer + 1);
			game.ttl = now + LOBBY_BC_TIMEOUT;
			
			if (p->list.count(remote))
			{
				if (gamelist->onJoin)
					gamelist->onJoin(remote, game);
			}
			else if (p->list[remote].numPlayers != game.numPlayers)
			{
				if (gamelist->onChange)
					gamelist->onChange(remote, game.numPlayers);
			}
			
			p->list[remote] = game;
		}
		
		// Remove servers that had a timeout
		for (GameListData::List::iterator it = p->list.begin(); it != p->list.end(); ++it)
		{
			if (it->second.ttl < now)
			{
				if (gamelist->onPart)
					gamelist->onPart(it->first);
				p->list.erase(it);
			}
		}
		
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		
		sleep(LOBBY_POLL_INTERVAL);
	}
	
	pthread_exit(0);
}

//------------------------------------------------------------------------------

} // namespace Lobby

//------------------------------------------------------------------------------
