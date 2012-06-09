/*
 * Game module -- see header file for more info
 */

#include <stdio.h>
#include <ctype.h>

#include <algorithm>

#include "protocol.h"
#include "game.h"

namespace Game {

using namespace std;
using namespace Protocol;

string to_lower_case(string str);

//------------------------------------------------------------------------------

class Command
{
	public:
	typedef void (*Func)(const Message &);
	typedef map<string, pair<Func,size_t> > List;
	
	static List list;
	
	Command(string name, Func func, size_t count)
		{ list[to_lower_case(name)] = make_pair(func,count); }
};

Command::List Command::list;

#define CMD(name, count, arg, ...)                                   \
	void _ ## name(const Message &arg) { name(__VA_ARGS__); }        \
	Command _cmd_ ## name(#name, _ ## name, count);
	

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[])
{
}

//------------------------------------------------------------------------------

void Terminate()
{
}

//------------------------------------------------------------------------------

void Call(string command)
{
	Protocol::Message args = command;
	if (args.size() < 1) return;
	
	string cmd = to_lower_case(args[0]);
	args.erase(args.begin());
	
	if (!Command::list.count(cmd))
		Echo(string("Unknown command: ") + cmd);
	else
	{
		pair<Command::Func,size_t> func = Command::list[cmd];
		if (args.size() < func.second)
			Echo(string("To few arguments: ") + cmd);
		else
			func.first(args);
	}
}

//------------------------------------------------------------------------------

bool Callable(std::string command)
{
	Protocol::Message args = command;
	if (args.size() < 1) return false;
	
	string cmd = to_lower_case(args[0]);
	
	return !!Command::list.count(cmd);
}

//------------------------------------------------------------------------------

string to_lower_case(string str)
{
	transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

//==============================================================================

CMD(Echo, 1, arg, (string) arg[0])
void Echo(string msg)
{
	puts(msg.c_str());
}

//------------------------------------------------------------------------------

} // namespace Game

//------------------------------------------------------------------------------
