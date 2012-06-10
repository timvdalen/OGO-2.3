/*
 * Game module -- see header file for more info
 */

#include <stdio.h>
#include <ctype.h>

#include <algorithm>

#include "video.h"
#include "protocol.h"
#include "game.h"
#include "netcode.h"

namespace Game {

using namespace std;
using namespace Protocol;

string to_lower_case(string str);

GameState game;

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
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

//==============================================================================

CMD(Echo, 1, arg, (string) arg[0])
void Echo(string msg)
{
	puts(msg.c_str());
}

//------------------------------------------------------------------------------

CMD(Notice, 1, arg, (string) arg[0])
void Notice(string msg)
{
	printf("*** %s ***\n", msg.c_str());
}

//------------------------------------------------------------------------------

CMD(Prompt, 1, arg, (string) arg[0])
void Prompt(string cmd)
{
}

//------------------------------------------------------------------------------

CMD(Quit, 0, arg)
void Quit()
{
	Video::StopEventLoop();
}

//------------------------------------------------------------------------------

CMD(RQuit, 0, arg, (arg.size() < 1 ? "AAAAaaaRRGG!!" : arg[0]))
void RQuit(string msg)
{
	Say(msg);
	
	// Let player explode
	
	Quit();
}

//------------------------------------------------------------------------------

CMD(Connect, 1, arg, (string) arg[0])
void Connect(string address)
{
	NetCode::Connect(address);
}

//------------------------------------------------------------------------------

CMD(Disconnect, 0, arg)
void Disconnect()
{
	NetCode::Disconnect();
}

//------------------------------------------------------------------------------

CMD(Say, 1, arg, (string) arg[0])
void Say(string msg)
{
}

//------------------------------------------------------------------------------

CMD(TeamSay, 1, arg, (string) arg[0])
void TeamSay(string msg)
{
}

//------------------------------------------------------------------------------

CMD(MoveX, 1, arg, (double) arg[0])
void MoveX(double speed)
{
}

//------------------------------------------------------------------------------

CMD(MoveY, 1, arg, (double) arg[0])
void MoveY(double speed)
{
}

//------------------------------------------------------------------------------

CMD(MoveZ, 1, arg, (double) arg[0])
void MoveZ(double speed)
{
}

//------------------------------------------------------------------------------

CMD(LookX, 1, arg, (double) arg[0])
void LookX(double speed)
{
}

//------------------------------------------------------------------------------

CMD(LookY, 1, arg, (double) arg[0])
void LookY(double speed)
{
}

//------------------------------------------------------------------------------

CMD(LookZ, 1, arg, (double) arg[0])
void LookZ(double speed)
{
}

//------------------------------------------------------------------------------

CMD(Jump, 0, arg)
void Jump()
{
}

//------------------------------------------------------------------------------

CMD(Fire, 0, arg)
void Fire()
{
}

//------------------------------------------------------------------------------

CMD(Build, 0, arg)
void Build()
{
}

//------------------------------------------------------------------------------

CMD(Weapon, 1, arg, (WeaponType) (int) arg[0])
void Weapon(WeaponType weapon)
{
	WeaponType prevWeapon = game.player->weapon;
	if (prevWeapon == weapon) return;
	
	game.player->weapon = weapon;
	if (weapon == weapWrench)
	{
		// Goto build mode
	}
	else if (prevWeapon == weapWrench)
	{
		// Leave build mode
	}
}

//------------------------------------------------------------------------------

CMD(Tool, 1, arg, (ToolType) (int) arg[0])
void Tool(ToolType tool)
{
	ToolType prevTool = game.player->tool;
	if (prevTool == tool) return;
	
	game.player->tool = tool;
}

//------------------------------------------------------------------------------

} // namespace Game

//------------------------------------------------------------------------------
