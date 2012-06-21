/*
 * World object -- see header file for more info
 */

#include "broadcaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../common/net.h"
#include <sstream>
#include "game.h"
#ifdef _MSC_VER
	#ifndef WIN32
		#define WIN32 1
	#endif
#endif

#if (defined WIN32 || defined _MSC_VER)
	#include <windows.h>
	#include <stdint.h>
	#define sleep(x) Sleep((x)*1000)
#elif (defined __MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h> 
#else
	#include <unistd.h>
#endif
namespace Broadcaster {
using namespace Broadcaster;
using namespace Net;
using namespace Game;

struct Thread {
		bool run;
		pthread_t broadcastThread;
		UDPSocket udp;
};
Thread t;

void* broadcaster(void* ignoreBitch)
{
    while(t.run){
		sleep(1);
		Address receiver;
//		printf("hallo\n");
		if(game.player){
			char buf[1024];
			strcpy(buf,"IP-BROADCAST ");
			strcat(buf, game.player->name.c_str());
			printf(buf);
			printf("\n");
			size_t len = sizeof(buf);
			bool succes = t.udp.shout(9069,  (char*)&buf, len);
			char writebuf[1024];
			size_t length;
			while(succes = t.udp.recvfrom(receiver,writebuf, length = sizeof(writebuf))){
				printf("SUCCESSSS:! \n");
				printf(writebuf);
				printf("\n\n");
			}
		}
	}
}
	
void Initialize(){
	t.udp.bind(9069);
	t.udp.broadcast();
	t.udp.setNonBlocking();
	t.run = true;
	pthread_attr_t attr;
    /* get the default attributes */
    pthread_attr_init(&attr);
	void* argument;
    //create a thread which has as starting point of execution calcfib with parameter
    //param
    pthread_create(&(t.broadcastThread), NULL, broadcaster, argument);
}
	
void Terminate(){
	t.run = false;
	pthread_join((t.broadcastThread), NULL);
}
	

} // namespace Objects

//------------------------------------------------------------------------------
