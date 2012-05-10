/*
 * Protocol module
 * 
 * Date: 26-04-12 12:29
 *
 * Description: Abstraction for communication protocols
 *
 */

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <string>
#include <vector>

#include "net.h"

//! Protocol module
namespace Protocol {

//------------------------------------------------------------------------------

//! TCP 
struct TextSocket : public Net::TCPSocket
{
	TextSocket() : TCPSocket(), buffer() {}
	
	TextSocket accept(Net::Address &remote);
	
	bool send(const std::string &msg);
	std::string recv();
	
	private:
	std::string buffer;	
};

//------------------------------------------------------------------------------

struct Message;
struct Argument;

struct MsgSocket : public TextSocket
{
	MsgSocket accept(Net::Address &remote);
	
	bool send(const Message &msg);
	Message recv();
};

//------------------------------------------------------------------------------

struct Message : public std::vector<Argument>
{
	Message() {}
	Message(const std::string &);
	
	operator std::string() const;
	
	static Message eof;
};

//------------------------------------------------------------------------------

struct Argument
{
	Argument(const std::string &S) : str(S) {}
	Argument(const char *S) : str(S) {}
	Argument(int);
	Argument(long);
	Argument(double);
	
	operator std::string() const;
	operator int() const;
	operator long() const;
	operator double() const;
	
	std::string str;
};

//------------------------------------------------------------------------------

} // namespace Protocol

#endif // _PROTOCOL_H

//------------------------------------------------------------------------------
