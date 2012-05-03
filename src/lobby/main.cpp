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

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	
	Net::Initialize();
	
	Net::UDPSocket sock;
	sock.broadcast();
	sock.setNonBlocking();
	sock.bind(1337);
	
	char buffer[1024];
	for (;;)
	{
		printf("#> ");
		gets(buffer);
		if (*buffer)
		{
			if (!stricmp(buffer, "exit")) break;
			sock.shout(1337, buffer, strlen(buffer) + 1);
		}
				
		int ret = 1;
		while (ret != -1)
		{
			Net::Address remote;
			ret = sock.recvfrom(remote, buffer, sizeof (buffer));
			char name[128];
			remote.name(name, sizeof (name));
			printf("<%s> %s\n", name, buffer);
		}
	}
	
	Net::Terminate();
	
	//puts("Press any key...");
	//getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
