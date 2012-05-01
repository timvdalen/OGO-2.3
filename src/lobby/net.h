/*
 * Network module
 * 
 * Date: 26-04-12 12:29
 *
 * Description: Maintains a list of players in the selected game
 *
 */

#ifndef _NET_H
#define _NET_H

namespace Net {

//------------------------------------------------------------------------------

void Initialize();
void Terminate();

//------------------------------------------------------------------------------

struct Address
{
	Address();
	Address(unsigned long address = 0, unsigned port = 0);
	Address(const char *address, unsigned int port = 0);
	
	unsigned long debug();
	
	private:
	size_t length;
	void *data;
};

struct Socket
{
	Address local;
	Address remote;
};

struct UDPSocket : public Socket
{
};

struct TCPSocket : public Socket
{
};

//------------------------------------------------------------------------------

} // namespace Net

#endif // _NET_H

//------------------------------------------------------------------------------
