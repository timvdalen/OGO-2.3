/*
 * Lobby main file
 *
 * Authors: Ogo 2.3 Group 3 2012
 * 
 * Date: 26-04-12 12:11
 *
 * Description: Player accumulation and social interaction prior to a game.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>

#include "net.h"
#include "protocol.h"
#include "games.h"
#include "lobby.h"

using namespace Lobby;

std::string playerName, gameName;

//------------------------------------------------------------------------------

std::map<Net::Address,Game> gamelist;

void gamelist_update();
void gamelist_join(Net::Address server, Game game);
void gamelist_change(Net::Address server, Game game);
void gamelist_part(Net::Address server);

//------------------------------------------------------------------------------

std::map<Lobby::Player::Id, std::string> playerlist;

void lobby_connect(Player::Id pid, Game game);
void lobby_player(Player player);
void lobby_join(Player::Id pid, std::string playerName);
void lobby_part(Player::Id pid);
void lobby_team(Player::Id pid, unsigned char team);
void lobby_state(Player::Id pid, Player::State state);
void lobby_chat(Player::Id pid, std::string line);
void lobby_close();
void lobby_start();

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	{
	Net::Initialize();
	
	char buffer[512];
	int choice;
	
	printf("player name> ");
	playerName = gets(buffer);
	
	printf("\t1) server\n\t2) client\n> ");
	scanf("%d", &choice);
	
	GameLobby *lobby;
	if (choice == 1)
	{
		fflush(stdin);
		printf("game name> ");
		gameName = gets(buffer);
		
		lobby = new ServerLobby(gameName, playerName, LOBBY_PORT);
	}
	else if (choice == 2)
	{
		Net::Address addr;
		{
			Lobby::GameList games(LOBBY_PORT);
			games.onJoin = gamelist_join;
			games.onChange = gamelist_change;
			games.onPart = gamelist_part;
		
			puts("Waiting for servers...");
			printf("addr> ");
			
			std::string address = gets(buffer);
			addr = Net::Address(address.c_str(), LOBBY_PORT);
		}
		lobby = new ClientLobby(playerName, addr);
	}
	
	lobby->onConnect = lobby_connect;
	lobby->onPlayer = lobby_player;
	lobby->onJoin = lobby_join;
	lobby->onPart = lobby_part;
	lobby->onTeam = lobby_team;
	lobby->onState = lobby_state;
	lobby->onChat = lobby_chat;
	lobby->onClose = lobby_close;
	lobby->onStart = lobby_start;
	
	if (choice == 1)
	{
		Game game;
		game.numPlayers = 1;
		game.name = gameName;
		lobby_connect(1, game);
	}
	
	while (lobby->valid())
	{
		std::string input = gets(buffer);
		lobby->chat(input);
	}
	
	delete lobby;
	
	Net::Terminate();
	}
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

void gamelist_update()
{
	puts(std::string(50, '\n').c_str());
	puts("Servers found:");
	
	std::map<Net::Address,Game>::iterator it;
	for (it = gamelist.begin(); it != gamelist.end(); ++it)
	{
		char addr[256];
		it->first.name(addr, sizeof (addr));
		printf("\t%s) %s (%d)\n", addr, it->second.name.c_str(), it->second.numPlayers);
	}
	
	printf("addr> ");
}

//------------------------------------------------------------------------------

void gamelist_join(Net::Address server, Game game)
{
	gamelist[server] = game;
	gamelist_update();
}

//------------------------------------------------------------------------------

void gamelist_change(Net::Address server, Game game)
{
	gamelist[server] = game;
	gamelist_update();
}

//------------------------------------------------------------------------------

void gamelist_part(Net::Address server)
{
	gamelist.erase(server);
	gamelist_update();
}

//------------------------------------------------------------------------------

void lobby_connect(Player::Id pid, Game game)
{
	gameName = game.name;
	printf("Connected to server: %s (%d)\n", game.name.c_str(), game.numPlayers);
	playerlist[pid] = playerName;
}

//------------------------------------------------------------------------------

void lobby_player(Player player)
{
	playerlist[player.id] = player.name;
}

//------------------------------------------------------------------------------

void lobby_join(Player::Id pid, std::string playerName)
{
	printf("*** %s joined.\n", playerName.c_str());
	playerlist[pid] = playerName;
}

//------------------------------------------------------------------------------

void lobby_part(Player::Id pid)
{
	printf("*** %s parted.\n", playerlist[pid].c_str());
	playerlist.erase(pid);
}

//------------------------------------------------------------------------------

void lobby_team(Player::Id pid, unsigned char team)
{
	printf("*** %s changed to team %d.\n", playerlist[pid].c_str(), team);
}

//------------------------------------------------------------------------------

void lobby_state(Player::Id pid, Player::State state)
{
	
	printf("*** %s is %s.\n", playerlist[pid].c_str(), state ? "ready" : "busy");
}

//------------------------------------------------------------------------------

void lobby_chat(Player::Id pid, std::string line)
{
	printf("<%s> %s\n", playerlist[pid].c_str(), line.c_str());
}

//------------------------------------------------------------------------------

void lobby_close()
{
	puts("Connection lost.");
}

//------------------------------------------------------------------------------

void lobby_start()
{
}

//------------------------------------------------------------------------------
