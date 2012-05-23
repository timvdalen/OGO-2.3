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

class Clique
{
	public:
	typedef Net::Address Address;
	
	Clique(unsigned short port);
	~Clique();
	
	bool connect(const Address &remote, int timeout = 0);
	void close();
	
	bool connected() const;
	bool connected(const Address &node) const;
	
	bool sendto(const Address &node, const Message &msg);
	bool shout(const Message &msg);
	
	bool entry(Address &node);
	bool loss(Address &node);
	bool recvfrom(Address &node, Message &msg);
	
	bool select(int timeout = 0);
	
	private:
	void *data;
	static void *process(void *);
	void reset();
};

//------------------------------------------------------------------------------

} // namespace Protocol

#endif // NETALG_H

//------------------------------------------------------------------------------
