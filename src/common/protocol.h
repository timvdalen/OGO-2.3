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

#include "net.h"

//! Protocol module
namespace Protocol {

//------------------------------------------------------------------------------

struct TextSocket : public Net::TCPSocket
{
	TextSocket();
	~TextSocket();
	
	TextSocket accept(Net::Address &remote);
	
	bool send(const std::string &msg);
	std::string recv();
	
	private:
		std::string buffer;	
};

//------------------------------------------------------------------------------

} // namespace Protocol

#endif // _PROTOCOL_H

//------------------------------------------------------------------------------
