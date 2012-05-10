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

#include "net.h"
#include "protocol.h"
#include "games.h"
#include "lobby.h"

//------------------------------------------------------------------------------

void Join(Net::Address server, Lobby::Game game)
{
	printf("Joined: %s %d\n", game.name.c_str(), game.numPlayers);
}

void Change(Net::Address server, Lobby::Game game)
{
	printf("Changed: %s %d\n", game.name.c_str(), game.numPlayers);
}

void Part(Net::Address server)
{
	printf("Parted\n");
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	
	/*{
	Net::Initialize();
	
	char buffer[1024];
	size_t length;
	
	Protocol::TextSocket server;
	server.listen(72);
	
	puts("Listening...");
	
	Net::Address remote;
	Protocol::TextSocket client = server.accept(remote);
	
	remote.name(buffer, sizeof (buffer));
	printf("Client connected: %s\n", buffer);
	
	std::string msg;
	while (client.valid())
	{
		msg = client.recv();
		
		if (msg.empty())
			continue;
		
		if (msg == "exit")
			client.close();
		else
		{
			client.send(msg);
			printf("> %s\n", msg.c_str());
		}
	}
	
	Net::Terminate();
	}*/
	
	{
	Net::Initialize();
	
	int choice;
	printf("1) server\n2) client\n >");
	scanf("%d", &choice);
	
	if (choice == 2)
	{
		Lobby::GameList gamelist(LOBBY_PORT);
		
		while (gamelist.valid());
	}
	else if (choice == 1)
	{
		fflush(stdin);
		printf(" name>");
		char name[256];
		gets(name);
		Lobby::ServerLobby lobby(std::string(name), "unknown", LOBBY_PORT);
		
		while (lobby.valid());
	}
	
	Net::Terminate();
	}
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
