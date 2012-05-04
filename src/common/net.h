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
	//! Creates an address from raw types
	Address(unsigned long address, unsigned port = 0);
	//! Creates an addres from a string
	Address(const char *address, unsigned int port = 0);
	//! Cleans up the address
	~Address();
	
	friend struct UDPSocket;
	friend struct TCPSocket;
	
	bool operator <(const Address &) const;
	
	private:
	size_t length;
	void *data;
};

//------------------------------------------------------------------------------

//! Basic internet socket.
struct Socket
{
	//Address local;
	//Address remote;
	
	bool setBlocking();    //!< Makes operations on this socket block. (default)
	bool setNonBlocking(); //!< Makes operations on this socket not block.
	
	protected:
	void *data;
};

//------------------------------------------------------------------------------

//! A message based connectionless internet socket
struct UDPSocket : public Socket
{
	UDPSocket();
	~UDPSocket();
	
	//! Binds the socket to a local address.
	bool bind(const Address &local);
	//! Binds this socket to a local address with specified port.
	bool bind(unsigned int port);
	//! Allows this socket to use broadcast. \sa shout
	bool broadcast();
	
	//! Sends a message to a remote host.
	ssize_t sendto(const Address &remote, const char *data, size_t length);
	//! Send a message to all remote hosts in the subnet.
	ssize_t shout(unsigned int port, const char *data, size_t length);
	//! Receives a message (from the local bound address). \sa broadcast
	ssize_t recvfrom(Address &remote, char *data, size_t length);
};

//------------------------------------------------------------------------------

//! A stream based connection internet socket
struct TCPSocket : public Socket
{
	TCPSocket();
	~TCPSocket();
	
	//! Connects to a remote host.
	bool connect(const Address &remote);
	//! Makes this socket a listening socket to the specified port.
	bool listen(unsigned int port);
	//! Makes this socket a listening socket to bound to the specified address.
	bool listen(const Address &local);
	//! Accepts a incomming connection (when this is a listening socket). \sa listen
	TCPSocket accept(Address &remote);
	
	//! Sends a message to the remote host.
	ssize_t send(const char *data, size_t length);
	//! Receives a message from the remote host.
	ssize_t recv(char *data, size_t length);
};

//------------------------------------------------------------------------------

} // namespace Net

#endif // _NET_H

//------------------------------------------------------------------------------
