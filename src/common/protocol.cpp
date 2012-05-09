/*
 * Protocol module -- see header file
 */

#include "protocol.h"

namespace Protocol {

//------------------------------------------------------------------------------

TextSocket::TextSocket() : TCPSocket(), buffer("")
{
}

//------------------------------------------------------------------------------

TextSocket::~TextSocket()
{
}

//------------------------------------------------------------------------------

TextSocket TextSocket::accept(Net::Address &remote)
{
	TCPSocket sock = TCPSocket::accept(remote);
	TextSocket textsock;
	memcpy(&textsock, &sock, sizeof (sock));
	memset(&sock, 0, sizeof (sock));
	return (textsock);
}

//------------------------------------------------------------------------------
	
bool TextSocket::send(const std::string &input)
{
	std::string msg = input;
	msg += "\r\n";
	size_t len = msg.length();
	return (TCPSocket::send(msg.c_str(), len) == len);
}

//------------------------------------------------------------------------------

std::string TextSocket::recv()
{
	std::string msg;
	size_t pos = buffer.find("\r\n");
	if (pos != std::string::npos)
	{
		msg = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
		return msg;
	}
	
	char buf[1024];
	while (TCPSocket::recv(buf, pos = sizeof (buf)))
	{
		buf[pos] = 0;
		buffer += buf;
		
		size_t pos = buffer.find("\r\n");
		if (pos != std::string::npos)
		{
			msg = buffer.substr(0, pos);
			buffer.erase(0, pos + 2);
			return msg;
		}
	}
	
	return (" ");
}

//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
