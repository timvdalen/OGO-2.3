/*
 * Network module -- see header file
 */

#include <string.h>

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
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
unsigned long Address::debug() { if (data) { return *((unsigned long int *) &((sockaddr_in *) data)->sin_addr); } }
//------------------------------------------------------------------------------

Address::Address(unsigned long address, unsigned port)
	 : data(new sockaddr_in), length(sizeof (sockaddr_in))
{
	if (data)
	{
		memset(data, 0, length);
		sockaddr_in *addr = (sockaddr_in *)data;
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
			sockaddr_in *addr = (sockaddr_in *)data;
			addr->sin_addr = result->ai_addr;
			addr->sin_port = htons(port);
		}
		
		freeaddrinfo(result);
	}
}

//------------------------------------------------------------------------------

} // namespace Net

//------------------------------------------------------------------------------
