/*
 * Network module -- see header file
 */

#include <string.h>

#ifdef WIN32
	#define _WIN32_WINNT 0x0501 
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#define SOCKET int
	#define closesocket close
#endif

#include "net.h"

namespace Net {

#ifdef WIN32
WSADATA wsa;
#endif

//------------------------------------------------------------------------------

void Initialize()
{
	#ifdef WIN32
	WSAStartup(MAKEWORD(2, 2), &wsa);
	#endif
}

//------------------------------------------------------------------------------

void Terminate()
{
	#ifdef WIN32
	WSACleanup();
	#endif
}

//------------------------------------------------------------------------------

Address::Address() : data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (data)
		memset(data, 0, length);
}

//------------------------------------------------------------------------------

Address::Address(unsigned long address, unsigned port)
	 : data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (data)
	{
		memset(data, 0, length);
		sockaddr_in *addr = (sockaddr_in *)data;
		addr->sin_family = AF_INET;
		addr->sin_port = htons(port);
		addr->sin_addr.s_addr = htonl(address);
	}
}

//------------------------------------------------------------------------------

Address::Address(const char *address, unsigned int port)
	 : data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (data)
	{
		memset(data, 0, length);
		
		addrinfo *result;
		if (getaddrinfo(address, NULL, NULL, &result))
			return;
		
		if (result && (result->ai_family == AF_INET))
		{
			memcpy(data, result->ai_addr, result->ai_addrlen);
			
			if (port)
				((sockaddr_in *) data)->sin_port = htons(port);
		}
		
		freeaddrinfo(result);
	}
}

//------------------------------------------------------------------------------

Address::~Address()
{
	if (data)
		delete (sockaddr_in *)data;
}

//------------------------------------------------------------------------------

bool Socket::setBlocking()
{
	#ifdef WIN32
		u_long x = 0;
		return (ioctlsocket((SOCKET) data, FIONBIO, &x) != -1);
	#else
		int x = fcntl((SOCKET) data, F_GETFL, 0);
		return (fcntl((SOCKET) data, F_SETFL, x & ~O_NONBLOCK) != -1);
	#endif
}

//------------------------------------------------------------------------------

bool Socket::setNonBlocking()
{
	#ifdef WIN32
		u_long x = 1;
		return (ioctlsocket((SOCKET) data, FIONBIO, &x) != -1);
	#else
		int x = fcntl((SOCKET) data, F_GETFL, 0);
		return (fcntl((SOCKET) data, F_SETFL, x | O_NONBLOCK) != -1);
	#endif
}

//------------------------------------------------------------------------------

UDPSocket::UDPSocket()
{
	data = (void *)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

//------------------------------------------------------------------------------

UDPSocket::~UDPSocket()
{
	if (data)
		closesocket((SOCKET) data);
}

//------------------------------------------------------------------------------

bool UDPSocket::bind(const Address &address)
{
	return !::bind((SOCKET) data, (const sockaddr *) address.data,
		address.length);
}

//------------------------------------------------------------------------------

bool UDPSocket::bind(unsigned int port)
{
	return bind(Address(htonl(INADDR_ANY), port));
}

//------------------------------------------------------------------------------

bool UDPSocket::broadcast()
{
	#ifdef WIN32
		const char mode = 1;
		return !setsockopt((SOCKET) data, SOL_SOCKET, SO_BROADCAST,
			&mode, sizeof (mode));
	#else
		int mode = 1;
		return !setsockopt((SOCKET) data, SOL_SOCKET, SO_BROADCAST,
			&mode, sizeof (int));
	#endif
}

//------------------------------------------------------------------------------

ssize_t UDPSocket::sendto(const Address &address, const char *data_in, size_t length)
{
	return ::sendto((SOCKET) data, data_in, length, 0,
		(sockaddr *) address.data, address.length);
}

//------------------------------------------------------------------------------

ssize_t UDPSocket::shout(unsigned int port, const char *data_in, size_t length)
{
	Address address(INADDR_BROADCAST, port);
	return ::sendto((SOCKET) data, data_in, length, 0,
		(sockaddr *) address.data, address.length);
}

//------------------------------------------------------------------------------

ssize_t UDPSocket::recvfrom(Address &address, char *data_out, size_t length)
{
	return ::recvfrom((SOCKET) data, data_out, length, 0,
		(sockaddr *) address.data, (int *) &address.length);
}

//------------------------------------------------------------------------------

TCPSocket::TCPSocket()
{
	data = (void *)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

//------------------------------------------------------------------------------

TCPSocket::~TCPSocket()
{
	if (data)
		closesocket((SOCKET) data);
}

//------------------------------------------------------------------------------

bool TCPSocket::connect(const Address &address)
{
	return !::connect((SOCKET) data,
		(const sockaddr *) address.data, address.length);
}

//------------------------------------------------------------------------------

bool TCPSocket::listen(const Address &address)
{
	if (::bind((SOCKET) data, (const sockaddr *) address.data, address.length))
		return false;
	return !::listen((SOCKET) data, 10);
}

//------------------------------------------------------------------------------

bool TCPSocket::listen(unsigned int port)
{
	return listen(Address(htonl(INADDR_ANY), port));
}

//------------------------------------------------------------------------------

TCPSocket TCPSocket::accept(Address &address)
{
	SOCKET sock = ::accept((SOCKET) data,
		(sockaddr *)address.data, (int *) &address.length);
	TCPSocket socket;
	socket.data = (void *)sock;
	return socket;
}

//------------------------------------------------------------------------------

ssize_t TCPSocket::send(const char *data_in, size_t length)
{
	return ::send((SOCKET) data, data_in, length, 0);
}

//------------------------------------------------------------------------------

ssize_t TCPSocket::recv(char *data_out, size_t length)
{
	return ::recv((SOCKET) data, data_out, length, 0);
}

//------------------------------------------------------------------------------

} // namespace Net

//------------------------------------------------------------------------------
