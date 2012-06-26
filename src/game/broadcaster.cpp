
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
#include <vector>

#ifdef _MSC_VER
	#ifndef WIN32
		#define WIN32 1
	#endif
#endif


#define SOCKET_ERROR -1
#define MAX_UDP_MESSAGE_LENGTH 65536
#define TIMEOUT_TIME 5000
#include "net.h"
#include "game.h"
#include "video.h"
#include "broadcaster.h"
/**/
namespace Broadcaster {
using namespace std;
using namespace Broadcaster;
using namespace Net;
using namespace Game;

//Used to hold data from a recently received neighbour
struct Neighbour{
		dword lastMessage;
		string name;
		string ip;
};

//Used to hold all data for the broadcaster thread
struct Thread {
		bool run;
		pthread_t broadcastThread;
		UDPSocket* udp;
		vector<Neighbour*> neighbours;
		pthread_mutex_t mutex;
};
Thread t;


char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

void updateNeighbours(Neighbour* u){
	pthread_mutex_lock(&t.mutex);
	for(int i = 0; i < t.neighbours.size(); i++){
		Neighbour* n = t.neighbours[i];
		if(n->name == u->name && n->ip == u->ip){//Just update the entry
			t.neighbours[i] = u;
			delete n;
			pthread_mutex_unlock(&t.mutex);
			return;
		}
	}
	//Not updated, insert the element
	t.neighbours.push_back(u);
	pthread_mutex_unlock(&t.mutex);
}

void removeOldNeighbours(){
	pthread_mutex_lock(&t.mutex);
	vector<Neighbour*>::iterator it;
	for(it=t.neighbours.begin(); it < t.neighbours.end(); it++){
		Neighbour* n = *it;
		if(Video::ElapsedTime() -  n->lastMessage > TIMEOUT_TIME){
			it = t.neighbours.erase(it);
			it--;
			delete n;
		}
	}
	pthread_mutex_unlock(&t.mutex);
}

//!Gets all players playing in the same subnet
string getNeighbours(){
	pthread_mutex_lock(&t.mutex);
	stringstream ss;
	for(int i = 0; i < t.neighbours.size(); i++){
		Neighbour* n = t.neighbours[i];
		ss << i+1 << " - " << n->name << "\n";
	}
	pthread_mutex_unlock(&t.mutex);
	return ss.str();
}

string getIP(int index){
	pthread_mutex_lock(&t.mutex);
	if(0 <= index && index < t.neighbours.size()){
		string result(t.neighbours[index]->ip);
		pthread_mutex_unlock(&t.mutex);
		return result;
	}else{
		pthread_mutex_unlock(&t.mutex);
		return "";
	}
}

//Debugging output
void printNeighbours(){
	pthread_mutex_lock(&t.mutex);
	for(int i = 0; i < t.neighbours.size(); i++){
		Neighbour* n = t.neighbours[i];
		printf("Neighbour %d: name = %s, ip = %s, timesincelastmessage = %ld ms \n",
		i, (n->name).c_str(), (n->ip).c_str(), (Video::ElapsedTime() -  n->lastMessage));
	}
	pthread_mutex_unlock(&t.mutex);
}

//IP-RESPONSE is used for detecting your own ip address by sending a response to each shout
//IP-BROADCAST is used for giving your own ip and player name to the opponent
//IP-BROADCAST1 request for a response telling me my own ip-adress
//IP-BROADCAST2 requests for no response
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
	char* own_ip = new char[64];
	strcpy(own_ip, "");
    while(t.run){
		sleep(1);
		Address receiver;
		if(game.player){
			int message_length = 16+(game.player->name.length());
			char* buf = new char[message_length];
			size_t buf_length = sizeof(char)*message_length;
			if(!strcmp(own_ip,"")){
				strcpy(buf,"IP-BROADCAST1 ");
			}else{
				strcpy(buf,"IP-BROADCAST2 ");
			}
			strcat(buf, game.player->name.c_str());
			t.udp->shout(9069, (const char*) buf, buf_length);
			//Receive all pending messages
			while(t.udp->recvfrom(receiver, buffer, buffer_length)){
				char* index = strstr(buffer, "IP-BROADCAST");
				if(index && !(index-buffer)){
					index = strstr(buffer, "IP-BROADCAST1");
					bool request_for_ip = index && !(index-buffer);
					//buffer starts with IP-BROADCAST
					Neighbour* n = new Neighbour();
					n->name = (trimwhitespace(buffer+14));
					receiver.string(buffer);
					//----- FOR detecting your own ip
					if(request_for_ip){
						message_length = 64;
						buf = new char[message_length];
						size_t buf_length = sizeof(char)*message_length;
						strcpy(buf, "IP-RESPONSE ");
						strcat(buf, buffer);
						t.udp->sendto(receiver, buf, buf_length);
					}
					//----- Continue with other part
					if(strcmp(own_ip,"") == 0 || strcmp(own_ip,buffer) == 0){
					//Not sure if this is not myself, so in doubt discard message
						delete n;
					}else{
						buffer[strchr(buffer, ':')-buffer] = 0;
						n->ip = (trimwhitespace(buffer));
					//This certainly is not me, so append this to the list
						n->ip.append(":9070");
						n->lastMessage = Video::ElapsedTime();
						updateNeighbours(n);
					}
				}else{
					index = strstr(buffer,"IP-RESPONSE ");
					if(index && !(index-buffer)){
						strcpy(own_ip, trimwhitespace(buffer+12));
					}
				}
				buffer_length = sizeof(char)*MAX_UDP_MESSAGE_LENGTH;
			}
			delete buf;
		}
	//	printNeighbours();
		removeOldNeighbours();
	}
	//Send message to rest of the world
	//close and destroy pointers
	t.udp->close();
	delete t.udp;
	delete buffer;
	return NULL;
}
	
void Initialize(){
	pthread_mutex_init(&t.mutex, NULL);
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
	pthread_mutex_destroy(&t.mutex);
	return;
}
	

} // namespace Broadcaster

//------------------------------------------------------------------------------
