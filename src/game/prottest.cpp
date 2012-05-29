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
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include <map>
#include <string>

#include "common.h"
#include "netalg.h"

using namespace std;
using namespace Net;
using namespace Protocol;

string queue = "";
bool queueing = true;
char *gets2(char *data, size_t len);
char *printf2(const char *fmt, ...);
void *loop(void *);

//------------------------------------------------------------------------------

int last = 1;
map<Address,int> id;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	string line;
	Message msg;
	Address remote;
	char buffer[1024];
	
	Initialize();
	
	printf("port> ");
	line = gets2(buffer, sizeof(buffer));
	msg = line;
	Clique cq((int) msg[0]);
	
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&thread, &attr, loop, (void *) &cq))
		puts("Warning: could not create a thread!");
	
	pthread_attr_destroy(&attr);
	
	while (cq.connected())
	{
		if (queueing)
			printf("> ");
		
		line = gets2(buffer, sizeof(buffer));
		msg = line;
		
		if (!queue.empty())
		{
			printf(queue.c_str());
			queue.clear();
		}
		
		if (line.empty())
			queueing = !queueing;
		else if (line[0] == '!')
		{
			string cmd = msg[0];
			if ((cmd == "!exit") || (cmd == "!close"))
				cq.close();
			else if (cmd == "!list")
				cq.debug();
			else if ((cmd == "!break") && (msg.size() > 1))
			{
				Address node(string(msg[1]).c_str());
				cq.debugClose(node);
			}
			else if ((cmd == "!connect") && (msg.size() > 1))
				cq.connect(Address(string(msg[1]).c_str()));
		}
		else
			cq.shout(msg);
	}
	
	pthread_cancel(thread);
	
	void *status;
	pthread_join(thread, &status);
	
	Terminate();
		
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

void *loop(void *data)
{
	Clique *cq = (Clique *)data;
	
	Message msg;
	Address remote;
	while (cq->connected())
	{
		pthread_testcancel();
		cq->select();
		
		while (cq->entry(remote))
			printf2("*** Node #%d entered.\n", id[remote] = last++);
		
		while (cq->loss(remote))
			printf2("*** Node #%d left.\n", id[remote]);
		
		while(cq->recvfrom(remote, msg))
			printf2("<%d> %s\n", id[remote], string(msg).c_str());
	}
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

char *gets2(char *data, size_t len)
{
	fgets(data, len, stdin);
	char *ptr = data + strlen(data) - 1;
	if (*ptr == '\n') *ptr-- = 0;
	if (*ptr == '\r') *ptr = 0;
	return data;
}

//------------------------------------------------------------------------------

char *printf2(const char *fmt, ...)
{
	char buffer[1024];
	va_list args;
	
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	
	if (queueing)
		queue += string(buffer);
	else
		printf(buffer);
}

//------------------------------------------------------------------------------
