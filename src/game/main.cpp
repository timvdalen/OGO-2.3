/*
 * Main module
 * 
 * Date: 01-05-12 15:01
 *
 * Description: 
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "netalg.h"
#include "objects.h"
#include "video.h"

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	Video::Initialize(argc, argv);
	Video::Window w1(640, 480, "Game");
	
	Video::Viewport *v1 = new Video::Viewport(640, 480);
	v1->world = Objects::World(100,100);
	
	w1.viewports.insert(v1);
	
	Video::StartEventLoop();
	
	/*{
		Net::Initialize();
		
		unsigned short port = 23;
		scanf("%d", &port);
		
		Protocol::Clique cq(port);
		while (cq.connected())
		{
			printf("> ");
			char buffer[128];
			fgets(buffer, sizeof (buffer), stdin);
			if (*buffer == '>')
				cq.connect(Net::Address(buffer + 1));
			else
				cq.shout(std::string(buffer));
			
			if (cq.select(1))
			{
				Net::Address remote;
				Protocol::Message msg;
				if (cq.recvfrom(remote, msg))
				{
					remote.string(buffer);
					printf("%s > %s\n", buffer, ((std::string) msg).c_str());
				}
				if (cq.entry(remote))
				{
					remote.string(buffer);
					printf("%s connected!\n", buffer);
				}
				if (cq.loss(remote))
				{
					remote.string(buffer);
					printf("%s disconnected!\n", buffer);
				}
			}
		}
		puts("Connection closed!");
		
		Net::Terminate();
	}*/
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
