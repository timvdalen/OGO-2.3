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

ObjectHandle npc;
int lastmess = 0;

bool building = false;
bool lastview = false;

void Frame();
void KeyUp(Button btn);
void KeyDown(Button btn);
void MouseMove(word x, word y);
void handleMouse(bool left);
void updatePlayers();

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	//Config file
	ConfigFile config("game.conf");
	string pname;
	config.readInto(pname, "playername");

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
	ObjectHandle player = Objects::Player(0, 'a', pname);
	player->rotation = Rd(0,Vd(0,0,1));
	Player *p = TO(Player, player);
	p->weapon = weapLaser;	
	npc = Objects::Player(1, 'b', "NPC", Pd(30, 40, 0));

	cube->material = Assets::Test;

	ObjectHandle world = Objects::World(100, 150);

	{
		World *w = TO(World, world);
		w->hud->messageDisplayer->addMessage(SystemMessage("Loading assets..."));
		w->hud->messageDisplayer->addMessage(SystemMessage("Everything loaded!"));
		w->hud->messageDisplayer->addMessage(SystemMessage("Welcome to the game"));
		w->children.insert(cube);
	}

	world->children.insert(player);
	world->children.insert(npc);
	
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
		world->terrain->setSelected(world->terrain->getGridCoordinates(cam.origin, cam.origin + -cam.objective * Vd(0,10,0)));
	
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
			NetCode::Move(player->origin, velocity);
			break;
		case 2:  NetCode::Look(player->rotation); break;
		case 5: loop = 0; break;
	}
	
	int time = Video::ElapsedTime();
	Objects::Player * pNPC = TO(Objects::Player, npc);
	World *w = TO(World, controller->world);
	HUD *h = TO(HUD, w->hud);
	if(time > 900 && time < 1100){
		if(lastmess == 0){
			ChatMessage m = ChatMessage(*pNPC, "Hey man, welcome to the game!");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 2900 && time < 3100){
		if(lastmess == 1){
			ChatMessage m = ChatMessage(*pNPC, "So your name is " + player->name + "?");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 4900 && time < 5100){
		if(lastmess == 2){
			ChatMessage m = ChatMessage(*pNPC, "Try to look around with the mouse");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 6900 && time < 7100){
		if(lastmess == 3){
			ChatMessage m = ChatMessage(*pNPC, "If you press W you will roll in the direction");
			ChatMessage m2 = ChatMessage(*pNPC, "you're looking in");
			h->messageDisplayer->addMessage(m);
			h->messageDisplayer->addMessage(m2);
			lastmess++;
		}
	}else if(time > 8900 && time < 9100){
		if(lastmess == 4){
			ChatMessage m = ChatMessage(*pNPC, "Left clicking will make you shoot a laser beam");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 10900 && time < 11100){
		if(lastmess == 5){
			ChatMessage m = ChatMessage(*pNPC, "Pressing B will enter build mode");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 12900 && time < 13100){
		if(lastmess == 6){
			ChatMessage m = ChatMessage(*pNPC, "Try to build some towers");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 13900 && time < 14100){
		if(lastmess == 7){
			ChatMessage m = ChatMessage(*pNPC, "They're of a special stealth");
			ChatMessage m2 = ChatMessage(*pNPC, "kind that looks like a box.");
			h->messageDisplayer->addMessage(m);
			h->messageDisplayer->addMessage(m2);
			lastmess++;
		}
	}else if(time > 16900 && time < 17100){
		if(lastmess == 8){
			ChatMessage m = ChatMessage(*pNPC, "Anyway. Have fun!");
			h->messageDisplayer->addMessage(m);
			lastmess++;
		}
	}else if(time > 19900 && time < 20100){
		if(lastmess == 9){
			ChatMessage m = ChatMessage(*pNPC, "Oh, one last thing. You can press [enter] if");
			ChatMessage m2 = ChatMessage(*pNPC, "you ever want to talk to me!");
			h->messageDisplayer->addMessage(m);
			h->messageDisplayer->addMessage(m2);
			lastmess++;
		}
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
	
	binds.processDown(btn);
	
	//Handle input
	switch (btn)
	{
		case btnMouseRight: handleMouse(false);               	 break;
		case btnMouseLeft:  handleMouse(true);                   break;
	}
}

//------------------------------------------------------------------------------

void MouseMove(word x, word y)
{
	//printf("mouse move: %d %d\n", x, y);
}

//------------------------------------------------------------------------------

void handleMouse(bool left){
	//If this ever gets called from anywhere but KeyDown, remember to check
	//For !input
	if(left){
		if(!input->grabbing){
			input->grabMouse();
		}
	}else{
		input->releaseMouse();
	}
}

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
