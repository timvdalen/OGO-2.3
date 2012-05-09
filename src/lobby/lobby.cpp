/*
 * Lobby module -- see header file
 */

#include <pthread.h>

#include <map>

#include "lobby.h"

static void *ClientLobby_thread(void *arg);
static void *ServerLobby_thread(void *arg);
static void *ServerLobby_broadcast(void *arg);

//------------------------------------------------------------------------------

struct GameLobbyData
{
	//Net::TCPStringSocket sock;
	pthread_t thread;
};

//------------------------------------------------------------------------------

struct ServerPlayer : public Player
{
	Net::TCPStringSocket sock;
};

//------------------------------------------------------------------------------

struct ServerLobbyData : public GameLobbyData
{
	typedef std::map<Net::Address,ServerPlayer> List;
	List list;
	std::string gameName;
	pthread_t broadcaster;
};

//------------------------------------------------------------------------------

ClientLobby::ClientLobby(std::string playerName, const Net::Address &server)
{
	data = (void *) new GameLobbyData;
	GameLobbyData *lobby = (GameLobbyData *)data;
	
	if (!lobby)
		return;
	
	if (!lobby->sock->connect(server))
	{
		delete lobby;
		lobby = 0;
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&lobby->thread, &attr, ClientLobby_thread, (void *) this))
	{
		delete lobby;
		lobby = 0;
		return;
	}
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

ClientLobby::~ClientLobby()
{
	if (!data)
		return;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	pthread_cancel(lobby->thread, 0);
	
	void *status;
	pthread_join(lobby->thread, &status);
	
	delete lobby;
	lobby = 0;
}

//------------------------------------------------------------------------------

bool ClientLobby::chat(const std::string &line)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	std::string msg = "M";
	return lobby->sock->send(msg + line);
}

//------------------------------------------------------------------------------

bool ClientLobby::ready(bool ready)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	std::string msg = "R";
	return lobby->sock->send(msg + (ready ? '1' : '0'));
}

//------------------------------------------------------------------------------

static void *ClientLobby_thread(void *arg)
{
	ClientLobby *clientlobby = (ClientLobby *)arg;
	ClientLobbyData *lobby = (ClientLobbyData *)clientlobby->data;
	
	if (!lobby)
		pthread_exit(0);
	
	buffer
	for (;;)
	{
		std::string msg = lobby->sock->recv();
		switch (msg[0])
		{
			case 'M':
				if (onChat)
					onChat();
				break;
		}
	}
}

//------------------------------------------------------------------------------

ServerLobby::ServerLobby(std::string gameName, std::string playerName, unsigned int port)
{
}

//------------------------------------------------------------------------------

ServerLobby::~ServerLobby()
{
}

//------------------------------------------------------------------------------

ServerLobby::start()
{
}

//------------------------------------------------------------------------------

static void *ServerLobby_thread(void *arg)
{
}

//------------------------------------------------------------------------------

static void *ServerLobby_broadcast(void *arg)
{
}

//------------------------------------------------------------------------------
