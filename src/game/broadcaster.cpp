/*
 * World object -- see header file for more info
 */

#include <stdio.h>
#if (defined WIN32 || defined _MSC_VER)
#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <windows.h>
	#include <stdint.h>
	#define sleep(x) Sleep((x)*1000)
	#define usleep(x) Sleep((x))
#elif (defined __MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h> 
#else
	#include <unistd.h>
#endif
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#ifdef _MSC_VER
	#ifndef WIN32
		#define WIN32 1
	#endif
#endif


#define SOCKET_ERROR -1
#define MAX_UDP_MESSAGE_LENGTH 65536
#include <errno.h>
#include "net.h"
#include "game.h"
#include "broadcaster.h"
/**/
namespace Broadcaster {
using namespace Broadcaster;
using namespace Net;
using namespace Game;

struct Thread {
		bool run;
		pthread_t broadcastThread;
		UDPSocket* udp;
};
Thread t;

void* broadcaster(void* ignoreBitch)
{
	sleep(5);
	t.udp = new UDPSocket();
	t.udp->setNonBlocking();
	t.udp->broadcast();
	t.udp->bind(9069);
	char* buffer = new char[MAX_UDP_MESSAGE_LENGTH];
	size_t buffer_length = sizeof(char)*MAX_UDP_MESSAGE_LENGTH;
	/*if(!succes){
		printf("Could not initialize discovery module!\n Something went wrong loading the udp ports!\n");
		return 0;
	}*/
    while(t.run){
		sleep(1);
		Address receiver;
		if(game.player){
			int message_length = 15+(game.player->name.length());
			char* buf = new char[message_length];
			size_t buf_length = sizeof(char)*message_length;
			strcpy(buf,"IP-BROADCAST ");
			strcat(buf, game.player->name.c_str());
			t.udp->shout(9069, (const char*) buf, buf_length);
			//Receive all pending messages
			while(t.udp->recvfrom(receiver, buffer, buffer_length)){
				printf(buffer);
				printf(" ip: ");
				receiver.string(buffer);
				printf(buffer);
				printf("\n\n");
				buffer_length = sizeof(char)*MAX_UDP_MESSAGE_LENGTH;
			}
			delete buf;
		}
	}
	//close and destroy pointers
	t.udp->close();
	delete t.udp;
	delete buffer;
}
	
void Initialize(){
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
	return;
}
	

} // namespace Objects

//------------------------------------------------------------------------------
