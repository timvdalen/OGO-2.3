/*
 * Network module
 * 
 * Date: 26-04-12 12:29
 *
 * Description: Abstraction for internet communication
 *
 */

#ifndef _NET_H
#define _NET_H

#include <vector>

//! Network module
namespace Net {

//------------------------------------------------------------------------------

void Initialize(); //!< Initializes networking at the start of the program.
void Terminate();  //!< Cleans up networking at the end of the program.

//------------------------------------------------------------------------------

//! An internet address.

//! Address is used to indentify the local and remote sides in communication.
struct Address
{
	//! Creates an empty address
	Address();
	//! Creates a copy of an address
	Address(const Address &);
	//! Creates an address from raw types
	Address(unsigned long address, unsigned port = 0);
	//! Creates an addres from a string
	Address(const char *address, unsigned int port = 0);
	//! Cleans up the address
	~Address();
	
	//! Gets the hostname the address points to
	bool name(char *str, size_t len) const;
	//! Gets a textual representation of an address
	bool string(char *str) const;

	friend struct UDPSocket;
	friend struct TCPSocket;
	
	bool operator ==(const Address &) const;
	bool operator <(const Address &) const;
	Address &operator =(const Address &);
	
	private:
	size_t length;
	void *data;
};

//------------------------------------------------------------------------------

//! Basic internet socket.
struct Socket
{
	Socket();
	//! Copies a socket: this will invalidate the original socket.
	Socket(Socket &);
	~Socket();
	
	bool setBlocking();    //!< Makes operations on this socket block. (default)
	bool setNonBlocking(); //!< Makes operations on this socket not block.
	
	void close(); //!< Closes the socket. This will invalidate the instance.
	//! Returns the validity of the socket.
	bool valid() const;
	
	//! Socket list
	typedef std::vector<Socket *> List;
	
	//! Checks a list of sockets for change.
	//! Note: The lists will reflect the changed sockets afterwards.
	static bool select(Socket::List &read,
	                   Socket::List &write,
				       Socket::List &error, long timeout = 0);
	
	void operator =(Socket &); // Not allowed
	
	protected:
	void *data;
};

//------------------------------------------------------------------------------

//! A message based connectionless internet socket
struct UDPSocket : public Socket
{
	UDPSocket();
	UDPSocket(UDPSocket &);
	~UDPSocket();
	
	//! Binds the socket to a local address.
	bool bind(const Address &local);
	//! Binds this socket to a local address with specified port.
	bool bind(unsigned int port);
	//! Allows this socket to use broadcast. \sa shout
	bool broadcast();
	
	//! Sends a message to a remote host.
	bool sendto(const Address &remote, const char *data, size_t &length);
	//! Send a message to all remote hosts in the subnet.
	bool shout(unsigned int port, const char *data, size_t &length);
	//! Receives a message (from the local bound address). \sa broadcast
	bool recvfrom(Address &remote, char *data, size_t &length);
};

//------------------------------------------------------------------------------

//! A stream based connection internet socket
struct TCPSocket : public Socket
{
	TCPSocket();
	TCPSocket(TCPSocket &);
	~TCPSocket();
	
	//! Connects to a remote host.
	bool connect(const Address &remote);
	//! Makes this socket a listening socket to the specified port.
	bool listen(unsigned int port);
	//! Makes this socket a listening socket to bound to the specified address.
	bool listen(const Address &local);
	//! Accepts a incomming connection (when this is a listening socket). \sa listen
	bool accept(TCPSocket &client, Address &remote);
	
	//! Sends a message to the remote host.
	bool send(const char *data, size_t &length);
	//! Receives a message from the remote host.
	bool recv(char *data, size_t &length);
};

//------------------------------------------------------------------------------

} // namespace Net

#endif // _NET_H

//------------------------------------------------------------------------------
