/*
 * Network code module
 */

#include <stdlib.h>
#include <string.h>

#include <map>

#include "common.h"
#include "netalg.h"
#include "netcode.h"
#include "video.h"
#include "game.h"

#define CONNECTED (tokenring && tokenring->connected())

namespace NetCode {

using namespace std;
using namespace Game;

TokenRing *tokenring = NULL;
uword port = GAME_PORT;
FPS cps;

//------------------------------------------------------------------------------

struct Receive
{
	typedef void(*Func) (NodeID, Message &, bool);
	typedef map<string,Func> List;
	
	static List list;
	
	Receive(string cmd, Func func) { list[cmd] = func; }
};

Receive::List Receive::list;

#define RECEIVE(cmd, id, m, r)                   \
	void _ ## cmd(NodeID, Message &, bool);      \
	Receive __ ## cmd(#cmd, _ ## cmd);           \
	void _ ## cmd(NodeID id, Message &m, bool r)

#define SEND(msg,rel) {if (tokenring) tokenring->shout(msg,rel);}

//==============================================================================

void Initialize(int argc, char *argv[])
{
	string host;
	
	for (int i = 0; i < argc - 1; ++i)
	{
		if      (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--connect"))
			host = argv[++i];
		else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "--port"))
			port = atoi(argv[++i]);
	}
	
	tokenring = new TokenRing(port);
	
	if (host.empty())   // Server
		Game::Notice(string("Listening on port ") + Argument(port).str + string("..."));
	else Connect(host); //Client
}

//------------------------------------------------------------------------------

void Terminate()
{
	if (tokenring) delete tokenring;
	tokenring = NULL;
}

//------------------------------------------------------------------------------

void Frame()
{
	if (!CONNECTED) return;
	
	NodeID id;
	Message msg;
	bool reliable;
	
	while (tokenring->recvfrom(id, msg, reliable))
		if (Receive::list.count(msg[0]))
			Receive::list[msg[0]](id, msg, reliable);
	
	while (tokenring->loss(id))
	{
		// Temporarily remove a player from the world
		if (game.players.count(id))
		{
			game.world->children.erase(game.players[id]);
			game.players.erase(id);
		}
	}
	
	while (tokenring->entry(id))
	{
		// Temporarily add a player to the world
		ObjectHandle player = Player(id);
		game.players[id] = player;
		game.world->children.insert(player);
	}
}

//------------------------------------------------------------------------------

bool TryLock()
{
	if (!tokenring) return false;
	return tokenring->authorized();
}

//------------------------------------------------------------------------------

void Unlock()
{
	if (tokenring && tokenring->pass()) cps();
}

//==============================================================================

bool Connected()
{
	return CONNECTED;
}

//------------------------------------------------------------------------------

bool Reliable()
{
	if (!tokenring) return false;
	return tokenring->tainted();
}

//------------------------------------------------------------------------------

double CurrentCPS()
{
	return cps;
}

//------------------------------------------------------------------------------

void Disconnect()
{
	if (tokenring) delete tokenring;
	tokenring = new TokenRing(port);
	Game::Notice(string("Disconnected: listening on port ") + Argument(port).str + string("..."));
}

//------------------------------------------------------------------------------

bool Connect(std::string host)
{
	Address remote(host.c_str());
	return tokenring->connect(remote, 10);
}

//==============================================================================

bool Send(const Message &msg, bool reliable)
{
	if (!CONNECTED) return false;
	return tokenring->shout(msg, reliable);
}

//------------------------------------------------------------------------------

void Chat(string line)
{
	Message msg;
	msg.push_back("CHAT");
	msg.push_back(line);
	SEND(msg, false);
}
RECEIVE(CHAT, id, msg, reliable)
{
	DisplayChatMsg(PLAYER((Player::Id) id), msg[1]);
}

//------------------------------------------------------------------------------

void TeamChat(string line)
{
	Message msg;
	msg.push_back("TEAMCHAT");
	msg.push_back(line);
	SEND(msg, false);
}
RECEIVE(TEAMCHAT, id, msg, reliable)
{
	if (!game.players.count(id)) return;
	if (TO(Player,game.players[id])->team == game.player->team)
		DisplayChatMsg(PLAYER((Player::Id) id), msg[1]);
}

//------------------------------------------------------------------------------

void Move(Pd position, Vd velocity)
{
	if (!game.player) return;
	
	Message msg;
	msg.push_back("MOVE");
	msg.push_back(position.x);
	msg.push_back(position.y);
	msg.push_back(position.z);
	msg.push_back(velocity.x);
	msg.push_back(velocity.y);
	msg.push_back(velocity.z);
	SEND(msg, false);
}
RECEIVE(MOVE, id, msg, reliable)
{
	Pd position((double) msg[1], (double) msg[2], (double) msg[3]);
	Vd velocity((double) msg[4], (double) msg[5], (double) msg[6]);
	
	Player *player = PLAYER(id);
	if (!player) return;
	player->velocity = velocity;
	player->origin = position;
}

//------------------------------------------------------------------------------

void Look(Qd rotation)
{
	Message msg;
	msg.push_back("LOOK");
	msg.push_back(rotation.a);
	msg.push_back(rotation.b);
	msg.push_back(rotation.c);
	msg.push_back(rotation.d);
	SEND(msg, false);
}
RECEIVE(LOOK, id, msg, reliable)
{
	Qd rotation((double) msg[1], (double) msg[2], (double) msg[3], (double) msg[4]);
	Player *player = PLAYER(id);
	if (!player) return;
	player->rotation = rotation;
}

//------------------------------------------------------------------------------

void Debug()
{
	if (tokenring) tokenring->debug();
}

//------------------------------------------------------------------------------

} // namespace NetCode

//------------------------------------------------------------------------------
