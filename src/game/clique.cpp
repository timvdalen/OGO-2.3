/*
 * Network algorithms -- see header file for more information
 */

#include <time.h>
//#include <stdio.h>
#include <pthread.h>
#if (defined WIN32 || defined _MSC_VER)
#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <stdint.h>
#elif (defined __MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h> 
#endif

#include <set>
#include <map>
#include <queue>

#include "netalg.h"

#if (defined WIN32 || defined _MSC_VER)
	void getthetime(timespec *tsp)
	{
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		int64_t t = (*(int64_t *) &ft - ( ((int64_t) 27111902UL << 32) + (int64_t) 3577643008UL ));
		tsp->tv_sec  = (int) (t / 10000000);
		tsp->tv_nsec = (int) ((t - ((int64_t) tsp->tv_sec * (int64_t) 10000000)) * 100);
	}
#elif (defined __MACH__)
	void getthetime(timespec *tsp)
	{
		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		tsp->tv_sec = mts.tv_sec;
		tsp->tv_nsec = mts.tv_nsec;
	}
#else
	#define getthetime(tsp) clock_gettime(CLOCK_REALTIME, tsp)
#endif

#define VPRIV(T,x) if (!data) return; T *x = (T *) data;
#define PRIV(T,x) if (!data) return false; T *x = (T *) data;

#define PRO\
BE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

namespace Protocol {
	
#define DECIDED (qd->connection != CliqueData::cnPending)
#define INCOMMING (!qd->entry.empty() || !qd->loss.empty() || !qd->msgs.empty())
#define CONNECTED (qd->connection == CliqueData::cnOpen)

using namespace std;
using namespace Net;

//------------------------------------------------------------------------------

struct CliqueNode;
struct CliqueGraph;
struct CliqueData
{
	typedef map<Address,CliqueNode> NodeList;
	
	MsgSocket *sock;
	unsigned short port; // Unguarded
	
	NodeList connected;
	set<Address> connecting;
	map<Address, pair<size_t,time_t> > lost;
	
	enum Connection { cnClosed, cnPending, cnOpen } connection;
	queue<Address> entry, loss;
	queue< pair<Address,Message> > msgs;
	
	pthread_t thread;
	pthread_mutex_t lock;  // Guards this very structure
	pthread_cond_t decided; 
	pthread_cond_t incomming;
	
	CliqueNode *find(MsgSocket *sock) const;
	void closed(MsgSocket *sock, bool reply = false);
	void loose(const Address &remote);
};

struct CliqueNode
{
	MsgSocket *sock;
	Address remote;
	
	CliqueNode() : sock(NULL), remote() {}
	CliqueNode(MsgSocket *S, const Address &A) : sock(S), remote(A) {}
	
	bool operator <(const CliqueNode &b) const { return sock < b.sock; }
	operator string() const;
};

//------------------------------------------------------------------------------

Clique::Clique(unsigned short port)
{
	data = (void *) new CliqueData;
	
	VPRIV(CliqueData, qd)
	
	qd->sock = new MsgSocket();
	qd->port = port;
	qd->connection = qd->sock->listen(port) ? CliqueData::cnOpen
	                                        : CliqueData::cnClosed;
	qd->sock->setNonBlocking();
	
	pthread_mutex_init(&qd->lock, NULL);
	pthread_cond_init(&qd->decided, NULL);
	pthread_cond_init(&qd->incomming, NULL);
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&qd->thread, &attr, process, (void *) this))
		close();
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

Clique::~Clique()
{
	VPRIV(CliqueData, qd)
	
	//pthread_mutex_lock(&qd->lock); <-- select will lock, just cancel it
	{
		pthread_cancel(qd->thread);
		
		void *status;
		pthread_join(qd->thread, &status);
	}
	pthread_mutex_unlock(&qd->lock);
	
	close();
	
	pthread_mutex_destroy(&qd->lock);
	pthread_cond_destroy(&qd->decided);
	pthread_cond_destroy(&qd->incomming);
	
	delete qd->sock;
	delete qd;
}

//------------------------------------------------------------------------------

bool Clique::connect(const Address &remote, int timeout)
{
	PRIV(CliqueData, qd)
	bool success = false;
	
	pthread_mutex_lock(&qd->lock);
	do {
		// if already part of a clique: fail connect
		if (!qd->connected.empty()) break;
		
		MsgSocket *sock = new MsgSocket();
		if (!sock) break;
		
		if (!sock->reuse()) break;
		if (!sock->bind(qd->port + 1)) break;
		if (!sock->connect(remote)) break;
		if (!sock->setNonBlocking()) break;
		
		qd->connected[remote] = CliqueNode(sock, remote);
		qd->connection = CliqueData::cnPending;
		reset();
		
		// wait for connection
		int ret = 0;
		if (timeout > 0)
		{
			timespec ts;
			getthetime(&ts);
			ts.tv_sec += timeout;
			
			while (!DECIDED && !ret)
				ret = pthread_cond_timedwait(&qd->decided, &qd->lock, &ts);
		}
		else
			while (!DECIDED && !ret)
				ret = pthread_cond_wait(&qd->decided, &qd->lock);
		
		success = (qd->connection == CliqueData::cnOpen);
		if (!success) break;
		
		qd->entry.push(remote);
		pthread_cond_broadcast(&qd->incomming);
	} while(0);
	pthread_mutex_unlock(&qd->lock);
	
	
	if (!success) close();
	return (success);
}

//------------------------------------------------------------------------------

void Clique::close()
{
	VPRIV(CliqueData, qd)
	
	pthread_mutex_lock(&qd->lock);
	{
		CliqueData::NodeList::iterator it;
		for (it = qd->connected.begin(); it != qd->connected.end(); ++it)
		{
			it->second.sock->send(Message("@!"));
			it->second.sock->close();
			delete it->second.sock;
		}
		qd->connected.clear();
		
		qd->sock->close();
		qd->connection = CliqueData::cnClosed;
	}
	pthread_mutex_unlock(&qd->lock);
	pthread_cond_broadcast(&qd->incomming);
}

//------------------------------------------------------------------------------

bool Clique::connected() const
{
	PRIV(CliqueData, qd)
	bool conn;
	
	pthread_mutex_lock(&qd->lock);
	{
		conn = (qd->connection == CliqueData::cnOpen);
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (conn);
}

//------------------------------------------------------------------------------

bool Clique::connected(const Address &node) const
{
	PRIV(CliqueData, qd)
	bool conn;
	
	pthread_mutex_lock(&qd->lock);
	{
		conn = !!qd->connected.count(node);
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (conn);
}

//------------------------------------------------------------------------------

bool Clique::sendto(const Address &node, const Message &msg)
{
	PRIV(CliqueData, qd)
	bool success = false;
	
	pthread_mutex_lock(&qd->lock);
	{
		if (qd->connected.count(node))
			success = qd->connected[node].sock->send(msg);
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool Clique::shout(const Message &msg)
{
	PRIV(CliqueData, qd)
	bool success = true;
	
	pthread_mutex_lock(&qd->lock);
	{
		CliqueData::NodeList::iterator it;
		for (it = qd->connected.begin(); it != qd->connected.end(); ++it)
			success &= it->second.sock->send(msg);
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool Clique::entry(Address &node)
{
	PRIV(CliqueData, qd)
	bool success = false;
	
	pthread_mutex_lock(&qd->lock);
	{
		if (!qd->entry.empty())
		{
			node = qd->entry.front();
			qd->entry.pop();
			success = true;
		}	
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool Clique::loss(Address &node)
{
	PRIV(CliqueData, qd)
	bool success = false;
	
	pthread_mutex_lock(&qd->lock);
	{
		if (!qd->loss.empty())
		{
			node = qd->loss.front();
			qd->loss.pop();
			success = true;
		}	
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool Clique::recvfrom(Address &node, Message &msg)
{
	PRIV(CliqueData, qd)
	bool success = false;
	
	pthread_mutex_lock(&qd->lock);
	{
		if (!qd->msgs.empty())
		{
			node = qd->msgs.front().first;
			msg = qd->msgs.front().second;
			qd->msgs.pop();
			success = true;
		}	
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

bool Clique::select(int timeout)
{
	PRIV(CliqueData, qd)
	bool success = false;
	
	pthread_mutex_lock(&qd->lock);
	{
		// wait for incomming changes (node entry, loss and messages)
		int ret = 0;
		if (timeout > 0)
		{
			timespec ts;
			getthetime(&ts);
			ts.tv_sec += timeout;
			
			while (!INCOMMING && !ret && CONNECTED)
				ret = pthread_cond_timedwait(&qd->incomming, &qd->lock, &ts);
		}
		else
			while (!INCOMMING && !ret && CONNECTED)
				ret = pthread_cond_wait(&qd->incomming, &qd->lock);
		
		success = INCOMMING;
	}
	pthread_mutex_unlock(&qd->lock);
	
	return (success);
}

//------------------------------------------------------------------------------

void Clique::debug()
{
	VPRIV(CliqueData, qd);
	CliqueData::NodeList::iterator nit;
	for (nit = qd->connected.begin(); nit != qd->connected.end(); ++nit)
		printf("%s ", string(nit->second).c_str());
	puts("");
}

//------------------------------------------------------------------------------

void Clique::debugClose(Address &node)
{
	VPRIV(CliqueData, qd);
	
	if (!qd->connected.count(node)) return;
	qd->connected[node].sock->close();
}

//------------------------------------------------------------------------------

void *Clique::process(void *obj)
{
	Clique *clique = (Clique *)obj;
	CliqueData *qd = (CliqueData *)clique->data;
	
	if (!qd)
	{
		pthread_exit(0);
		return (NULL);
	}
	
	Socket::List read, write, error;
	CliqueData::NodeList::iterator nit;
	bool valid = true;
	while (valid)
	{
		// Wait for incomming connections or messages
		read.clear(), write.clear(), error.clear();
		
		pthread_testcancel();
		pthread_mutex_lock(&qd->lock);
		{
			read.push_back(qd->sock);  // Add listening socket to selection
			for (nit = qd->connected.begin(); nit != qd->connected.end();)
			{
				// Check for liveness
				if (!nit->second.sock->valid())
				{
					qd->closed(nit->second.sock);
					read.clear();
					nit = qd->connected.begin();
				}
				else // Add nodes to selection
					read.push_back((nit++)->second.sock);
			}
		}
		
		// See if half-open connections had a timeout
		{
			map<Address, pair<size_t,time_t> >::iterator it;
			for (it = qd->lost.begin(); it != qd->lost.end();)
			{
				if (it->second.second <= time(NULL))
				{
					qd->loose(it->first);
					it = qd->lost.begin(); 
				}
				else
					++it;
			}
		}
		pthread_mutex_unlock(&qd->lock);
		
		if (read.empty()) break;
		if (!Socket::select(read, write, error)) break;
		
		pthread_testcancel();
		pthread_mutex_lock(&qd->lock);
		{
			// Process incomming connections or messages
			Socket::List::iterator it;
			for (it = read.begin(); it != read.end(); ++it)
			{
				MsgSocket *sock = (MsgSocket *) *it;
				if (sock == qd->sock)
				{
					// Process incomming connections
					TCPSocket client;
					Address remote;
					if (!sock->accept(client, remote)) continue;
					
					remote.port(remote.port() - 1);
					
					// Socket connected
					MsgSocket *node = new MsgSocket(client);
					node->setNonBlocking();
					
					Message msg;
					msg.push_back("@>");
					for (nit = qd->connected.begin();
					     nit != qd->connected.end(); ++nit)
						msg.push_back((string) nit->second);
					node->send(msg);
					
					qd->connected[remote] = CliqueNode(node, remote);
					
					bool cond = INCOMMING;
					qd->entry.push(remote);
					if (!cond) pthread_cond_broadcast(&qd->incomming);
				}
				else
				{
					// Process incomming messages
					CliqueNode *node = qd->find(sock);
					if (!node) continue;
					
					Message msg;
					while (!(msg = sock->recv()).empty())
					{
						string cmd = msg[0];
						if (cmd == "@>")
						{
							Message::iterator it;
							Address remote;
							for (it = msg.begin() + 1; it != msg.end(); ++it)
							{
								remote = Address(string(*it).c_str());
								
								if (qd->connected.count(remote))
									continue;
								
								qd->connecting.insert(remote);
							}
							
							if (qd->connecting.empty() && !DECIDED)
							{
								qd->connection = CliqueData::cnOpen;
								pthread_cond_broadcast(&qd->decided);
							}
							
							set<Address>::iterator it2;
							for (it2 = qd->connecting.begin();
							     it2 != qd->connecting.end(); ++it2)
							{
								MsgSocket *sock = new MsgSocket();
								if (!sock
								||  !sock->reuse()
								||  !sock->bind(qd->port + 1)
								||  !sock->connect(*it2)
								||  !sock->setNonBlocking())
								{
									//printf("err: %d\n", WSAGetLastError());
									valid = false;
									break;
								}
								
								qd->entry.push(*it2);
								qd->connected[*it2] = CliqueNode(sock, *it2);
								qd->lost.erase(*it2);
							}
							qd->connecting.clear();
						}
						else if ((cmd == "@!") || (cmd == "@!!"))
						{
							// A node sent a disconnect request
							if (msg.size() == 1)
							{
								if (qd->find(sock))
								{
									sock->close();
									qd->closed(sock, true);
									break;
								}
								continue;
							}
						
							// Update the connection status
							for (nit = qd->connected.begin();
							     nit != qd->connected.end();)
							{
								if (!nit->second.sock->valid())
								{
									// Socket disconnected
									qd->closed(nit->second.sock);
									nit = qd->connected.begin();
								}
								else
									++nit;
							}
							
							// A loss notification has arrived
							Address remote = Address(string(msg[1]).c_str());
							if (qd->connected.count(remote))
							{
								if (cmd != "@!!")
								{
									Message msg;
									msg.push_back("@>");
									for (nit = qd->connected.begin();
									     nit != qd->connected.end(); ++nit)
										if (&nit->second != node)
											msg.push_back((string) nit->second);
									node->sock->send(msg);
								}
							}
							else if (!qd->lost.count(remote))
							{
								if (cmd != "@!!")
								{
									msg[0] = "@!!";
									for (nit = qd->connected.begin();
									     nit != qd->connected.end(); ++nit)
										nit->second.sock->send(msg);
								}
							}
							else
								qd->loose(remote);
						}
						else
						{
							bool cond = INCOMMING;
							qd->msgs.push(make_pair(node->remote, msg));
							if (!cond) pthread_cond_broadcast(&qd->incomming);
						}
					}
				}
			}
		}
		pthread_mutex_unlock(&qd->lock);
	}
	
	clique->close();
	
	pthread_exit(0);
	return (NULL);
}

//------------------------------------------------------------------------------

void Clique::reset()
{
	VPRIV(CliqueData, qd)
	
	pthread_cancel(qd->thread);
	
	void *status;
	pthread_join(qd->thread, &status);
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if (pthread_create(&qd->thread, &attr, process, (void *) this))
		close();
	
	pthread_attr_destroy(&attr);
}

//------------------------------------------------------------------------------

CliqueNode *CliqueData::find(MsgSocket *sock) const
{
	CliqueData::NodeList::const_iterator it;
	for (it = connected.begin(); it != connected.end(); ++it)
		if (it->second.sock == sock)
			break;
	
	if (it == connected.end())
		return (NULL);
	else
		return const_cast<CliqueNode *>(&(it->second));
}

//------------------------------------------------------------------------------

void CliqueData::closed(MsgSocket *sock, bool reply)
{
	CliqueNode *node = find(sock);
	if (!node) return;
	
	Message msg;
	msg.push_back(reply  ? "@!!" : "@!");
	msg.push_back((string) *node);
	
	loose(node->remote);
	connected.erase(node->remote);
	delete sock;
	
	NodeList::iterator nit;
	for (nit = connected.begin(); nit != connected.end(); ++nit)
		nit->second.sock->send(msg);
}

//------------------------------------------------------------------------------

void CliqueData::loose(const Address &remote)
{
	if (lost.count(remote))
		++lost[remote].first;
	else
		lost[remote] = pair<size_t,time_t>(1,time(NULL) + 3);
	
	CliqueData *qd = this;
	if ((lost[remote].first >= connected.size() - 1)
	||  (lost[remote].second <= time(NULL)))
	{
		lost.erase(remote);
		
		bool cond = INCOMMING;
		loss.push(remote);
		if (!cond) pthread_cond_broadcast(&incomming);
	}
}

//------------------------------------------------------------------------------

CliqueNode::operator string() const
{
	char buffer[128];
	remote.string(buffer);
	return string(buffer);
}

//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
