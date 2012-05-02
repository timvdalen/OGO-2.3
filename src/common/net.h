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

#ifdef WIN32
	#define ssize_t unsigned long int
#endif

namespace Net {

//------------------------------------------------------------------------------

void Initialize();
void Terminate();

//------------------------------------------------------------------------------

struct Address
{
	Address();
	Address(unsigned long address, unsigned port = 0);
	Address(const char *address, unsigned int port = 0);
	~Address();
	
	friend struct UDPSocket;
	friend struct TCPSocket;
	
	private:
	size_t length;
	void *data;
};

//------------------------------------------------------------------------------

struct Socket
{
	//Address local;
	//Address remote;
	
	bool setBlocking();
	bool setNonBlocking();
	
	protected:
	void *data;
};

//------------------------------------------------------------------------------

struct UDPSocket : public Socket
{
	UDPSocket();
	~UDPSocket();
	
	bool bind(const Address &local);
	bool bind(unsigned int port);
	bool broadcast();
	
	ssize_t sendto(const Address &remote, const char *data, size_t length);
	ssize_t shout(unsigned int port, const char *data, size_t length);
	ssize_t recvfrom(Address &remote, char *data, size_t length);
};

//------------------------------------------------------------------------------

struct TCPSocket : public Socket
{
	TCPSocket();
	~TCPSocket();
	
	bool connect(const Address &remote);
	bool listen(unsigned int port);
	bool listen(const Address &local);
	TCPSocket accept(Address &remote);
	
	ssize_t send(const char *data, size_t length);
	ssize_t recv(char *data, size_t length);
};

//------------------------------------------------------------------------------

} // namespace Net

#endif // _NET_H

//------------------------------------------------------------------------------
