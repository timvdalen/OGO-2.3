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

//! Text based stream socket
struct TextSocket : public Net::TCPSocket
{
	TextSocket() : TCPSocket(), buffer() {}
	TextSocket(TextSocket &S) : TCPSocket(S), buffer() {}
	TextSocket(TCPSocket &S) : TCPSocket(S), buffer() {}
	
	//! Sends a text message
	bool send(const std::string &msg);
	//! Receives a text message
	std::string recv();
	
	private:
	std::string buffer;	
};

//------------------------------------------------------------------------------

struct Message;
struct Argument;

//! Message based socket
struct MsgSocket : public TextSocket
{
	MsgSocket() : TextSocket() {}
	MsgSocket(MsgSocket &S) : TextSocket(S) {}
	MsgSocket(TCPSocket &S) : TextSocket(S) {}
	
	//! Sends a structured message
	bool send(const Message &msg);
	//! Receives a structured message
	Message recv();
};

//------------------------------------------------------------------------------

//! A structured textual message
//! \note Only the last message may contain spaces and may start with a colon.
struct Message : public std::vector<Argument>
{
	Message() {}
	Message(const std::string &);
	
	bool eof() const;   //!< Returns whether an end of file(stream) was detected
	bool empty() const; //!< Returns whether the message is empty
	
	operator std::string() const;
};

//------------------------------------------------------------------------------

//! A message argument
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
