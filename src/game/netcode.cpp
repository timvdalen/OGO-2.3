/*
 * Network code module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#inlcude <limits.h>

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

void TeamInfo(NodeID nid);
void PlayerInfo(NodeID nid);
void StructInfo(NodeID nid);
void ItemInfo(NodeID nid);

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

int loop = 0;

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
		Player::Id pid = nodes[id];
		if (!game.players.count(pid)) return;
		ObjectHandle player = game.players[pid];
		game.world->children.erase(player);
		game.players.erase(pid);
	}
	
	while (tokenring->entry(id))
	{
		lastNode = id;
		// A potential player entered
		// We do nothing and wait till he send an (Re)Enter request
	}
	
	if (game.connecting)
	{
		if (loop > 80)
		{
			NetCode::Enter(game.player->team, game.player->name);
			loop = 0;
		}
		else
			loop++;
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
	
	// Send game state
	TeamInfo(id);
	PlayerInfo(id);
	StructInfo(id);
	ItemInfo(id);
	
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
	game.connecting = false;
	Echo(msg[4]);
	Player::Id pid = (long) msg[1];
	nodes[tokenring->id()] = pid;
	game.players[pid] = game.players[game.player->id];
	game.players.erase(game.player->id);
	game.player->id = pid;
	game.topId = MAX(game.topId,pid) + 1;
}

//------------------------------------------------------------------------------

void TeamInfo(NodeID nid)
{
	Message msg;
	msg.push_back("TEAMINFO");
	map<unsigned char,Objects::Team>::iterator it;
	for (it = game.teams.begin(); it != game.teams.end(); ++it)
	{
		msg.push_back((long) it->first);
		msg.push_back((double) it->second.resources);
	}
	SENDTO(nid, msg, true);
}
RECEIVE(TEAMINFO, id, msg, reliable)
{
	if (!reliable) return;
	for (size_t i = 1; i < msg.size(); i += 2)
		game.teams[(long) msg[i]].resources = (double) msg[i+1];
}

//------------------------------------------------------------------------------

void PlayerInfo(NodeID nid)
{
	Message msg;
	msg.push_back("PLAYERINFO");
	map<Player::Id,ObjectHandle>::iterator it;
	for (it = game.players.begin(); it != game.players.end(); ++it)
	{
		if (it->first == nodes[nid]) continue; // Don't send player his own info
		if (it->first == game.player->id)
			msg.push_back((long) tokenring->id());
		else
			msg.push_back((long) findNode(it->first));
		
		msg.push_back((string) *it->second);
	}
	SENDTO(nid, msg, true);
}
RECEIVE(PLAYERINFO, id, msg, reliable)
{
	if (!reliable) return;
	for (size_t i = 1; i < msg.size(); i += 2)
	{
		ObjectHandle player = Player();
		player->unserialize((string) msg[i+1]);
		Player *p = TO(Player,player);
		Player::Id pid = p->id;
		game.topId = MAX(game.topId,pid) + 1;
		game.root->children.insert(player);
		game.players[pid] = player;
		nodes[(long) msg[i]] = pid;
		p->updateTextures();
	}
}

//------------------------------------------------------------------------------

void StructInfo(NodeID nid)
{
	Terrain *t = TO(Terrain,game.world->terrain);
	Message msg;
	msg.push_back("STRUCTINFO");
	map<GridPoint,ObjectHandle>::iterator it;
	for (it = t->structures.begin(); it != t->structures.end(); ++it)
	{
		msg.push_back(convert(it->first));
		msg.push_back((string) *it->second);
	}
	SENDTO(nid, msg, true);
}
RECEIVE(STRUCTINFO, id, msg, reliable)
{
	if (!reliable) return;
	
	Terrain *t = TO(Terrain,game.world->terrain);
	t->structures.clear();

	for (size_t i = 1; i < msg.size(); i += 2)
	{
		GridPoint g = ToGridPoint(msg[i]);
		if (!g.isValid()) continue;
		ObjectHandle structure = Object::construct((string) msg[i+1]);
		if (!structure) continue;
		structure->unserialize(msg[i+1]);
		t->structures[g] = structure;
		TO(Structure,structure)->updateTextures();
	}
}

//------------------------------------------------------------------------------

void ItemInfo(NodeID nid)
{
	Message msg;
	msg.push_back("ITEMINFO");
	vector<ObjectHandle>::iterator it;
	for (it = game.world->temporary.begin(); it != game.world->temporary.end(); ++it)
	{
		Droppable *d = TO(Droppable, *it);
		if (!d) continue;
		msg.push_back((string) *d);
	}
	SENDTO(nid, msg, true)
}
RECEIVE(ITEMINFO, id, msg, reliable)
{
	if (!reliable) return;
	game.world->temporary.clear();
	for (size_t i = 1; i < msg.size(); ++i)
	{
		ObjectHandle item = Object::construct((string) msg[i]);
		if (!item) continue;
		item->unserialize(msg[i]);
		game.world->temporary.push_back(item);
	}
}

//------------------------------------------------------------------------------

void Sync(Player::Id pid)
{
	NodeID nid = nodes[pid];
	TeamInfo(nid);
	PlayerInfo(nid);
	StructInfo(nid);
	ItemInfo(nid);
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
	unsigned char team = (long) msg[3];
	
	if (pid == game.player->id)
	{
		// Connection complete, do stuff
	}
	else
	{
		// Other player joined
		game.topId = MAX(game.topId,pid) + 1;
		ObjectHandle player = Player(pid, team, msg[4]);
		game.root->children.insert(player);
		game.players[pid] = player;
		nodes[(long) msg[1]] = pid;
	}
	
	long redID = INT_MAX - 'a';
	long blueID = INT_MAX - 'b';
	
	if (game.players.count(redID) && (team == 'a'))
		game.world->terrain->Reassign(redID, pid);
	
	if (game.players.count(blueID) && (team == 'b'))
		game.world->terrain->Reassign(blueID, pid);
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

void Team(unsigned char team, Resource gold)
{
	Message msg;
	msg.push_back("TEAM");
	msg.push_back((long) team);
	msg.push_back((double) gold);
	SEND(msg, false);
}
RECEIVE(TEAM, id, msg, reliable)
{
	unsigned char team = (long) msg[1];
	game.teams[team].resources += (double) msg[2];
	game.teams[team].resources /= 2.0;
}

//------------------------------------------------------------------------------

void Fire(const LaserBeam &laser)
{
	Message msg;
	msg.push_back("FIRE");
	msg.push_back((string) laser);
	SEND(msg, false);
}
RECEIVE(FIRE, id, msg, reliable)
{
	ObjectHandle laser = LaserBeam();
	laser->unserialize(msg[1]);
	game.world->addLaserBeam(laser);
}

//------------------------------------------------------------------------------

void Hit(Player::Id pid, double damage, bool self)
{
	Message msg;
	msg.push_back("HIT");
	msg.push_back((long) pid);
	msg.push_back((double) damage);
	msg.push_back((long) (self ? 1 : 0));
	SEND(msg, true);
}
RECEIVE(HIT, id, msg, reliable)
{
	Player::Id pid = (long) msg[1];
	bool self = (long) msg[3];
	if (!game.players.count(pid)) return;
	TO(Player,game.players[pid])->damage((double) msg[2], self ? nodes[id] : 0);
}

//------------------------------------------------------------------------------

void Died(Player::Id pid)
{
	Message msg;
	msg.push_back("DIED");
	msg.push_back((long) pid);
	SEND(msg, false);
}
RECEIVE(DIED, id, msg, reliable)
{
	Player::Id vid = nodes[id];
	Player::Id pid = (long) msg[1];
	if (!game.players.count(pid))
		DisplayFragMsg(0, TO(Player,game.players[vid]));
	else
		DisplayFragMsg(TO(Player,game.players[pid]), TO(Player,game.players[vid]));
}

//------------------------------------------------------------------------------

void Build(GridPoint g, Structure *s)
{
	Message msg;
	msg.push_back("BUILD");
	msg.push_back(convert(g));
	msg.push_back((string) *s);
	SEND(msg, true);
}
RECEIVE(BUILD, id, msg, reliable)
{
	if (!reliable) return;
	GridPoint g = ToGridPoint(msg[1]);
	if (!g.isValid()) return;
	ObjectHandle structure = Object::construct((string) msg[2]);
	if (!structure) return;
	structure->unserialize(msg[2]);
	game.world->terrain->structures[g] = structure;
	TO(Structure,structure)->updateTextures();
}

//------------------------------------------------------------------------------

void Take(unsigned long id)
{
	Message msg;
	msg.push_back("TAKE");
	msg.push_back((long) id);
	SEND(msg, true);
}
RECEIVE(TAKE, id, msg, reliable)
{
	if (!reliable) return;
	unsigned long iid = (long) msg[1];
	
	vector<ObjectHandle>::iterator it;
	for (it = game.world->temporary.begin(); it != game.world->temporary.end(); ++it)
	{
		Droppable *d = TO(Droppable, *it);
		if (!d) continue;
		if (d->id == iid)
		{
			if (!game.players.count(nodes[id])) continue;
			Player *p = TO(Player,game.players[nodes[id]]);
			if (!p) continue;
			game.teams[p->team].resources += d->worth;
			game.world->temporary.erase(it);
			break;
		}
	}
}

//------------------------------------------------------------------------------

void Drop(Droppable *s)
{
	Message msg;
	msg.push_back("DROP");
	msg.push_back((string) *s);
	SEND(msg, true);
}
RECEIVE(DROP, id, msg, reliable)
{
	ObjectHandle item = Object::construct((string) msg[1]);
	if (!item) return;
	item->unserialize(msg[1]);
	game.world->temporary.push_back(item);
}

//------------------------------------------------------------------------------

void Attack(GridPoint g, double damage, bool self)
{
	Message msg;
	msg.push_back("ATTACK");
	msg.push_back(convert(g));
	msg.push_back((double) damage);
	msg.push_back((long) (self ? 1 : 0));
	SEND(msg, true);
}
RECEIVE(ATTACK, id, msg, reliable)
{
	GridPoint g = ToGridPoint(msg[1]);
	if (!g.isValid()) return;
	bool self = (long) msg[3];
	Building *b = TO(Building,game.world->terrain->structures[g]);
	if (!b) return;
	b->damage((double) msg[2], self ? nodes[id] : 0);
}

//------------------------------------------------------------------------------

void Destroy(GridPoint g, Player::Id pid)
{
	Message msg;
	msg.push_back("DESTROY");
	msg.push_back(convert(g));
	msg.push_back((long) pid);
	SEND(msg, false);
}
RECEIVE(DESTROY, id, msg, reliable)
{
	GridPoint g = ToGridPoint(msg[1]);
	Player::Id pid = (long) msg[2];
	// We get the notification a building was destroyed.
	// What do we do here?
}

//------------------------------------------------------------------------------

void End(unsigned char team)
{
	Message msg;
	msg.push_back("END");
	msg.push_back((long) team);
	SEND(msg, true);
}
RECEIVE(END, id, msg, reliable)
{
	if (!reliable) return;
	unsigned char team = (long) msg[1];
	// Fireworks?
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
