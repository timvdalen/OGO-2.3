/*
 * Protocol module -- see header file
 */

#include "protocol.h"

namespace Protocol {

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
	
	return ("\r\n");
}

//------------------------------------------------------------------------------

Argument::Argument(long i)
{
	char buffer[32];
	sprintf(buffer, "%d", i);
	str = std::string(buffer);
}

//------------------------------------------------------------------------------

Argument::Argument(int i)
{
	char buffer[32];
	sprintf(buffer, "%d", i);
	str = std::string(buffer);
}

//------------------------------------------------------------------------------

Argument::Argument(double f)
{
	char buffer[32];
	sprintf(buffer, "%f", f);
	str = std::string(buffer);
}

//------------------------------------------------------------------------------

Argument::operator std::string() const
{
	return str;
}

//------------------------------------------------------------------------------

Argument::operator int() const
{
	return atoi(str.c_str());
}

//------------------------------------------------------------------------------

Argument::operator long() const
{
	return atol(str.c_str());
}

//------------------------------------------------------------------------------

Argument::operator double() const
{
	return atof(str.c_str());
}

//------------------------------------------------------------------------------

Message::Message(const std::string &msg)
{
	size_t begin = 0;
	for (size_t pos = msg.find(" ", 0);
	     pos != std::string::npos;
		 pos = msg.find(" ", pos))
	{
		push_back(msg.substr(begin, pos - begin));
		begin = ++pos;
		
		if (msg[pos] == ':')
		{
			++begin;
			break;
		}
	}
	push_back(msg.substr(begin));
}

//------------------------------------------------------------------------------

Message Message::eof = Message("\r\n");

//------------------------------------------------------------------------------

Message::operator std::string() const
{
	std::string str;
	
	Message::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
		if ((it + 1) == end())
			str += std::string(":") + it->str;
		else
			str += it->str + std::string(" ");
	}
	
	return (str);
}

//------------------------------------------------------------------------------

MsgSocket MsgSocket::accept(Net::Address &remote)
{
	TCPSocket sock = TCPSocket::accept(remote);
	MsgSocket msgsock;
	memcpy(&msgsock, &sock, sizeof (sock));
	memset(&sock, 0, sizeof (sock));
	return (msgsock);
}

//------------------------------------------------------------------------------

bool MsgSocket::send(const Message &msg)
{
	return TextSocket::send(msg);
}

//------------------------------------------------------------------------------

Message MsgSocket::recv()
{
	return Message(TextSocket::recv());
}

//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
