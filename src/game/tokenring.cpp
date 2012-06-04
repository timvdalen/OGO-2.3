/*
 * Network algorithms -- see header file for more information
 */

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#if (defined WIN32 || defined _MSC_VER)
#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <stdint.h>
#elif (defined __MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h> 
#endif

//#include <set>
#include <map>
#include <queue>

#include "netalg.h"

// Defined in clique.cpp
#if (defined WIN32 || defined _MSC_VER)
	void getthetime(timespec *tsp);
#elif (defined __MACH__)
	void getthetime(timespec *tsp);
#else
	#define getthetime(tsp) clock_gettime(CLOCK_REALTIME, tsp)
#endif

#define VPRIV(T,x) if (!data) return; T *x = (T *) data;
#define PRIV(T,x) if (!data) return false; T *x = (T *) data;

#define PRO\
BE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

namespace Protocol {
	
#define DECIDED (!td->deciding)
#define INCOMMING (!td->entry.empty() || !td->loss.empty() || !td->msgs.empty())
#define CONNECTED (clique && clique->connected())

using namespace std;
using namespace Net;

//------------------------------------------------------------------------------

struct TokenRingNode;

struct TokenRingData
{
	typedef pair<Message,bool> TRMessage;
	
	NodeID id;
	NodeID topId;
	NodeID token;
	
	map<NodeID,TokenRingNode> nodes;
	map<Address,NodeID> lookup;
	queue<Address> request;
	
	bool deciding;
	queue<NodeID> entry;
	queue<NodeID> loss;
	queue< pair<NodeID,TRMessage> > msgs;
	
	pthread_t thread;
	pthread_mutex_t lock;  // Guards this very structure
	pthread_cond_t decided; 
	pthread_cond_t incomming;
	
	NodeID nextId() const;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

struct TokenRingNode
{
	Address addr;
	queue<Message> buffer; // Fast forwarded reliable message buffer
};

//------------------------------------------------------------------------------

TokenRing::TokenRing(unsigned short port)
{
	data = (void *) new TokenRingData;
	
	VPRIV(TokenRingData, td)
	
	td->id = 1;
	td->topId = 2;
	td->token = 1;
	td->deciding = false;
	clique = new Clique(port);
	
	pthread_mutex_init(&td->lock, NULL);
	pthread_cond_init(&td->decided, NULL);
	pthread_cond_init(&td->incomming, NULL);
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&td->thread, &attr, process, (void *) this))
		close();
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

TokenRing::~TokenRing()
{
	VPRIV(TokenRingData, td)
	
	//pthread_mutex_lock(&td->lock); Would cause a deadlock
	{
		pthread_cancel(td->thread);
		
		void *status;
		pthread_join(td->thread, &status);
	}
	pthread_mutex_unlock(&td->lock);
	
	close();
	
	pthread_mutex_destroy(&td->lock);
	pthread_cond_destroy(&td->decided);
	pthread_cond_destroy(&td->incomming);
	
	delete clique;
	delete td;
}

//------------------------------------------------------------------------------


bool TokenRing::connect(const Address &remote, int timeout)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	Message msg;
	msg.push_back("#>");
	
	pthread_mutex_lock(&td->lock);
	{
		success = clique->connect(remote, timeout);
		
		// Build token ring
		if (success) success = clique->sendto(remote, msg);
		
		// Wait for response
		if (success)
		{
			int ret = 0;
			if (timeout > 0)
			{
				timespec ts;
				getthetime(&ts);
				ts.tv_sec += timeout;
				
				while (!DECIDED && !ret && CONNECTED)
					ret = pthread_cond_timedwait(&td->decided, &td->lock, &ts);
			}
			else
				while (!DECIDED && !ret && CONNECTED)
					ret = pthread_cond_wait(&td->decided, &td->lock);
			
			success = CONNECTED; // Connection will be broken on failure
		}
	}
	pthread_mutex_unlock(&td->lock);
	
	if (!success) close();
	return (success);
}

//------------------------------------------------------------------------------

void TokenRing::close()
{
	VPRIV(TokenRingData, td)
	
	if (!CONNECTED) return;
	
	Message msg;
	msg.push_back("#!");
	
	pthread_mutex_lock(&td->lock);
	{
		// Pass token
		if (td->token == td->id)
		{
			msg.push_back((long) td->nextId());
			clique->shout(msg);
		}
		
		clique->close();
	}
	pthread_mutex_unlock(&td->lock);
}

//------------------------------------------------------------------------------

bool TokenRing::connected() const
{
	PRIV(TokenRingData, td)
	bool conn;
	
	pthread_mutex_lock(&td->lock);
	{
		conn = CONNECTED;
	}
	pthread_mutex_unlock(&td->lock);
	
	return (conn);
}

//------------------------------------------------------------------------------

bool TokenRing::authorized() const
{
	PRIV(TokenRingData, td)
	bool auth;
	
	pthread_mutex_lock(&td->lock);
	{
		auth = (td->token == td->id);
	}
	pthread_mutex_unlock(&td->lock);
	
	return (auth);
}

//------------------------------------------------------------------------------

NodeID TokenRing::id() const
{
	PRIV(TokenRingData, td)
	NodeID id;
	
	pthread_mutex_lock(&td->lock);
	{
		id = td->id;
	}
	pthread_mutex_unlock(&td->lock);
	
	return (id);
}

//------------------------------------------------------------------------------

bool TokenRing::shout(const Message &msg, bool reliable)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	Message msg2 = msg;
	if (reliable)
		msg2[0].str.insert(0, 1, '#');
	
	pthread_mutex_lock(&td->lock);
	{
		if (!reliable || (td->token == td->id))
			success = clique->shout(msg2);
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::pass()
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	Message msg;
	msg.push_back("##");
	
	pthread_mutex_lock(&td->lock);
	{
		if (td->token == td->id)
		{
			msg.push_back((long) td->nextId());
			success = clique->shout(msg);
		}
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::entry(NodeID &node)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	pthread_mutex_lock(&td->lock);
	{
		if (!td->entry.empty())
		{
			node = td->entry.front();
			td->entry.pop();
			success = true;
		}	
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::loss(NodeID &node)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	pthread_mutex_lock(&td->lock);
	{
		if (!td->loss.empty())
		{
			node = td->loss.front();
			td->loss.pop();
			success = true;
		}	
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::recvfrom(NodeID &node, Message &msg, bool &reliable)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	pthread_mutex_lock(&td->lock);
	{
		if (!td->msgs.empty())
		{
			node = td->msgs.front().first;
			msg = td->msgs.front().second.first;
			reliable = td->msgs.front().second.second;
			td->msgs.pop();
			success = true;
		}	
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::select(int timeout)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	pthread_mutex_lock(&td->lock);
	{
		// wait for incomming changes (node entry, loss and messages)
		int ret = 0;
		if (timeout > 0)
		{
			timespec ts;
			getthetime(&ts);
			ts.tv_sec += timeout;
			
			while (!INCOMMING && !ret && CONNECTED)
				ret = pthread_cond_timedwait(&td->incomming, &td->lock, &ts);
		}
		else
			while (!INCOMMING && !ret && CONNECTED)
				ret = pthread_cond_wait(&td->incomming, &td->lock);
		
		success = INCOMMING;
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

void *TokenRing::process(void *obj)
{
	TokenRing *tokenring = (TokenRing *)obj;
	Clique *clique = tokenring->clique;
	TokenRingData *td = (TokenRingData *)tokenring->data;
	
	if (!td)
	{
		pthread_exit(0);
		return (NULL);
	}
	
	while (CONNECTED)
	{
	pthread_testcancel();
	pthread_mutex_lock(&td->lock);
	
	//---------------------------------------------------------------
	
	{
		
	}
	
	//---------------------------------------------------------------
	
	pthread_mutex_unlock(&td->lock);
	}
	
	tokenring->close();
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

NodeID TokenRingData::nextId() const
{
	map<NodeID,TokenRingNode>::const_iterator it = nodes.upper_bound(token);
	if (it == nodes.end())
		return nodes.begin()->first;
	else
		return it->first;
}

//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
