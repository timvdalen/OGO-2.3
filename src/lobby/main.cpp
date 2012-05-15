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

//------------------------------------------------------------------------------

std::map<Net::Address,Lobby::Game> gamelist;

void gamelist_update();
void gamelist_join(Net::Address server, Lobby::Game game);
void gamelist_change(Net::Address server, Lobby::Game game);
void gamelist_part(Net::Address server);

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	{
	Net::Initialize();
	
	std::string playerName, gameName;
	char name[256];
	int choice;
	
	printf("player name> ");
	playerName = gets(name);
	
	printf("\t1) server\n\t2) client\n> ");
	scanf("%d", &choice);
	

	if (choice == 1)
	{
		fflush(stdin);
		printf("game name> ");
		gameName = gets(name);
		
		Lobby::ServerLobby lobby(gameName, playerName, LOBBY_PORT);
		
		while (lobby.valid());
	}
	else if (choice == 2)
	{
		{
			Lobby::GameList gamelist(LOBBY_PORT);
		
			puts("Waiting for servers...");
			scanf("%d", &choice);
		}
		{
			std::map<Net::Address,Lobby::Game>::iterator it;
			int i;
			for (it = gamelist.begin(), i = 0; i < choice; ++it, ++i);
			Lobby::ClientLobby lobby(playerName, it->first);
			
			while (lobby.valid());
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
	
	std::map<Net::Address,Lobby::Game>::iterator it;
	int i;
	for (it = gamelist.begin(), i = 0; it != gamelist.end(); ++it, ++i)
		printf("\t%d) %s (%d)\n", i, it->second.name.c_str(), it->second.numPlayers);
	
	printf("> ");
}

//------------------------------------------------------------------------------

void gamelist_join(Net::Address server, Lobby::Game game)
{
	gamelist[server] = game;
	gamelist_update();
}

//------------------------------------------------------------------------------

void gamelist_change(Net::Address server, Lobby::Game game)
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
