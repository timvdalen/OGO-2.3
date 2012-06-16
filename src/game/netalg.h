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
//! An algorithm that maintains a complete connection graph between nodes.
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

typedef unsigned long NodeID; //!< Unique number assigned to connected nodes

//! Token ring socket
//! An algorithm that maintains fifo and mutual exclusion properties among a
//! complete connection graph.
class TokenRing
{
	public:
	typedef Net::Address Address;
	
	//! Creates a token ring that listens for candidate nodes on the given port
	TokenRing(unsigned short port);
	//! Destroys the local node
	~TokenRing();
	
	//! Connects to another token ring by specified address
	//! \note this method will fail when already connected
	//! \note check for entries after connect succeeded
	bool connect(const Address &remote, int timeout = 0);
	//! Closes the local connection
	void close();
	
	//! Returns the connection status of the node
	bool connected() const;
	//! Returns whether this node has the token.
	bool authorized() const;
	//! Returns ths id of the local node
	NodeID id() const;
	
	//! Send a message to all connected nodes
	bool shout(const Message &msg, bool reliable);
	//! Passes the token to the next node in line
	//! \note calling this function when unauthorized is a logical error
	bool pass();
	
	//! Checks for incomming connections
	bool entry(NodeID &node);
	//! Checks for remote connection loss
	bool loss(NodeID &node);
	//! Receives messages
	bool recvfrom(NodeID &node, Message &msg, bool &reliable);
	//! Returns wether a connection inconsistency was detected
	bool tainted();
	
	//! Waits for incomming connections/messages and losses
	//! \warning does not return on token gain
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
