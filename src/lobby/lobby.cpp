/*
 * Lobby module -- see header file
 */

#include <pthread.h>

#include <map>

#include "protocol.h"
#include "lobby.h"

namespace Lobby {

//------------------------------------------------------------------------------

struct GameLobbyData
{
	Protocol::MsgSocket *sock;
	pthread_t thread;
};

//------------------------------------------------------------------------------

struct ServerPlayer : public Player
{
	Protocol::MsgSocket *sock;
};

//------------------------------------------------------------------------------

struct ServerLobbyData : public GameLobbyData
{
	Net::UDPSocket *bcsock;
	typedef std::map<Net::Address,ServerPlayer> List;
	unsigned int port;
	List list;
	std::string gameName;
	pthread_t thread2;
};

//------------------------------------------------------------------------------

ClientLobby::ClientLobby(std::string playerName, const Net::Address &server)
{
	data = (void *) new GameLobbyData;
	if (!data)
		return;
	
	GameLobbyData *p = (GameLobbyData *)data;
	
	p->sock = new Protocol::MsgSocket();
	if (!p->sock->connect(server))
	{
		delete p->sock;
		delete p;
		p = 0;
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&p->thread, &attr, listen, (void *) this))
	{
		delete p->sock;
		delete p;
		p = 0;
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
	
	pthread_cancel(lobby->thread);
	
	void *status;
	pthread_join(lobby->thread, &status);
	
	delete lobby->sock;
	delete lobby;
	lobby = 0;
}

//------------------------------------------------------------------------------

bool GameLobby::chat(const std::string &line)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	Protocol::Message msg;
	msg.push_back("SAY");
	msg.push_back(line);
	return lobby->sock->send(msg);
}

//------------------------------------------------------------------------------

bool GameLobby::ready(bool ready)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	Protocol::Message msg;
	msg.push_back(ready ? "READY" : "BUSY");
	return lobby->sock->send(msg);
}

//------------------------------------------------------------------------------

void *ClientLobby::listen(void *arg)
{
}

//------------------------------------------------------------------------------

ServerLobby::ServerLobby(std::string gameName, std::string playerName, unsigned int port)
{
	data = (void *) new ServerLobbyData;
	if (!data)
		return;
	
	ServerLobbyData *p = (ServerLobbyData *)data;
	
	p->port = port;
	p->gameName = gameName;
	
	p->sock = new Protocol::MsgSocket();
	if (!p->sock->listen(port))
	{
		delete p->sock;
		delete p;
		p = 0;
		return;
	}
	
	p->bcsock = new Net::UDPSocket();
	p->bcsock->setNonBlocking();
	p->bcsock->broadcast();
	if (!p->bcsock->bind(port))
	{
		delete p->sock;
		delete p->bcsock;
		delete p;
		p = 0;
		return;
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&p->thread, &attr, listen, (void *) this))
	{
		delete p->sock;
		delete p->bcsock;
		delete p;
		p = 0;
		return;
	}
	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&p->thread2, &attr, broadcast, (void *) this))
	{
		delete p->sock;
		delete p->bcsock;
		delete p;
		p = 0;
		return;
	}
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

ServerLobby::~ServerLobby()
{
	if (!data)
		return;
	
	ServerLobbyData *lobby = (ServerLobbyData *) data;
	
	pthread_cancel(lobby->thread);
	pthread_cancel(lobby->thread2);
	
	void *status;
	pthread_join(lobby->thread, &status);
	pthread_join(lobby->thread2, &status);
	
	delete lobby->sock;
	delete lobby->bcsock;
	delete lobby;
	lobby = 0;
}

//------------------------------------------------------------------------------

bool ServerLobby::start()
{
}

//------------------------------------------------------------------------------

void *ServerLobby::listen(void *arg)
{
}

//------------------------------------------------------------------------------

void *ServerLobby::broadcast(void *arg)
{
	ServerLobby *lobby = (ServerLobby *)arg;
	ServerLobbyData *p = (ServerLobbyData *) lobby->data;
	
	if (!lobby)
		pthread_exit(0);
	
	for (;;)
	{
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		Protocol::Message msg;
		msg.push_back("GOTO");
		msg.push_back(VERSION);
		msg.push_back((long) p->list.size());
		msg.push_back(p->gameName);
		std::string str = msg;
		size_t length;
		p->bcsock->shout(p->port, str.c_str(), length = str.length());
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		
		sleep(LOBBY_BC_INTERVAL * 1000);
	}
	
	pthread_exit(0);
}

//------------------------------------------------------------------------------

} // namespace Lobby

//------------------------------------------------------------------------------
