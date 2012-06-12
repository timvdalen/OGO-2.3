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

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
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
	ObjectHandle player = Objects::Player();
	player->rotation = Rd(0,Vd(0,0,1));
	
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

	movebind[btnKeyA] = dirLeft;
	movebind[btnKeyD] = dirRight;
	movebind[btnKeyW] = dirForward;
	movebind[btnKeyS] = dirBackward;

	movebind[btnKeyArrowLeft] = dirLeft;
	movebind[btnKeyArrowRight] = dirRight;
	movebind[btnKeyArrowUp] = dirForward;
	movebind[btnKeyArrowDown] = dirBackward;

	movebind[btnKeySpace] = dirUp;
	movebind[btnKeyC] = dirDown;

	lookbind[btnMouseMoveLeft] = dirLeft;
	lookbind[btnMouseMoveRight] = dirRight;
	lookbind[btnMouseMoveUp] = dirUp;
	lookbind[btnMouseMoveDown] = dirDown;

	lookbind[btnMouseScrollUp] = dirForward;
	lookbind[btnMouseScrollDown] = dirBackward;
	
	lookbind[btnKeyQ] = dirForward;
	lookbind[btnKeyZ] = dirBackward;

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

	input->frame();
	
	World *world = TO(World, window->viewports[0]->world);
	Camera &cam = window->viewports[0]->camera;
	
	if(building)
		world->terrain->selected = world->terrain->getGridCoordinates(cam.origin, cam.origin + -cam.objective * Vd(0,10,0));
	
	if (window->resized)
	{
		uword width, height;
		window->size(width = 0, height = 0);
		world->hud->resize(width, height);
	}
	
	controller->frame();
	
	Objects::Player * player = TO(Objects::Player,controller->player);
	player->update(controller->camera.objective);
	
	switch (++loop)
	{
		case 1:  NetCode::Move(player->origin, Vd()); break;
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
	

	controller->look.reset();
	window->render();
}

//------------------------------------------------------------------------------

void KeyUp(Button btn)
{
	//printf("key up: %d\n", btn);
	
	if (!controller) return;
	
	binds.processUp(btn);
	
	if ((btn >= btnMouseScrollUp) && (btn <= btnMouseMoveDown)) return;

	if (movebind.count(btn)) controller->move[movebind[btn]] = false;
	if (lookbind.count(btn)) controller->look[lookbind[btn]] = false;
}

//------------------------------------------------------------------------------

void KeyDown(Button btn)
{
	//printf("key down: %d\n", btn);
	
	if (!controller) return;
	if (!input) return;
	
	binds.processDown(btn);
	
	//Handle input
	if (movebind.count(btn)) controller->move[movebind[btn]] = true;
	if (lookbind.count(btn)) controller->look[lookbind[btn]] = true;
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
		if(input->grabbing){
			if(building){
				//Build something
				Video::Viewport *v = window->viewports.front();
				World *world = TO(World, v->world);
				Camera &cam = v->camera;
				GridPoint clicked = world->terrain->getGridCoordinates(cam.origin, cam.origin + -cam.objective * Vd(0,10,0));
				
				ObjectHandle tower = Objects::DefenseTower(10);
				{
					MaterialHandle m = TwinMaterial(ShadedMaterial(Cf(0.5, 0.5, 0.5, 1)), Assets::Test);
					tower->material = m;
				}
                stringstream ss;
                if(clicked.isValid()){
                    ss << "Invalid Place selected, building aborted";
                }else{
                    bool done = world->terrain->placeStructure(clicked, tower);
				
                    //Dit zou ik eigenlijk met std::to_string() willen doen
                    if(done){
                        ss << "Tower placed at  (" << clicked.x << ", " << clicked.y << ")";
                    }else{
                        ss << "Tower place failed, already there at (" << clicked.x << ", " <<  clicked.y << ")";
                    }
				}
				world->hud->messageDisplayer->addMessage(SystemMessage(ss.str()));
                controller->avoidPulverizebyBuilding();
			}else{
				//Shoot
				Player *p = TO(Player, controller->player);
				//This translation was copied from Player::translateModel
				//That information should be accessible
				Pd gunLoc = p->origin + Vd(-0.499,-0.037,1.333);
				World *w = TO(World, controller->world);
				w->addLaserBeam(ObjectHandle(LaserBeam(gunLoc, -controller->camera.objective * Vd(0,10,0))));
			}
		}else{
			input->grabMouse();
		}
	}else{
		input->releaseMouse();
	}
}

//------------------------------------------------------------------------------
