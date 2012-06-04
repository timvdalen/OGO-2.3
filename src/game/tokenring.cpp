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
//#include <map>
//#include <queue>

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
	
//#define DECIDED (qd->connection != CliqueData::cnPending)
//#define INCOMMING (!qd->entry.empty() || !qd->loss.empty() || !qd->msgs.empty())
//#define CONNECTED (qd->connection == CliqueData::cnOpen)

using namespace std;
using namespace Net;

//------------------------------------------------------------------------------

struct TokenRingData
{
	NodeID id;
	NodeID nextId;
	NodeID token;
	
	pthread_t thread;
	pthread_mutex_t lock;  // Guards this very structure
	//pthread_cond_t decided; 
	//pthread_cond_t incomming;
};

//------------------------------------------------------------------------------

TokenRing::TokenRing(unsigned short port)
{
	data = (void *) new TokenRingData;
	
	VPRIV(TokenRingData, td)
	
	td->id = 1;
	td->nextId = 2;
	td->token = 1;
	clique = new Clique(port);
	
	pthread_mutex_init(&td->lock, NULL);
	//pthread_cond_init(&td->decided, NULL);
	//pthread_cond_init(&td->incomming, NULL);
	
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
	//pthread_cond_destroy(&td->decided);
	//pthread_cond_destroy(&td->incomming);
	
	delete clique;
	delete td;
}

//------------------------------------------------------------------------------


bool TokenRing::connect(const Address &remote, int timeout)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	pthread_mutex_lock(&td->lock);
	{
		success = clique->connect(remote, timeout);
		
		// Build token ring
	}
	pthread_mutex_unlock(&td->lock);
	
	if (!success) close();
	return (success);
}

//------------------------------------------------------------------------------

void TokenRing::close()
{
	VPRIV(TokenRingData, td)
	
	pthread_mutex_lock(&td->lock);
	{
		// Pass token
		
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
		conn = clique->connected();
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
	msg.push_back("#>");
	
	pthread_mutex_lock(&td->lock);
	{
		// Find id of next node
		//msg.push_back((long) ...);
		
		if (td->token == td->id)
			success = clique->shout(msg);
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::entry(NodeID &node)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::loss(NodeID &node)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::recvfrom(NodeID &node, Message &msg, bool &reliable)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::select(int timeout)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	return (success);
}

//------------------------------------------------------------------------------

void *TokenRing::process(void *obj)
{
	TokenRing *tokenring = (TokenRing *)obj;
	TokenRingData *td = (TokenRingData *)tokenring->data;
	
	if (!td)
	{
		pthread_exit(0);
		return (NULL);
	}
	
	bool valid = true;
	while (valid)
	{
	}
	
	tokenring->close();
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
