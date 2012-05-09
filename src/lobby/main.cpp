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
	
	Protocol::Message msg;
	msg.add("TEST");
	msg.add(4.5);
	msg.add(1337L);
	std::string str = msg;
	
	printf("%s", std::string(msg).c_str());
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
