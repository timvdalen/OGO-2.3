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

using namespace Core;
using namespace Base::Alias;
using namespace Movement;

Window *window = NULL;
Controller *controller = NULL;
Input *input = NULL;
map<Button,Direction> movebind;
map<Button,Direction> lookbind;
map<Button,word> lookcount;
FPS fps;
ObjectHandle cube;
bool building = false;
bool lastview = false;

void Frame();
void KeyUp(Button btn);
void KeyDown(Button btn);
void MouseMove(word x, word y);
void toggleBuild();
void handleMouse(bool left);

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    //parse arguments...
    int width = 640;
    int height = 480;
    bool fullscreen = false;
	for (int i = 0; i < argc; ++i){
		if (!strcmp(argv[i], "-width") && i < argc - 1){
			std::stringstream converter(argv[++i]);
            converter >> width;
            if (converter.fail()) {
                width = 640;
            }
        }else if(!strcmp(argv[i], "-height") && i < argc - 1){
            std::stringstream converter(argv[++i]);
            converter >> height;
            if(converter.fail()){
                height = 480;
            }
        }else if(!strcmp(argv[i], "-fullscreen")){
            fullscreen = true;
        }
    }
	
	srand(time(NULL));
	
    Video::Initialize(argc, argv);
	window = new Video::Window(width, height, "Game", fullscreen);
	Video::Viewport v1(1,1);
	window->viewports.push_back(&v1);

	Assets::Initialize(argc, argv); // after the viewports have been initialized!

	v1.camera.origin = Pd(0,0.1,0);
	v1.camera.objective = Rd(0.0 * Deg2Rad, Vd(0,0,1));

	cube = Cuboid(Pd(0,3,0));
	ObjectHandle player = Objects::Player();
	ObjectHandle playercube = Cuboid(Pd(-.5,-.5,-1));
	playercube->material = ShadedMaterial(Cf(1,0,0,1));
	player->children.insert(playercube);
	player->rotation = Rd(0,Vd(0,0,1));

	cube->material = Assets::Test;

	ObjectHandle world = Objects::World(100, 100);

	{
		World *w = TO(World, world);
		w->hud->messageDisplayer->addMessage(SystemMessage("Loading assets..."));
		w->hud->messageDisplayer->addMessage(SystemMessage("Everything loaded!"));
		w->hud->messageDisplayer->addMessage(SystemMessage("Welcome to the game"));
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

void Frame()
{
	if (!input) return;
	if (!window) return;
	if (!controller) return;

	input->frame();
	
	World *world = dynamic_cast<World *>(&*window->viewports[0]->world);
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

	cube->rotation = cube->rotation * Rd(.1, Vd(0,0,1));
	controller->look.reset();
	window->render();
	fps();
}

//------------------------------------------------------------------------------

void printFps()
{
    World *world = TO(World, (window->viewports.front())->world);
    stringstream ss;
    ss << "Current FPS: " << (double)fps;
    world->hud->messageDisplayer->addMessage(SystemMessage(ss.str()));
}

//------------------------------------------------------------------------------

void KeyUp(Button btn)
{
	//printf("key up: %d\n", btn);
	if (!controller) return;

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

	if (movebind.count(btn)) controller->move[movebind[btn]] = true;
	if (lookbind.count(btn)) controller->look[lookbind[btn]] = true;

	switch (btn)
	{
		case btnKeyEscape:  Video::StopEventLoop();              break;
		case btnKeyF:       printFps();                          break;
		case btnKeyB:       toggleBuild(); 						 break;
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

void toggleBuild(){
	Video::Viewport *v = window->viewports.front();
	World *world = TO(World, v->world);
	Terrain *terrain = TO(Terrain, world->terrain);
	
	if(building){
		terrain->showGrid = false;
		//Restore view
		controller->firstPerson = lastview;
	}else{
		//Save view before entering building mode
		lastview = controller->firstPerson;
		terrain->showGrid = true;
		controller->firstPerson = true;
	}
	building = !building;
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
				
				Handle<Structure> tower = Objects::DefenseTower(10);
				{
					MaterialHandle m = TwinMaterial(ShadedMaterial(Cf(0.5, 0.5, 0.5, 1)), Assets::Test);
					tower->material = m;
				}
                stringstream ss;
                if(clicked){
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
				
				toggleBuild();
			}else{
				//Shoot
				Player *p = TO(Player, controller->player);
				controller->world->children.insert(LaserBeam(p->origin, Vd(0.57, 0.57, -0.57)));
                controller->avoidPulverizebyBuilding();
			}
		}else{
			input->grabMouse();
		}
	}else{
		input->releaseMouse();
	}
}
