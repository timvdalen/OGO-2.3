/*
 * Lobby module -- see header file
 */

#include <stdio.h>
#include <pthread.h>

#include <set>

#include "protocol.h"
#include "lobby.h"

#define CALL(x) if (x) x

namespace Lobby {

using namespace std;
using namespace Net;
using namespace Protocol;

//------------------------------------------------------------------------------

struct GameLobbyData
{
	MsgSocket *sock;
	pthread_t thread;
};

//------------------------------------------------------------------------------

struct ServerPlayer : public Player
{
	Address addr;
	MsgSocket *sock;
	
	bool operator <(const ServerPlayer &that) const
		{ return this->sock < that.sock; }
};

//------------------------------------------------------------------------------

struct ServerLobbyData : public GameLobbyData
{
	typedef set<ServerPlayer> PlayerSet;
	ServerPlayer *find(MsgSocket *sock) const;
	
	UDPSocket *bcsock;
	unsigned int port;
	Player host;
	PlayerSet players;
	Player::Id uid;
	string name;
	pthread_t thread2;
};

//------------------------------------------------------------------------------

GameLobby::GameLobby() : onConnect(0), onPlayer(0), onJoin(0), onPart(0),
	onTeam(0), onState(0), onChat(0), onClose(0), onStart(0)
{
}

//------------------------------------------------------------------------------

bool GameLobby::team(unsigned char team)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	Message msg;
	msg.push_back("TEAM");
	msg.push_back((int) team);
	return lobby->sock->send(msg);
}

//------------------------------------------------------------------------------


bool GameLobby::chat(const string &line)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	Message msg;
	msg.push_back("CHAT");
	msg.push_back(line);
	return lobby->sock->send(msg);
}

//------------------------------------------------------------------------------

ClientLobby::ClientLobby(string playerName, const Address &server)
{
	data = (void *) new GameLobbyData;
	if (!data)
		return;
	
	GameLobbyData *p = (GameLobbyData *)data;
	
	p->sock = new MsgSocket();
	if (!p->sock->connect(server))
	{
		delete p->sock;
		delete p;
		data = 0;
		return;
	}
	p->sock->setNonBlocking();
	
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

bool ClientLobby::state(bool ready)
{
	if (!data)
		return false;
	
	GameLobbyData *lobby = (GameLobbyData *) data;
	
	Message msg;
	msg.push_back(ready ? "READY" : "BUSY");
	return lobby->sock->send(msg);
}

//------------------------------------------------------------------------------

void *ClientLobby::listen(void *arg)
{
	ClientLobby *lobby = (ClientLobby *)arg;
	if (!lobby)
		pthread_exit(0);
	
	GameLobbyData *p = (GameLobbyData *) lobby->data;
	
	Player::Id client = 0;
	Game game;
	
	Socket::List read, write, error;
	while (p->sock->valid())
	{
		// Wait for incomming messages
		read.clear(), write.clear(), error.clear();
		read.push_back(p->sock);
		Socket::select(read, write, error);
		
		// Process incomming messages
		if (read.empty())
			continue;
		
		Message msg;
		while (!(msg = p->sock->recv()).empty())
		{
			string cmd = msg[0];
			if (cmd == "HELLO") //-----------------------------------
			{
				if (msg.size() < 5)
					continue;
				
				client = (int) msg[1];
				game.numPlayers = (int) msg[2];
				game.name = (string) msg[3];
			}
			else if (cmd == "PLAYER") //-----------------------------
			{
				if (msg.size() < 5)
					continue;
				
				Player player;
				player.id = (int) msg[1];
				player.team = (int) msg[2];
				player.state = (Player::State) (int) msg[3];
				player.name = (string) msg[4];
				CALL(lobby->onPlayer)(player);
			}
			else if (cmd == "JOIN")  //------------------------------
			{
				if (msg.size() < 3)
					continue;
				
				if ((int) msg[1] == client)
					CALL(lobby->onConnect)(client, game);
				else
					CALL(lobby->onJoin)((int) msg[1], msg[2]);
			}
			else if (cmd == "PART") //-------------------------------
			{
				if (msg.size() < 2)
					continue;
				
				CALL(lobby->onPart)((int) msg[1]);
			}
			else if (cmd == "TEAM") //-------------------------------
			{
				if (msg.size() < 3)
					continue;
				
				CALL(lobby->onTeam)((int) msg[1], (int) msg[2]);
			}
			else if ((cmd == "READY") //-----------------------------
			     ||  (cmd ==  "BUSY"))
			{
				if (msg.size() < 2)
					continue;
				
				Player::State state = (cmd == "BUSY") ? Player::stBusy
				                                      : Player::stReady;
				
				CALL(lobby->onState)((int) msg[1], state);
				
			}
			else if (cmd == "CHAT") //-------------------------------
			{
				if (msg.size() < 3)
					continue;
				
				CALL(lobby->onChat)((int) msg[1], msg[2]);
			}
		}
		
		if (msg.eof())
		{
			CALL(lobby->onClose)();
			break;
		}
	}
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

ServerLobby::ServerLobby(string gameName, string playerName, unsigned int port)
{
	data = (void *) new ServerLobbyData;
	if (!data)
		return;
	ServerLobbyData *p = (ServerLobbyData *)data;
	
	p->host.id = 1;
	p->host.team = 0;
	p->host.state = Player::stHost;
	p->host.name = playerName;
	
	p->uid = 2;
	p->port = port;
	p->name = gameName;
	
	p->sock = new MsgSocket();
	if (!p->sock->listen(port))
	{
		delete p->sock;
		delete p;
		data = 0;
		return;
	}
	p->sock->setNonBlocking();
	
	p->bcsock = new UDPSocket();
	p->bcsock->setNonBlocking();
	p->bcsock->broadcast();
	if (!p->bcsock->bind(port))
	{
		delete p->sock;
		delete p->bcsock;
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
		delete p->bcsock;
		delete p;
		data = 0;
		return;
	}
	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&p->thread2, &attr, broadcast, (void *) this))
	{
		delete p->sock;
		delete p->bcsock;
		delete p;
		data = 0;
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
	
	ServerLobbyData::PlayerSet::iterator it;
	for (it = lobby->players.begin(); it != lobby->players.end(); ++it)
		delete it->sock;
	delete lobby->sock;
	delete lobby->bcsock;
	delete lobby;
	data = 0;
}

//------------------------------------------------------------------------------

bool ServerLobby::team(unsigned char team)
{
	if (!data)
		return false;
	
	ServerLobbyData *lobby = (ServerLobbyData *) data;
	
	lobby->host.team = team;
	
	Message msg;
	msg.push_back("TEAM");
	msg.push_back((int) lobby->host.id);
	msg.push_back((int) team);
	
	ServerLobbyData::PlayerSet::iterator pit;
	for (pit = lobby->players.begin(); pit != lobby->players.end(); ++pit)
		if (pit->id)
			pit->sock->send(msg);
	
	CALL(onTeam)((int) lobby->host.id, team);
	return true;
}

//------------------------------------------------------------------------------

bool ServerLobby::chat(const string &line)
{
	if (!data)
		return false;
	
	ServerLobbyData *lobby = (ServerLobbyData *) data;
	
	Message msg;
	msg.push_back("CHAT");
	msg.push_back((int) lobby->host.id);
	msg.push_back(line);
	
	ServerLobbyData::PlayerSet::iterator pit;
	for (pit = lobby->players.begin(); pit != lobby->players.end(); ++pit)
		if (pit->id)
			pit->sock->send(msg);
	
	CALL(onChat)(lobby->host.id, line);
	return true;
}

//------------------------------------------------------------------------------

bool ServerLobby::start()
{
}

//------------------------------------------------------------------------------

void *ServerLobby::listen(void *arg)
{
	ServerLobby *lobby = (ServerLobby *)arg;
	if (!lobby)
		pthread_exit(0);
	
	ServerLobbyData *p = (ServerLobbyData *) lobby->data;
	
	{
		Game game;
		game.numPlayers = p->players.size() + 1;
		game.name = p->name;
		CALL(lobby->onConnect)(p->host.id, game);
	}
	
	Socket::List read, write, error;
	while (p->sock->valid())
	{
		// Wait for incomming connections or messages
		read.clear(), write.clear(), error.clear();
		read.push_back(p->sock);
		{
			ServerLobbyData::PlayerSet::iterator it;
			for (it = p->players.begin(); it != p->players.end(); ++it)
				read.push_back(it->sock);
		}
		Socket::select(read, write, error);
		
		// Process incomming connections or messages
		Socket::List::iterator it;
		ServerLobbyData::PlayerSet::iterator pit;
		for (it = read.begin(); it != read.end(); ++it)
		{
			if (*it == p->sock)
			{
				// Process incomming connections
				MsgSocket *server = (MsgSocket *) *it;
				
				TCPSocket client;
				Address remote;
				if (!server->accept(client, remote))
					continue;
				
				ServerPlayer player;
				player.sock = new MsgSocket(client);
				player.sock->setNonBlocking();
				player.addr = remote;
				player.id = 0;
				p->players.insert(player);
			}
			else
			{
				// Process incomming messages
				MsgSocket *sock = (MsgSocket *) *it;
				ServerPlayer *player = p->find(sock);
				if (!player)
					continue;
				
				Message msg;
				while (!(msg = sock->recv()).empty())
				{
					string cmd = msg[0];
					if (cmd == "NAME") //----------------------------
					{
						if (player->id)
							continue;
						
						player->id = p->uid++;
						player->team = 0;
						player->state = Player::stBusy;
						player->name = (string) msg[1];
						
						msg.clear();
						msg.push_back("HELLO");
						msg.push_back((int) player->id);
						msg.push_back(VERSION);
						msg.push_back((int) p->players.size() + 1);
						msg.push_back(p->name);
						sock->send(msg);
						
						Message msg_join;
						msg_join.push_back("JOIN");
						msg_join.push_back((int) player->id);
						msg_join.push_back(player->name);
						
						
						{
							Message msg_player;
							msg_player.push_back("PLAYER");
							msg_player.push_back((int) p->host.id);
							msg_player.push_back(p->host.team);
							msg_player.push_back(p->host.state);
							msg_player.push_back(p->host.name);
							sock->send(msg_player);
						}
						for (pit = p->players.begin(); pit != p->players.end(); ++pit)
						{
							if ((!pit->id) || (pit->sock == sock))
								continue;
							
							Message msg_player;
							msg_player.push_back("PLAYER");
							msg_player.push_back((int) pit->id);
							msg_player.push_back(pit->team);
							msg_player.push_back(pit->state);
							msg_player.push_back(pit->name);
							sock->send(msg_player);
							
							pit->sock->send(msg_join);
						}
						sock->send(msg_join);
						CALL(lobby->onJoin)(player->id, player->name);
					}
					else if (!player->id) //-------------------------
					{
						// Do nothing
					}
					else if (cmd == "TEAM") //-----------------------
					{
						if (msg.size() != 2)
							continue;
						
						unsigned char team = (int) msg[1];
						if (player->team == team)
							continue;
						
						player->team = team;
						msg[1] = (int) team;
						msg.insert(msg.begin() + 1, (int) player->id);
						
						for (pit = p->players.begin(); pit != p->players.end(); ++pit)
							if (pit->id)
								pit->sock->send(msg);
						
						CALL(lobby->onTeam)(player->id, team);
					}
					else if ((cmd == "READY") //---------------------
					     ||  (cmd ==  "BUSY"))
					{
						if (msg.size() != 1)
							continue;
						
						Player::State state = (cmd == "BUSY") ? Player::stBusy
						                                      : Player::stReady;
						
						if (player->state == state)
							continue;
						
						player->state = state;
						msg.push_back((int) player->id);
						
						for (pit = p->players.begin(); pit != p->players.end(); ++pit)
							if (pit->id)
								pit->sock->send(msg);
						
						CALL(lobby->onState)(player->id, state);
					}
					else if (cmd == "CHAT") //-----------------------
					{
						if (msg.size() < 2)
							continue;
						
						msg.insert(msg.begin() + 1, (int) player->id);
						
						for (pit = p->players.begin(); pit != p->players.end(); ++pit)
							if (pit->id)
								pit->sock->send(msg);
						
						CALL(lobby->onChat)(player->id, msg[2]);
					}
				}
				
				if (msg.eof())
				{
					Message msg_part;
					msg_part.push_back("PART");
					msg_part.push_back((int) player->id);
					
					p->players.erase(p->players.find(*player));
					delete player->sock;
					//delete player; TODO: find out where the dependency is
					
					for (pit = p->players.begin(); pit != p->players.end(); ++pit)
						if (pit->id)
							pit->sock->send(msg_part);
					
					CALL(lobby->onPart)(player->id);
				}
			}
		}
	}
	
	CALL(lobby->onClose)();
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

void *ServerLobby::broadcast(void *arg)
{
	ServerLobby *lobby = (ServerLobby *)arg;
	if (!lobby)
		pthread_exit(0);
	
	ServerLobbyData *p = (ServerLobbyData *) lobby->data;
	
	for (;;)
	{
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		
		Message msg;
		msg.push_back("GOTO");
		msg.push_back(VERSION);
		msg.push_back((long) p->players.size() + 1);
		msg.push_back(p->name);
		
		string str = msg;
		size_t length;
		p->bcsock->shout(p->port, str.c_str(), length = str.length());
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		
		sleep(LOBBY_BC_INTERVAL * 1000);
	}
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

ServerPlayer *ServerLobbyData::find(MsgSocket *sock) const
{
	ServerPlayer player;
	player.sock = sock;
	ServerLobbyData::PlayerSet::iterator it = players.find(player);
	if (it == players.end())
		return (NULL);
	else
		return const_cast<ServerPlayer *>(&(*it));
}

//------------------------------------------------------------------------------

} // namespace Lobby

//------------------------------------------------------------------------------
