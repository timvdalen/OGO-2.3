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
	

	if (choice == 1)
	{
		fflush(stdin);
		printf("game name> ");
		gameName = gets(buffer);
		
		Lobby::ServerLobby lobby(gameName, playerName, LOBBY_PORT);
		
		lobby.onConnect = lobby_connect;
		lobby.onPlayer = lobby_player;
		lobby.onJoin = lobby_join;
		lobby.onPart = lobby_part;
		lobby.onTeam = lobby_team;
		lobby.onState = lobby_state;
		lobby.onChat = lobby_chat;
		lobby.onClose = lobby_close;
		lobby.onStart = lobby_start;
		
		{
			Game game;
			game.numPlayers = 1;
			game.name = gameName;
			lobby_connect(1, game);
		}
		
		while (lobby.valid())
		{
			std::string input = gets(buffer);
			lobby.chat(input);
		}
	}
	else if (choice == 2)
	{
		{
			Lobby::GameList gamelist(LOBBY_PORT);
		
			puts("Waiting for servers...");
			scanf("%d", &choice);
		}
		{
			std::map<Net::Address,Game>::iterator it;
			int i;
			for (it = gamelist.begin(), i = 0; i < choice; ++it, ++i);
			ClientLobby lobby(playerName, it->first);
			
			lobby.onConnect = lobby_connect;
			lobby.onPlayer = lobby_player;
			lobby.onJoin = lobby_join;
			lobby.onPart = lobby_part;
			lobby.onTeam = lobby_team;
			lobby.onState = lobby_state;
			lobby.onChat = lobby_chat;
			lobby.onClose = lobby_close;
			lobby.onStart = lobby_start;
			
			while (lobby.valid())
			{
				std::string input = gets(buffer);
				lobby.chat(input);
			}
		}
	}
	
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
	int i;
	for (it = gamelist.begin(), i = 0; it != gamelist.end(); ++it, ++i)
		printf("\t%d) %s (%d)\n", i, it->second.name.c_str(), it->second.numPlayers);
	
	printf("> ");
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
