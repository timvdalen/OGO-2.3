/*
 * Game module -- see header file for more info
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <vector>
#include <algorithm>

#include "video.h"
#include "protocol.h"
#include "game.h"
#include "netcode.h"

#include "input.h" // debug

namespace Game {

using namespace std;
using namespace Protocol;

static void getInput(string input);
string to_lower_case(string str);

int windowWidth = 640;
int windowHeight = 480;
bool fullscreen = false;

double gameWidth = 100;
double gameHeight = 100;
string path = "./";

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
	for (int i = 0; i < argc - 1; ++i)
	{
		if      (!strcmp(argv[i], "-x") || !strcmp(argv[i], "--map-width"))
			gameWidth = atof(argv[++i]);
		else if (!strcmp(argv[i], "-y") || !strcmp(argv[i], "--map-height"))
			gameHeight = atof(argv[++i]);
		else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--path"))
			path = argv[++i];
		else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--screen-width"))
			windowWidth = atoi(argv[++i]);
		else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--screen-height"))
			windowHeight = atoi(argv[++i]);
	}

	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp(argv[i], "--fullscreen"))
			fullscreen = true;
	}

	srand(time(NULL));

	game.world = new World(gameWidth, gameHeight);
}

//------------------------------------------------------------------------------

void Terminate()
{
	game.player = NULL;
	if (game.world) delete game.world;
	game.world = NULL;
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

//------------------------------------------------------------------------------

static void getInput(string input)
{
	HUD *hud = game.world->hud;

	if(!input.empty()) Game::Call(input);

	set<ObjectHandle>::iterator it;
	TextInput *tInput;
	for (it = hud->children.begin(); it != hud->children.end();)
	{
		tInput = TO(TextInput, *it);
		if (tInput)
		{
			hud->children.erase(it);
			break;
		}
		else
			++it;
	}
}

//==============================================================================

CMD(Echo, 1, arg, (string) arg[0])
void Echo(string msg)
{
	game.world->hud->messageDisplayer->addMessage(SystemMessage(msg));
}

//------------------------------------------------------------------------------

CMD(Notice, 1, arg, (string) arg[0])
void Notice(string msg)
{
	// Temporarily
	Echo(string("*** ") + msg + string(" ***"));
}

//------------------------------------------------------------------------------

CMD(Prompt, 0, arg, (arg.empty() ? "" : (string) arg[0]))
void Prompt(string cmd)
{
	int &width = game.world->hud->width;
	int &height = game.world->hud->height;
	game.input->text = cmd;
	game.input->onText = getInput; // Move this to init
	game.world->hud->children.insert(TextInput(game.input, 0, 0, width, height));
}

//------------------------------------------------------------------------------

CMD(ShowLog, 0, arg)
void ShowLog()
{
	game.world->hud->messageDisplayer->setShowAlways(true);
}

//------------------------------------------------------------------------------

CMD(HideLog, 0, arg)
void HideLog()
{
	game.world->hud->messageDisplayer->setShowAlways(false);
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

CMD(Bind, 2, arg, (string) arg[0], (string) arg[1])
void Bind(string button, string line)
{
	binds.bind(button, line);
}

//------------------------------------------------------------------------------

CMD(Exec, 1, arg, (string) arg[0])
void Exec(string filename)
{
	FILE *fp = fopen((path+filename).c_str(), "rt");
	if (!fp)
	{
		Echo(string("Unable to open script file: " + filename));
		return;
	}

	Echo(string("Executing ") + filename + string("..."));

	char buffer[1024], *ptr;
	while(fgets(buffer, sizeof (buffer), fp))
	{
		ptr = buffer;
		while ((*ptr == ' ') || (*ptr == '\t')) ptr++;
		if (!*ptr || (*ptr == '#')) continue;

		int len = strlen(ptr) - 1;
		if (ptr[len] == '\n') ptr[len--] = 0;
		if (ptr[len] == '\r') ptr[len--] = 0;

		if (len >= 0) Call(string(ptr));
	}
	fclose(fp);
}

//------------------------------------------------------------------------------

CMD(Connect, 1, arg, (string) arg[0])
void Connect(string address)
{
	Game::Notice(string("Connecting to ") + address + string("..."));
	if (!NetCode::Connect(address))
		Game::Notice(string("Unable to connect to " + address + string("!")));
	else
	{
		Game::Notice(string("Connected to " + address + string("!")));

	}
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
	NetCode::Chat(msg);
	DisplayChatMsg(game.player, msg);
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
	game.controller->move[dirX] = speed;
	//game.controller->moveX(speed);
}

//------------------------------------------------------------------------------

CMD(MoveY, 1, arg, (double) arg[0])
void MoveY(double speed)
{
	game.controller->move[dirY] = speed;
	game.controller->moveY(speed);
}

//------------------------------------------------------------------------------

CMD(MoveZ, 1, arg, (double) arg[0])
void MoveZ(double speed)
{
	game.controller->move[dirZ] = speed;
	game.controller->moveZ(speed);
}

//------------------------------------------------------------------------------

CMD(LookX, 1, arg, (double) arg[0])
void LookX(double speed)
{
	game.controller->look[dirX] = speed;
	game.controller->lookX(speed);
}

//------------------------------------------------------------------------------

CMD(LookY, 1, arg, (double) arg[0])
void LookY(double speed)
{
	game.controller->look[dirY] = speed;
	game.controller->lookY(speed);
}

//------------------------------------------------------------------------------

CMD(LookZ, 1, arg, (double) arg[0])
void LookZ(double speed)
{
	game.controller->look[dirZ] = speed;
	game.controller->lookZ(speed);
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
	switch(game.player->weapon){
	case weapWrench:{
			Build();
			return;
		}
		break;
	case weapLaser:{
            Camera &cam = game.controller->camera;
			Vd vec = ~(game.player->rotation * Vd(0,1,0));
			double yaw = atan2(vec.x, vec.y);
			Pd gunLoc = game.player->origin;// + game.player->model.weapon->origin;
			gunLoc.x = gunLoc.x + game.player->model.weapon->origin.x * cos(yaw) + game.player->model.weapon->origin.y * sin(yaw);
			gunLoc.y = gunLoc.y + game.player->model.weapon->origin.x * sin(yaw) + game.player->model.weapon->origin.y * cos(yaw);
			gunLoc.z = gunLoc.z + game.player->model.weapon->origin.z;
			World *w = TO(World, game.controller->world);
			w->addLaserBeam(ObjectHandle(LaserBeam(gunLoc, cam.objective)));
			return;
		}
		break;
	}
}

//------------------------------------------------------------------------------

CMD(Build, 0, arg)
void Build()
{
	Camera &cam = game.controller->camera;
	GridPoint clicked = game.world->terrain->getGridCoordinates(cam.origin, cam.objective);
	if(clicked.isValid()){
		ObjectHandle tower = Objects::DefenseTower(ObjectHandle(*game.player));
		game.world->terrain->setSelected(GridPoint(-1, -1));
		bool done = game.world->terrain->placeStructure(clicked, tower);
		if(!done) game.world->hud->messageDisplayer->addMessage(SystemMessage("There's already a tower there"));
	}else{
		game.world->hud->messageDisplayer->addMessage(SystemMessage("Invalid place to build"));
	}
}

//------------------------------------------------------------------------------

CMD(Weapon, 1, arg, (WeaponType) (int) arg[0])
void Weapon(WeaponType weapon)
{
	WeaponType prevWeapon = game.player->weapon;
	Terrain *terrain = TO(Terrain, game.world->terrain);
	if (prevWeapon == weapon) return;

	game.player->weapon = weapon;
	if (weapon == weapWrench)
	{
		// Goto build mode
		terrain->showGrid = true;
		game.world->hud->buildselector->show = true;
		//game.controller->setView(true);
		// Todo: add controller
	}
	else if (prevWeapon == weapWrench)
	{
		// Leave build mode
		terrain->showGrid = false;
		game.world->hud->buildselector->show = false;
		game.world->terrain->setSelected(GridPoint(-1, -1));
		//game.controller->restoreView();
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

void DisplayChatMsg(Player *player, string line)
{
	if (!player) return;

	game.world->hud->messageDisplayer->addMessage(ChatMessage(*player, line));
}

//------------------------------------------------------------------------------

void DisplayTeamMsg(Player *player, string line)
{
	if (!player) return;

	game.world->hud->messageDisplayer->addMessage(ChatMessage(*player, line));
}

//------------------------------------------------------------------------------

CMD(PrintFPS, 0, arg)
void PrintFPS()
{
    Echo(string("Current FPS: ") + Argument((double) CurrentFPS()).str);
}

//------------------------------------------------------------------------------

CMD(PrintCPS, 0, arg)
void PrintCPS()
{
    Echo(string("Current CPS: ") + Argument((double) NetCode::CurrentCPS()).str);
}

//------------------------------------------------------------------------------

CMD(NetDebug, 0, arg)
void NetDebug()
{
	NetCode::Debug();
}

//==============================================================================

void Chain(string line)
{
	size_t pos;
	while ((pos = line.find(';')) != string::npos)
	{
		size_t pos2 = pos;
		while (line[--pos]  == ' ');
		while (line[++pos2] == ' ');
		Call(line.substr(0, pos + 1));
		line = line.substr(pos2);
	}
	Call(line);
}
CMD(Chain, 1, arg, (string) arg[0])

//------------------------------------------------------------------------------

struct Toggler
{
	vector<string> cmd;
	int last;
	Toggler() : last(0) {}
};

map<string,Toggler> toggles;

void Toggle(string line)
{
	if (!toggles.count(line))
	{
		Toggler &toggler = toggles[line];

		size_t pos;
		while ((pos = line.find('^')) != string::npos)
		{
			size_t pos2 = pos;
			while (line[--pos]  == ' ');
			while (line[++pos2] == ' ');
			toggler.cmd.push_back(line.substr(0, pos + 1));
			line = line.substr(pos2);
		}
		toggler.cmd.push_back(line);

		Call(toggler.cmd[toggler.last++]);
		toggler.last %= toggler.cmd.size();
		return;
	}

	Toggler &toggler = toggles[line];
	Call(toggler.cmd[toggler.last++]);
	toggler.last %= toggler.cmd.size();
}
CMD(Toggle, 1, arg, (string) arg[0])

//------------------------------------------------------------------------------

} // namespace Game

//------------------------------------------------------------------------------
