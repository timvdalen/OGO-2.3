/*
 * Network algorithms -- see header file for more information
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
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

#ifndef MAX
	#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define VPRIV(T,x) if (!data) return; T *x = (T *) data;
#define PRIV(T,x) if (!data) return false; T *x = (T *) data;

#define PRO\
BE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

#ifdef TRDEBUG
	#define DEBUG_SHOUT(msg) {printf("%c%d ->  @: %s\n",            \
		td->token == td->id ? '#' : ' ', td->id, msg);}
	#define DEBUG_SEND(dst,msg) {printf("%c%d -> %c%d: %s\n",       \
		td->token == td->id ? '#' : ' ', td->id,                    \
		td->token == dst    ? '#' : ' ', dst,  msg);}
	#define DEBUG_RECV(src,msg) {printf("%c%d <- %c%d: %s\n",       \
		td->token == td->id ? '#' : ' ', td->id,                    \
		td->token == src    ? '#' : ' ', src, msg);}
#else
	#define DEBUG_SHOUT(msg)
	#define DEBUG_SEND(dst,msg)
	#define DEBUG_RECV(src,msg)
#endif

namespace Protocol {
	
#define DECIDED (!td->deciding)
#define INCOMMING (!td->entry.empty() || !td->loss.empty() || !td->msgs.empty())
#define CONNECTED (clique && clique->connected())
#define AUTHORIZED ((td->token == td->id) || td->nodes.empty())

using namespace std;
using namespace Net;

//------------------------------------------------------------------------------

struct TokenRingNode;

struct TokenRingData
{
	typedef pair<Message,bool> TRMessage;
	
	Clique *clique;
	
	NodeID id;
	NodeID topId;
	NodeID token;
	
	// Invariant: nodes[lookup[X]].addr = X and lookup[nodes[Y].addr] = Y
	map<NodeID,TokenRingNode> nodes;
	map<Address,NodeID> lookup;
	queue<Address> request;
	
	bool deciding;
	bool tainted;
	queue<NodeID> entry;
	queue<NodeID> loss;
	queue< pair<NodeID,TRMessage> > msgs;
	
	pthread_t thread;
	pthread_mutex_t lock;  // Guards this very structure
	pthread_cond_t decided; 
	pthread_cond_t incomming;
	
	NodeID nextId() const;
	void receive(Message &msg, Address &addr);
	//! node nodeId passes token to node passId
	void arrival(NodeID nodeId, NodeID passId);
	void accept();
	void unbuffer(NodeID nodeId);
	void remove(NodeID nodeId);
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
	td->tainted = false;
	
	#ifdef TRDEBUG
		printf("Listening to port %d...\n", port);
	#endif
	
	clique = new Clique(port);
	td->clique = clique;
	
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
	
	close();
	
	//pthread_mutex_lock(&td->lock); Would cause a deadlock
	{
		pthread_cancel(td->thread);
		
		void *status;
		pthread_join(td->thread, &status);
	}
	pthread_mutex_unlock(&td->lock);
	
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
	
	#ifdef TRDEBUG
	{
		char buffer[128];
		remote.string(buffer);
		printf("Connecting to %s...\n", buffer);
	}
	#endif
	
	Message msg;
	msg.push_back("#>");
	
	pthread_mutex_lock(&td->lock);
	{
		td->deciding = true;
		
		success = clique->connect(remote, timeout);
		
		// Build token ring
		DEBUG_SEND(0, string(msg).c_str())
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
		if AUTHORIZED
		{
			msg.push_back((long) td->nextId());
			DEBUG_SHOUT(string(msg).c_str())
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
		auth = AUTHORIZED;
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

bool TokenRing::sendto(NodeID node, const Message &msg, bool reliable)
{
	PRIV(TokenRingData, td)
	bool success = false;
	
	Message msg2 = msg;
	if (reliable)
		msg2[0].str.insert(0, 1, '#');
	
	pthread_mutex_lock(&td->lock);
	{
		if (td->nodes.count(node) && (!reliable || AUTHORIZED))
			success = clique->sendto(td->nodes[node].addr, msg2);
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
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
		if (!reliable || AUTHORIZED)
			success = clique->shout(msg2);
	}
	pthread_mutex_unlock(&td->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool TokenRing::pass()
{
	PRIV(TokenRingData, td)
	bool success = true;
	
	Message msg;
	msg.push_back("##");
	
	pthread_mutex_lock(&td->lock);
	{
		if ((td->token == td->id) && !td->nodes.empty())
		{
			NodeID nextId = td->nextId();
			
			// Remove unresponsive nodes
			while (!clique->connected(td->nodes[nextId].addr) && success)
			{
				td->remove(nextId);
				Message msg2;
				msg2.push_back("#-");
				msg2.push_back((long) nextId);
				DEBUG_SHOUT(string(msg2).c_str())
				success &= clique->shout(msg2);
				
				nextId = td->nextId();
				if (td->nodes.empty()) break;
			}
			
			msg.push_back((long) nextId);
			DEBUG_SHOUT(string(msg).c_str())
			if (success && (success = clique->shout(msg)))
			{
				// Pass token and process buffered reliable messages
				td->token = nextId;
				td->unbuffer(nextId);
			}
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

bool TokenRing::tainted()
{
	PRIV(TokenRingData, td)
	bool tainted;
	
	pthread_mutex_lock(&td->lock);
	{
		tainted = td->tainted;
		td->tainted = false;
	}
	pthread_mutex_unlock(&td->lock);
	
	return (tainted);
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

void TokenRing::debug()
{
	VPRIV(TokenRingData, td)
	
	pthread_mutex_lock(&td->lock);
	{
		printf("Clique: ");
		clique->debug();
		printf("Ring: %c%d -- ", AUTHORIZED ? '#' : ' ', td->id);
		
		map<Address,NodeID>::iterator it;
		char ip[128];
		for (it = td->lookup.begin(); it != td->lookup.end(); ++it)
		{
			it->first.string(ip);
			NodeID id = it->second;
			printf("%c%d(%s) ", id == td->token ? '#' : ' ', id, ip);
		}
		printf("-> %d\n", td->nextId());
	}
	pthread_mutex_unlock(&td->lock);
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
	
	Message msg;
	Address addr;
	while (CONNECTED)
	{
	pthread_testcancel();
	pthread_mutex_lock(&td->lock);
	
	//---------------------------------------------------------------
	
	{
		// Parse incomming messages
		while (clique->recvfrom(addr,msg)) td->receive(msg, addr);
		
		// Parse connection loss
		while (clique->loss(addr))
		{
			if (!td->lookup.count(addr)) continue;
			NodeID nodeId = td->lookup[addr];
			if (nodeId == td->token)
				td->arrival(td->token, td->nextId());
			td->remove(nodeId);
		}
		
		// Parse incomming connections
		while (clique->entry(addr)); // We just ignore them
		
	}
	
	//---------------------------------------------------------------
	
	pthread_mutex_unlock(&td->lock);
	
	// Wait for changes
	clique->select();
	
	} // while (CONNECTED)
	
	td->deciding = false;
	pthread_cond_broadcast(&td->decided);
	
	tokenring->close();
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

NodeID TokenRingData::nextId() const
{
	if (nodes.empty()) return id;
	
	NodeID next;
	map<NodeID,TokenRingNode>::const_iterator it = nodes.upper_bound(token);
	if (it == nodes.end())
	{
		next = nodes.begin()->first;
		if ((id > token) || (id < next))
			next = id;
	}
	else
	{
		next = it->first;
		if ((id > token) && (id < next))
			next = id;
	}
	
	return next;
}

//------------------------------------------------------------------------------

void TokenRingData::receive(Message &msg, Address & addr)
{
	TokenRingData *td = this;
	string cmd = msg[0].str;
	size_t size = msg.size();
	if ((cmd == "#>") && (size == 1))      // Connection request
	{
		DEBUG_RECV(0,string(msg).c_str())
		request.push(addr);
		if (nodes.empty()) accept();
	}
	else if ((cmd == "#>") && (size > 1))  // Connection affirmation
	{
		DEBUG_RECV(lookup[addr],string(msg).c_str())
		id = (long) msg[1];
		topId = id + 1;
		token = lookup[addr];
		unbuffer(token);
		
		if (nodes.count(id))
		{
			lookup.erase(nodes[id].addr);
			nodes.erase(id);
		}
		
		deciding = false;
		pthread_cond_broadcast(&decided);
	}
	else if ((cmd == "#+") && (size == 2)) // Connection notification (requestee)
	{
		NodeID nodeId = (long) msg[1];
		DEBUG_RECV(nodeId,string(msg).c_str())
		
		TokenRingNode node;
		node.addr = addr;
		nodes[nodeId] = node;
		lookup[addr] = nodeId;
		{
			bool cond = INCOMMING;
			entry.push(nodeId);
			if (!cond) pthread_cond_broadcast(&incomming);
		}
	}
	else if ((cmd == "#+") && (size > 2))  // Connection notification (others)
	{
		NodeID nodeId = (long) msg[1];
		Address remote = Address(msg[2].str.c_str());
		DEBUG_RECV(lookup[addr],string(msg).c_str())
		
		TokenRingNode node;
		node.addr = remote;
		nodes[nodeId] = node;
		lookup[remote] = nodeId;
		topId = MAX(topId, nodeId) + 1;
		{
			bool cond = INCOMMING;
			entry.push(nodeId);
			if (!cond) pthread_cond_broadcast(&incomming);
		}
	}
	else if (!lookup.count(addr))          // Sender not part of the tokenring
	{
		// Do nothing
		
		// Hack
		if ((cmd == "##") && (size > 1))  // Token pass message
		{
			DEBUG_RECV(lookup[addr],string(msg).c_str())
			token = (long) msg[1];
		}
	}
	else if ((cmd == "#-") && (size > 1))  // Connection removal request
	{
		DEBUG_RECV(lookup[addr],string(msg).c_str())
		NodeID nodeId = (long) msg[1];
		if (nodes.count(nodeId))
			remove(nodeId);
	}
	else if ((cmd == "##") && (size > 1))  // Token pass message
	{
		DEBUG_RECV(lookup[addr],string(msg).c_str())
		NodeID nodeId = lookup[addr];
		NodeID passId = (long) msg[1];
		
		if (token != nodeId)
			nodes[nodeId].buffer.push(msg);
		else
			arrival(nodeId, passId);
	}
	else if ((cmd == "#!") && (size > 1))  // Token pass and leave message
	{
		DEBUG_RECV(lookup[addr],string(msg).c_str())
		NodeID nodeId = lookup[addr];
		NodeID passId = (long) msg[1];
		
		if (token != nodeId)
			nodes[nodeId].buffer.push(msg);
		else
		{
			arrival(nodeId, passId);
			remove(nodeId);
		}
	}
	else if (cmd[0] == '#')                // Reliable message
	{
		NodeID nodeId = lookup[addr];
		if (token != nodeId)
			nodes[nodeId].buffer.push(msg);
		else
		{
			msg[0].str.erase(0, 1);
			bool cond = INCOMMING;
			msgs.push(make_pair(nodeId,make_pair(msg,true)));
			if (!cond) pthread_cond_broadcast(&incomming);
		}
	}
	else                                   // Unrealiable message
	{
		bool cond = INCOMMING;
		msgs.push(make_pair(lookup[addr],make_pair(msg,false)));
		if (!cond) pthread_cond_broadcast(&incomming);
	}
}

//------------------------------------------------------------------------------

void TokenRingData::arrival(NodeID nodeId, NodeID passId)
{
	TokenRingData *td = this;
	NodeID nextId = this->nextId();
	
	if ((nextId == id) && (passId == id))      // this node gets the token
	{
		// Receive token
		token = id;
		
		// Process a connection request
		accept();
	}
	else if ((nextId == id) && (passId != id)) // node token expected
	{
		// close connection
		//clique->close(); Ignore for now
		return;
	}
	else if ((nextId != id) && (passId == id)) // node token unexpected
	{
		// pass token to indicated node
		Message msg;
		msg.push_back("##");
		msg.push_back((long) passId);
		DEBUG_SHOUT(string(msg).c_str())
		if (!clique->shout(msg))
		{
			clique->close();
			return;
		}
		
		// Pass token and process buffered reliable messages
		token = passId;
		unbuffer(token);
	}
	else                                       // nodes token passed
	{
		// Pass token and process buffered reliable messages
		token = passId;
		unbuffer(token);
		
		// Concistency check
		TokenRingNode *node = &nodes[token];
		if (!clique->connected(node->addr))
		{
			tainted = true;
			return;
		}
	}
}

//------------------------------------------------------------------------------

void TokenRingData::accept()
{
	TokenRingData *td = this;
	if (request.empty()) return;
	
	Address remote = request.front();
	request.pop();
	
	NodeID newId = topId++;
	
	map<NodeID,TokenRingNode>::iterator it;
	Message msg;
	char ip[128] = "";
	
	msg.push_back("#+");
	msg.push_back((long) newId);
	remote.string(ip);
	msg.push_back(ip);
	for (it = nodes.begin(); it != nodes.end(); ++it)
	{
		DEBUG_SEND(it->first,string(msg).c_str());
		clique->sendto(it->second.addr, msg);
	}
	
	for (it = nodes.begin(); it != nodes.end(); ++it)
	{
		it->second.addr.string(ip);
		msg[1] = (long) it->first;
		msg[2] = ip;
		DEBUG_SEND(0,string(msg).c_str());
		clique->sendto(remote, msg);
	}
	
	msg.pop_back();
	msg[1] = (long) id;
	DEBUG_SEND(0,string(msg).c_str());
	clique->sendto(remote, msg);
	
	msg[0] = "#>";
	msg[1] = (long) newId;
	DEBUG_SEND(0,string(msg).c_str());
	clique->sendto(remote, msg);
	
	TokenRingNode node;
	node.addr = remote;
	nodes[newId] = node;
	lookup[remote] = newId;
	
	{
		TokenRingData *td = this;
		bool cond = INCOMMING;
		entry.push(newId);
		if (!cond) pthread_cond_broadcast(&incomming);
	}
}

//------------------------------------------------------------------------------

void TokenRingData::unbuffer(NodeID nodeId)
{
	#ifdef TRDEBUG
		puts("[");
	#endif
	
	TokenRingNode *node = &nodes[nodeId];
	while (!node->buffer.empty())
	{
		receive(node->buffer.front(), node->addr);
		node->buffer.pop();
	}
	
	#ifdef TRDEBUG
		puts("]");
	#endif
}

//------------------------------------------------------------------------------

void TokenRingData::remove(NodeID nodeId)
{
	TokenRingData *td = this;
	lookup.erase(nodes[nodeId].addr);
	nodes.erase(nodeId);
	{
		bool cond = INCOMMING;
		loss.push(nodeId);
		if (!cond) pthread_cond_broadcast(&incomming);
	}
}

//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
