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
void printf2(const char *fmt, ...);
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
	TokenRing tr((int) msg[0]);
	
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&thread, &attr, loop, (void *) &tr))
		puts("Warning: could not create a thread!");
	
	pthread_attr_destroy(&attr);
	
	while (tr.connected())
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
				tr.close();
			//else if (cmd == "!list")
			//	tr.debug();
			else if ((cmd == "!connect") && (msg.size() > 1))
				tr.connect(Address(string(msg[1]).c_str()));
		}
		else if (line[0] == '#')
		{
			msg[0].str.erase(0,1);
			tr.shout(msg, true);
		}
		else
			tr.shout(msg, false);
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
	TokenRing *tr = (TokenRing *)data;
	
	Message msg;
	NodeID id;
	bool reliable;
	while (tr->connected())
	{
		pthread_testcancel();
		tr->select();
		
		while (tr->entry(id))
			printf2("*** Node #%d entered.\n", id);
		
		while (tr->loss(id))
			printf2("*** Node #%d left.\n", id);
		
		while(tr->recvfrom(id, msg, reliable))
			printf2("%c<%d> %s\n", reliable ? '#' : ' ', id, string(msg).c_str());
	}
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

char *gets2(char *data, size_t len)
{
	data[0] = 0;
	fgets(data, len, stdin);
	char *ptr = data + strlen(data) - 1;
	if (*ptr == '\n') *ptr-- = 0;
	if (*ptr == '\r') *ptr = 0;
	return data;
}

//------------------------------------------------------------------------------

void printf2(const char *fmt, ...)
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
