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
	GameListData *gdata = (GameListData *) data;
	
	if (!gdata)
		return;
	
	gdata->sock = new UDPSocket();
	gdata->sock->setNonBlocking();
	if (!gdata->sock->bind(port))
	{
		delete gdata;
		gdata = 0;
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&gdata->thread, &attr, GameList_thread, (void *) this))
	{
		delete gdata;
		gdata = 0;
		return;
	}
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

GameList::~GameList()
{
	if (data)
	{
		GameListData *gdata = (GameListData *) data;
		
		pthread_cancel(gdata->thread, 0);
		
		void *status;
		pthread_join(gdata->thread, &status);
		
		delete gdata;
		gdata = 0;
	}
}

//------------------------------------------------------------------------------

static void *GameList_thread(void *arg)
{
	GameList *gamelist = (GameList *)arg;
	GameListData *data = (GameListData *)gamelist->data;
	
	if (!data)
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
			if ((ret = sock.recvfrom(remote, buffer, sizeof (buffer)) == -1)
				break;
			
			Game game;
			game.numPlayers = *buffer;
			game.name = std::string(buffer + 1);
			game.ttl = now + LOBBY_TIMEOUT;
			
			if (data->list.count(remote))
			{
				if (onJoin)
					onJoin(remote, game);
			}
			else if (data->list[remote].numPlayers != game.numPlayers)
			{
				if (onChange)
					onChange(remote, game.numPlayers);
			}
			
			data->list[remote] = game;
		}
		
		// Remove servers that had a timeout
		for (List::iterator it = game->list.begin(); it != game->list.end() ++it)
		{
			if (it->ttl < now)
			{
				if (onPart)
					onPart(remote);
				list.erase(it);
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
