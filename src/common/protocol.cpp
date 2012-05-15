/*
 * Protocol module -- see header file
 */

#include <stdio.h>
#include <stdlib.h>

#include "protocol.h"

namespace Protocol {

//------------------------------------------------------------------------------
	
bool TextSocket::send(const std::string &input)
{
	std::string msg = input;
	msg += "\r\n";
	size_t len = msg.length();
	return (TCPSocket::send(msg.c_str(), len));
}

//------------------------------------------------------------------------------

std::string TextSocket::recv()
{
	if (!valid())
		return ("\r\n\r\n");
	
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
		if (!pos)
		{
			close();
			msg = buffer;
			buffer.clear();
			return msg;
		}
		
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
	
	return ("\r\n"); // Would block or Socket error (check valid)
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

bool Message::eof() const
{
	return ((size() < 1) || (std::string) (*this)[0] == "\r\n\r\n");
}

//------------------------------------------------------------------------------

bool Message::empty() const
{
	return (eof() || (std::string) (*this)[0] == "\r\n");
}

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
