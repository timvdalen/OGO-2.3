/*
 * Network module -- see header file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
	#ifndef WIN32
		#define WIN32 1
	#endif
#endif

#ifdef WIN32
	#define _WIN32_WINNT 0x0501
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <pthread.h>
	#define ssize_t signed long int
	#define socklen_t int
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <netdb.h>
	#define SOCKET_ERROR -1
	#define SOCKET int
#endif

#ifdef __LP64__
	#define TOSOCK(x) ((SOCKET) reinterpret_cast<long long> (x))
#else
	#define TOSOCK(x) (reinterpret_cast<SOCKET> (x))
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

Address::Address(const Address &address)
	: data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (!data) return;
	memcpy(data, address.data, length);
}

//------------------------------------------------------------------------------

Address::Address(unsigned long address, unsigned short port)
	: data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (!data) return;
	memset(data, 0, length);
	sockaddr_in *addr = (sockaddr_in *)data;
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = htonl(address);
}

//------------------------------------------------------------------------------

Address::Address(const char *address, unsigned short port)
	 : data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (!data) return;
	memset(data, 0, length);
	
	addrinfo *result;
	const char *ptr;
	if (ptr = strchr(address, ':'))
	{
		size_t len = ptr - address;
		
		char *ip = new char[len + 1];
		memcpy(ip, address, len);
		ip[len] = 0;
		
		if (getaddrinfo(ip, NULL, NULL, &result))
		{
			delete[] ip;
			return;
		}
		
		port = atoi(ptr + 1);
		delete[] ip;
	}
	else if (getaddrinfo(address, NULL, NULL, &result))
		return;
	
	if (!result)
		return;
	
	if (result->ai_family == AF_INET)
	{
		memcpy(data, result->ai_addr, result->ai_addrlen);

		if (port)
			((sockaddr_in *) data)->sin_port = htons(port);
	}

	freeaddrinfo(result);
}

//------------------------------------------------------------------------------

Address::~Address()
{
	if (!data)
		return;

	sockaddr_in *addr = (sockaddr_in *) data;
	delete addr;
}

//------------------------------------------------------------------------------

bool Address::name(char *str, size_t len) const
{
	return getnameinfo((sockaddr *) data, length, str, len, 0, 0, 0);
}

//------------------------------------------------------------------------------

bool Address::string(char *str) const
{
	sockaddr_in *addr = (sockaddr_in *) data;
	return (sprintf(str, "%s:%d", inet_ntoa(addr->sin_addr),
	                ntohs(addr->sin_port)) > 0);
}

//------------------------------------------------------------------------------

bool Address::valid() const
{
	if (!data)
		return false;
	
	sockaddr_in *addr = (sockaddr_in *) data;
	return (addr->sin_family == AF_INET);
}

//------------------------------------------------------------------------------

bool Address::operator ==(const Address &addr) const
{
	sockaddr_in *a = (sockaddr_in *) data;
	sockaddr_in *b = (sockaddr_in *) addr.data;
	if ((a->sin_addr.s_addr != b->sin_addr.s_addr)
	||  (a->sin_port != b->sin_port))
		return false;
	else
		return true;
}

//------------------------------------------------------------------------------

bool Address::operator <(const Address &addr) const
{
	sockaddr_in *a = (sockaddr_in *) data;
	sockaddr_in *b = (sockaddr_in *) addr.data;
	if (a->sin_addr.s_addr == b->sin_addr.s_addr)
		return a->sin_port < b->sin_port;
	else
		return a->sin_addr.s_addr < b->sin_addr.s_addr;
}

//------------------------------------------------------------------------------

Address &Address::operator =(const Address &addr)
{
	memcpy(data, addr.data, length);
	return *this;
}
//------------------------------------------------------------------------------

unsigned short Address::port()
{
	sockaddr_in *addr = (sockaddr_in *) data;
	return ntohs(addr->sin_port);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void Address::port(unsigned short port)
{
	sockaddr_in *addr = (sockaddr_in *) data;
	addr->sin_port = htons(port);
}

//------------------------------------------------------------------------------

Socket::Socket() : data((void *) SOCKET_ERROR)
{
}

//------------------------------------------------------------------------------

Socket::Socket(Socket &sock)
{
	data = sock.data;
	sock.data = (void *) SOCKET_ERROR;
}

//------------------------------------------------------------------------------

Socket::~Socket()
{
	close();
}

//------------------------------------------------------------------------------

bool Socket::setBlocking()
{
	if (TOSOCK(data) == SOCKET_ERROR)
		return false;
	
	#ifdef WIN32
		u_long x = 0;
		return (ioctlsocket(TOSOCK(data), FIONBIO, &x) != -1);
	#else
		int x = fcntl(TOSOCK(data), F_GETFL, 0);
		return (fcntl(TOSOCK(data), F_SETFL, x & ~O_NONBLOCK) != -1);
	#endif
}

//------------------------------------------------------------------------------

bool Socket::setNonBlocking()
{
	if (TOSOCK(data) == SOCKET_ERROR)
		return false;
	
	#ifdef WIN32
		u_long x = 1;
		return (ioctlsocket(TOSOCK(data), FIONBIO, &x) != -1);
	#else
		int x = fcntl(TOSOCK(data), F_GETFL, 0);
		return (fcntl(TOSOCK(data), F_SETFL, x | O_NONBLOCK) != -1);
	#endif
}

//------------------------------------------------------------------------------

bool Socket::reuse()
{
	if (TOSOCK(data) == SOCKET_ERROR)
		return false;
	
	#ifdef WIN32
		const char mode = 1;
		return !setsockopt(TOSOCK(data), SOL_SOCKET, SO_REUSEADDR,
			&mode, sizeof (mode));
	#else
		int mode = 1;
		return !setsockopt(TOSOCK(data), SOL_SOCKET, SO_REUSEADDR,
			&mode, sizeof (int));
	#endif
}

//------------------------------------------------------------------------------

bool Socket::bind(const Address &address)
{
	return !::bind(TOSOCK(data), (const sockaddr *) address.data,
		address.length);
}

//------------------------------------------------------------------------------

bool Socket::bind(unsigned short port)
{
	return bind(Address((long unsigned int)htonl(INADDR_ANY), port));
}

//------------------------------------------------------------------------------

void Socket::close()
{
	if (TOSOCK(data) == SOCKET_ERROR)
		return;

	#ifdef WIN32
		closesocket(TOSOCK(data));
	#else
		::close(TOSOCK(data));
	#endif

	data = (void *) SOCKET_ERROR;
}

//------------------------------------------------------------------------------

bool Socket::valid() const
{
	return (TOSOCK(data) != SOCKET_ERROR);
}

//------------------------------------------------------------------------------

bool Socket::select(Socket::List &read, Socket::List &write,
	Socket::List &error, long timeout)
{
	int maxfd = 0;

	timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	fd_set rfds, wfds, efds;
	
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (!read.empty())
	{
		for (List::iterator it = read.begin(); it != read.end(); ++it)
		{
			SOCKET fd = TOSOCK((*it)->data);
			FD_SET(fd, &rfds);
			maxfd = maxfd < fd ? fd : maxfd;
		}
	}

	if (!write.empty())
	{
		for (List::iterator it = write.begin(); it != write.end(); ++it)
		{
			SOCKET fd = TOSOCK((*it)->data);
			FD_SET(fd, &wfds);
			maxfd = maxfd < fd ? fd : maxfd;
		}
	}

	if (!error.empty())
	{
		for (List::iterator it = error.begin(); it != error.end(); ++it)
		{
			SOCKET fd = TOSOCK((*it)->data);
			FD_SET(fd, &efds);
			maxfd = (maxfd < fd ? fd : maxfd);
		}
	}
	
	#ifdef WIN32 // Windows will not cancel threads during select, curses!
	int ret;
	fd_set _rfds = rfds, _wfds = wfds, _efds = efds;
	for (;;)
	{
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		rfds = _rfds, wfds = _wfds, efds = _efds;
		
		pthread_testcancel();
		
		ret = ::select(maxfd + 1, read.empty() ? NULL : &rfds,
		                          write.empty() ? NULL : &wfds,
						          error.empty() ? NULL : &efds,
						          &tv);
		
		if ((ret != 0) || (timeout == 1))
			break;
		
		if (timeout > 0)
			--timeout;
	}
	#else
	int ret = ::select(maxfd + 1, read.empty() ? NULL : &rfds,
	                              write.empty() ? NULL : &wfds,
					              error.empty() ? NULL : &efds,
					              timeout > 0 ? &tv : NULL);
	#endif
	
	if (ret == SOCKET_ERROR)
		return false;

	if (!read.empty())
		for (List::iterator it = read.begin(); it != read.end(); ++it)
			if (!FD_ISSET(TOSOCK((*it)->data), &rfds))
				read.erase(it--);

	if (!write.empty())
		for (List::iterator it = write.begin(); it != write.end(); ++it)
			if (!FD_ISSET(TOSOCK((*it)->data), &wfds))
				write.erase(it--);

	if (!error.empty())
		for (List::iterator it = error.begin(); it != error.end(); ++it)
			if (!FD_ISSET(TOSOCK((*it)->data), &efds))
				error.erase(it--);

	return true;
}
//------------------------------------------------------------------------------

UDPSocket::UDPSocket()
{
	data = (void *)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

//------------------------------------------------------------------------------

UDPSocket::UDPSocket(UDPSocket &sock)
{
	data = sock.data;
	sock.data = (void *) SOCKET_ERROR;
}

//------------------------------------------------------------------------------

UDPSocket::~UDPSocket()
{
	close();
}

//------------------------------------------------------------------------------

bool UDPSocket::broadcast()
{
	if (TOSOCK(data) == SOCKET_ERROR)
		return false;
	
	#ifdef WIN32
		const char mode = 1;
		return !setsockopt(TOSOCK(data), SOL_SOCKET, SO_BROADCAST,
			&mode, sizeof (mode));
	#else
		int mode = 1;
		return !setsockopt(TOSOCK(data), SOL_SOCKET, SO_BROADCAST,
			&mode, sizeof (int));
	#endif
}

//------------------------------------------------------------------------------

bool UDPSocket::sendto(const Address &address, const char *data_in, size_t &length)
{
	#ifdef NETDEBUG
		printf("%03X> %s\n", data, data_in);
	#endif
	ssize_t ret = ::sendto(TOSOCK(data), data_in, length, 0,
		(sockaddr *) address.data, address.length);
	if (ret == length)
		return true;

	length = ret;
	return false;
}

//------------------------------------------------------------------------------

bool UDPSocket::shout(unsigned short port, const char *data_in, size_t &length)
{
	Address address(INADDR_BROADCAST, port);
	#ifdef NETDEBUG
		printf("%03X>> %s\n", data, data_in);
	#endif
	ssize_t ret = ::sendto(TOSOCK(data), data_in, length, 0,
		(sockaddr *) address.data, address.length);
	if (ret == length)
		return true;

	length = ret;
	return false;
}

//------------------------------------------------------------------------------

bool UDPSocket::recvfrom(Address &address, char *data_out, size_t &length)
{
	ssize_t ret = ::recvfrom(TOSOCK(data), data_out, --length, 0,
		(sockaddr *) address.data, (socklen_t *) &address.length);
	if (ret >= 0)
	{
		length = ret;
		data_out[length] = 0;
		#ifdef NETDEBUG
			printf("%03X< %s\n", data, data_out);
		#endif
		if (!length)
			close();
		return true;
	}
	#ifdef WIN32
		int error = WSAGetLastError();
		if ((error != WSAEINPROGRESS)
		&&  (error != WSAEWOULDBLOCK)
		&&  (error != WSAEMSGSIZE))
			close();
	#else
		if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
			close();
	#endif
	return false;
}

//------------------------------------------------------------------------------

TCPSocket::TCPSocket()
{
	data = (void *)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

//------------------------------------------------------------------------------

TCPSocket::TCPSocket(TCPSocket &sock)
{
	data = sock.data;
	sock.data = (void *) SOCKET_ERROR;
}

//------------------------------------------------------------------------------

TCPSocket::~TCPSocket()
{
	close();
}

//------------------------------------------------------------------------------

bool TCPSocket::connect(const Address &address)
{
	return !::connect(TOSOCK(data),
		(const sockaddr *) address.data, address.length);
}

//------------------------------------------------------------------------------

bool TCPSocket::listen(const Address &address)
{
	if (!bind(address))
		return false;
	return !::listen(TOSOCK(data), 10);
}

//------------------------------------------------------------------------------

bool TCPSocket::listen(unsigned short port)
{
	if (!bind(port))
		return false;
	return !::listen(TOSOCK(data), 10);
}

//------------------------------------------------------------------------------

bool TCPSocket::accept(TCPSocket &socket, Address &address)
{
	SOCKET sock = ::accept(TOSOCK(data),
		(sockaddr *)address.data, (socklen_t *) &address.length);
	socket.data = (void *)sock;
	return (sock != SOCKET_ERROR);
}

//------------------------------------------------------------------------------

bool TCPSocket::send(const char *data_in, size_t &length)
{
	#ifdef NETDEBUG
		printf("%03X> %s\n", data, data_in);
	#endif
	ssize_t ret = ::send(TOSOCK(data), data_in, length, 0);
	if (ret == length)
		return true;

	length = ret;
	return false;
}

//------------------------------------------------------------------------------

bool TCPSocket::recv(char *data_out, size_t &length)
{
	ssize_t ret = ::recv(TOSOCK(data), data_out, --length, 0);
	if (ret >= 0)
	{
		length = ret;
		data_out[length] = 0;
		#ifdef NETDEBUG
			printf("%03X< %s\n", data, data_out);
		#endif
		if (!length)
			close();
		return true;
	}
	#ifdef WIN32
		int error = WSAGetLastError();
		if ((error != WSAEINPROGRESS)
		&&  (error != WSAEWOULDBLOCK)
		&&  (error != WSAEMSGSIZE))
			close();
	#else
		if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
			close();
	#endif
	return false;
}

//------------------------------------------------------------------------------

} // namespace Net

//------------------------------------------------------------------------------
