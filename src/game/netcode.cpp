/*
 * Network code module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>

#include "common.h"
#include "netalg.h"
#include "netcode.h"
#include "video.h"
#include "game.h"

#ifndef MAX
	#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define CONNECTED (tokenring && tokenring->connected())

namespace NetCode {

using namespace std;
using namespace Game;

string MessageOfTheDay;

TokenRing *tokenring = NULL;
uword port = GAME_PORT;
FPS cps;

map<NodeID,Player::Id> nodes;
NodeID lastNode = 0;
NodeID findNode(Player::Id pid);
void Join(NodeID nid, Player::Id pid, unsigned char team, string name);

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
#define SENDTO(id,msg,rel) {if (tokenring) tokenring->sendto(id,msg,rel);}

//------------------------------------------------------------------------------

NodeID findNode(Player::Id pid)
{
	map<NodeID,Player::Id>::iterator it;
	for (it = nodes.begin(); it != nodes.end(); ++it)
		if (it->second == pid)
			return it->first;
	return 0;
}

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
	nodes[tokenring->id()] = 1;
	
	if (host.empty())   // Server
		Game::Notice(string("Listening on port ") + Argument(port).str + string("..."));
	else Connect(host); //Client
	
	// Debug:
	Game::game.player->name += Argument(port).str;
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
	{
		//printf("%d %s\n", id, string(msg).c_str());
		if (Receive::list.count(msg[0]))
			Receive::list[msg[0]](id, msg, reliable);
	}
	
	while (tokenring->loss(id))
	{
		// A player disconnected:
		// Assign all owned buildings to a different player
		// Check win condition: no players left for one team
	}
	
	while (tokenring->entry(id))
	{
		lastNode = id;
		// A potential player entered
		// We do nothing and wait till he send an (Re)Enter request
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
	nodes.clear();
	if (tokenring) delete tokenring;
	tokenring = new TokenRing(port);
	Game::Notice(string("Disconnected: listening on port ") + Argument(port).str + string("..."));
}

//------------------------------------------------------------------------------

bool Connect(std::string host)
{
	nodes.clear();
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

void Enter(unsigned char team, string name)
{
	Message msg;
	msg.push_back("ENTER");
	msg.push_back((long) team);
	msg.push_back(name);
	SENDTO(lastNode, msg, false);
}
RECEIVE(ENTER, id, msg, reliable)
{
	if (!tokenring->authorized()) return;
	unsigned char team = (unsigned char) (long) msg[1];
	string name = msg[2];
	
	Player::Id pid = game.topId++;
	ObjectHandle player = Player(pid, team, name);
	game.root->children.insert(player);
	game.players[pid] = player;
	nodes[id] = pid;
	
	Welcome(pid);
	
	// Todo: Send game state
	
	// The following is a temporarily way to sync up the playerlist
	Player *p;
	map<Player::Id,ObjectHandle>::iterator it;
	for (it = game.players.begin(); it != game.players.end(); ++it)
	{
		if (!(p = TO(Player,it->second)) || (it->first == pid)) continue;
		
		Message msg;
		msg.push_back("JOIN");
		if (p == game.player)
			msg.push_back((long) tokenring->id());
		else
			msg.push_back((long) findNode(p->id));
		msg.push_back((long) p->id);
		msg.push_back((long) p->team);
		msg.push_back(p->name);
		SENDTO(id, msg, true);
	}
	
	Join(findNode(pid), pid, team, name);
}

//------------------------------------------------------------------------------

void ReEnter(Player::Id pid)
{
	Message msg;
	msg.push_back("REENTER");
	msg.push_back((long) pid);
	SEND(msg, false);
}
RECEIVE(REENTER, id, msg, reliable)
{
	if (!tokenring->authorized()) return;
	Player::Id pid = (long) msg[1];
	if (!game.players.count(pid)) return; // Maybe send an error
	
	// Todo: Implement
}

//------------------------------------------------------------------------------

void Welcome(Player::Id pid)
{
	Message msg;
	msg.push_back("WELCOME");
	msg.push_back((long) pid);
	msg.push_back(VERSION);
	msg.push_back((long) game.players.size());
	msg.push_back(MessageOfTheDay);
	SENDTO(findNode(pid), msg, true);
}
RECEIVE(WELCOME, id, msg, reliable)
{
	if (!reliable) return;
	if (!game.players.count(game.player->id)) return; // This would be bad
	Echo(msg[4]);
	Player::Id pid = (long) msg[1];
	nodes[tokenring->id()] = pid;
	game.players[pid] = game.players[game.player->id];
	game.players.erase(game.player->id);
	game.player->id = pid;
	game.topId = MAX(game.topId,pid) + 1;
}

//------------------------------------------------------------------------------

void Join(NodeID nid, Player::Id pid, unsigned char team, string name)
{
	Message msg;
	msg.push_back("JOIN");
	msg.push_back((long) nid);
	msg.push_back((long) pid);
	msg.push_back((long) team);
	msg.push_back(name);
	SEND(msg, true);
}
RECEIVE(JOIN, id, msg, reliable)
{
	Player::Id pid = (long) msg[2];
	
	if (pid == game.player->id)
	{
		// Connection complete, do stuff
	}
	else
	{
		// Other player joined
		game.topId = MAX(game.topId,pid) + 1;
		ObjectHandle player = Player(pid, (long) msg[3], msg[4]);
		game.root->children.insert(player);
		game.players[pid] = player;
		nodes[(long) msg[1]] = pid;
	}
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
	Player::Id pid = nodes[id];
	if (!game.players.count(pid)) return;
	if (TO(Player,game.players[pid])->team == game.player->team)
		DisplayChatMsg(PLAYER((Player::Id) pid), msg[1]);
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
	
	Player *player = PLAYER(nodes[id]);
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
	Player *player = PLAYER(nodes[id]);
	if (!player) return;
	player->rotation = rotation;
}

//------------------------------------------------------------------------------

void Debug()
{
	if (tokenring) tokenring->debug();
	
	map<NodeID,Player::Id>::iterator it;
	for (it = nodes.begin(); it != nodes.end(); ++it)
		printf("N%d -> #%d\n", it->first, it->second);
}

//------------------------------------------------------------------------------

} // namespace NetCode

//------------------------------------------------------------------------------
