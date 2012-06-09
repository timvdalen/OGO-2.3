/*
 * Network algorithms
 *
 * Date: 21-05-12 12:25
 *
 * Description: Distributed algorithms that ensure certain properties in
 *              network communication.
 */

#ifndef _NETALG_H
#define _NETALG_H

#include "net.h"
#include "protocol.h"

namespace Protocol {

//------------------------------------------------------------------------------

//! Clique socket.
//! A algorithm that maintains a complete connection graph between nodes.
//! \sa { https://github.com/timvdalen/OGO-2.3/wiki/Dist-Algo%27s }
class Clique
{
	public:
	typedef Net::Address Address;
	
	//! Creates a new clique listening on the specified port
	Clique(unsigned short port);
	//! Terminates the clique
	~Clique();
	
	//! Tries to connect to another clique (or till the timeout occurs)
	//! \note This method will always fail when already connecetd to another clique
	bool connect(const Address &remote, int timeout = 0);
	//! Closes the connection. This will invalidate the instance permanently.
	void close();
	
	//! Returns whether this instance is connected to a clique.
	bool connected() const;
	//! Returns whether a certain node (by ip address) is connected.
	bool connected(const Address &node) const;
	
	//! Sends a messages to a certain node.
	bool sendto(const Address &node, const Message &msg);
	//! Sends a message to all nodes.
	bool shout(const Message &msg);
	
	//! Retrieves new nodes in the supplied address value. Returns success.
	bool entry(Address &node);
	//! Retrieves lost nodes in the supplied address value. Returns success.
	bool loss(Address &node);
	//! Retrieves messages in the supplied values. Returns success.
	bool recvfrom(Address &node, Message &msg);
	
	//! Wait for changes or (when supplied) a timeout, whichever comes first.
	bool select(int timeout = 0);
	
	void debug();
	void debugClose(Address &node);
	
	private:
	void *data;
	static void *process(void *);
	void reset();
};

//------------------------------------------------------------------------------

typedef unsigned long NodeID;

class TokenRing
{
	public:
	typedef Net::Address Address;
	
	TokenRing(unsigned short port);
	~TokenRing();
	
	bool connect(const Address &remote, int timeout = 0);
	void close();
	
	bool connected() const; // Check for entries after connect succeeded
	bool authorized() const;
	NodeID id() const;
	
	bool shout(const Message &msg, bool reliable);
	bool pass();
	
	bool entry(NodeID &node);
	bool loss(NodeID &node);
	bool recvfrom(NodeID &node, Message &msg, bool &reliable);
	
	bool select(int timeout = 0);
	
	void debug();
	
	private:
	Clique *clique;
	void *data;
	static void *process(void *);
};

//------------------------------------------------------------------------------

} // namespace Protocol

#endif // NETALG_H

//------------------------------------------------------------------------------
