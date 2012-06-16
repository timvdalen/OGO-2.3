/*
 * Main module
 *
 * Date: 01-05-12 15:01
 *
 * Description:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>
#include <string>

#include "ConfigFile.h"

#include "common.h"
#include "netalg.h"
#include "objects.h"
#include "world.h"
#include "structures.h"
#include "materials.h"
#include "video.h"
#include "input.h"
#include "movement.h"
#include "game.h"
#include "netcode.h"
#include "minion.h"

using namespace Core;
using namespace Base::Alias;
using namespace Movement;

Window *window = NULL;
Controller *controller = NULL;
Input *input = NULL;
map<Button,Direction> movebind;
map<Button,Direction> lookbind;
map<Button,word> lookcount;
ObjectHandle cube;

int lastmess = 0;

bool building = false;
bool lastview = false;

void Frame();
void KeyUp(Button btn);
void KeyDown(Button btn);
void MouseMove(word x, word y);
void updatePlayers();

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	string pname;
	char team;
	//Config file
	try{
		ConfigFile config("game.conf");
		config.readInto(pname, "playername", string("Unnamed"));
		team = config.read("team", 'a');
	}catch(ConfigFile::file_not_found e){
		printf("No config file found. Please add a game.conf (in INI format) with values player and team.");
		pname = string("Unnamed");
		team = 'a';
	}

	//parse arguments...
	Video::Initialize(argc, argv);
	window = new Video::Window(800, 600, GAME_NAME, false);
	Video::Viewport v1(1,1);
	window->viewports.push_back(&v1);

	Assets::Initialize(argc, argv); // after the viewports have been initialized!

	Game::Initialize(argc, argv);
	Game::Exec("inputs.exec");

	Net::Initialize();
	NetCode::Initialize(argc, argv);

	v1.camera.origin = Pd(0,0.1,0);
	v1.camera.objective = Rd(0.0 * Deg2Rad, Vd(0,0,1));

	cube = Cuboid(Pd(0,3,0));
	ObjectHandle player = Objects::Player(0, team, pname);
	player->rotation = Rd(0,Vd(0,0,1));
	Player *p = TO(Player, player);
	p->weapon = weapLaser;

	cube->material = Assets::Test;

	ObjectHandle world = Objects::World(200, 200);

	{
		World *w = TO(World, world);
		w->hud->messageDisplayer->addMessage(SystemMessage("Loading assets..."));
		w->hud->messageDisplayer->addMessage(SystemMessage("Everything loaded!"));
		w->hud->messageDisplayer->addMessage(SystemMessage("Welcome to the game"));
	
		ArenaGuard g1 = ArenaGuard(world, Pd(-100, -100, 0), Qd());
		ArenaGuard g3 = ArenaGuard(world, Pd(100, 100, 0), Qd());
		DefenseMinion d1 = DefenseMinion(player, world, Pd(50, 50, 0), Qd());
		world->children.insert(g1);
		world->children.insert(g3);
		world->children.insert(d1);

		w->children.insert(cube);
	}

	world->children.insert(player);
	
	v1.world = world;

	v1.camera.lookAt(cube->origin);

	controller = new Controller(v1.camera, player, world);
	input = new Input(*window);
	input->onKeyUp = KeyUp;
	input->onKeyDown = KeyDown;
	input->onMouseMove = MouseMove;

	// hack
	Game::game.world = TO(World, world);
	Game::game.player = TO(Player, player);
	Game::game.window = window;
	Game::game.input = input;
	Game::game.controller = controller;

	OnFrame = Frame;

	Game::game.world->terrain->placeStructure(GridPoint(7,14), Mine());
	Game::game.world->terrain->placeStructure(GridPoint(8,14), ResourceMine(BoundingBox(), player));

	Video::StartEventLoop();

	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

int loop = 0;

void Frame()
{
	if (!input) return;
	if (!window) return;
	if (!controller) return;

	NetCode::Frame();
	if (NetCode::TryLock())
	{
		// Critical section

		NetCode::Unlock();
	}

	World *world = TO(World, window->viewports[0]->world);
	Camera &cam = window->viewports[0]->camera;

	if(Game::game.player->weapon == weapWrench)
		world->terrain->setSelected(world->terrain->getGridCoordinates(cam.origin, cam.objective));

	if (window->resized)
	{
		uword width, height;
		window->size(width = 0, height = 0);
		world->hud->resize(width, height);
	}

	controller->frame();

	Objects::Player * player = TO(Objects::Player,controller->player);
	player->update(controller->camera.objective);
	Vd velocity;
	switch (++loop)
	{
		case 1: 
			velocity = ((-player->rotation)*Vd(0,1,0))*controller->move[dirY];
			if(!velocity){
				velocity.z = 0;
				velocity = ~velocity;
			}
			if(controller->blocked){
				velocity = Vd(0,0,0);
			}
			NetCode::Move(player->origin, velocity);
			break;
		case 2:  NetCode::Look(player->rotation); break;
		case 5: loop = 0; break;
	}
	updatePlayers();
	window->render();
}

//------------------------------------------------------------------------------

void KeyUp(Button btn)
{
	//printf("key up: %d\n", btn);

	if (!controller) return;

	binds.processUp(btn);
}

//------------------------------------------------------------------------------

void KeyDown(Button btn)
{
	//printf("key down: %d\n", btn);

	if (!controller) return;
	if (!input) return;

	if(btn >= btnMouseLeft){
		if(input->grabbing){
			if(btn == btnMouseRight){
				input->releaseMouse();
			}else{
				binds.processDown(btn);
			}
		}else{
			if(btn == btnMouseLeft)
				input->grabMouse();
		}
	}else{
		binds.processDown(btn);
	}
}

//------------------------------------------------------------------------------

void MouseMove(word x, word y)
{
	//printf("mouse move: %d %d\n", x, y);
}

//------------------------------------------------------------------------------

void updatePlayers(){
	map<Player::Id,ObjectHandle>::iterator it;
	for (it = Game::game.players.begin(); it != Game::game.players.end(); ++it)
	{
		Player *p = TO(Player,it->second);
		if(!p){
			return;
		}
		p->interpolate();
	}
}

//------------------------------------------------------------------------------
