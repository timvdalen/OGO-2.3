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

struct Cuboid : public Object
{
	Vector<double> u, v, w;

	Cuboid(Pd origin = Pd(), double S = 1) : Object(origin),
	                                         u(Vd(S,0,0)),
	                                         v(Vd(0,S,0)),
	                                         w(Vd(0,0,S))
		{ material = Materials::ShadedMaterial(Cf(1,0,0,1)); }

	void rotate(Quaternion<double> Q) { Qd q = ~Q; u = q*u ;v = q*v; w = q*w; }

	virtual void draw()
	{
		#define Vert(v) { Pd vt = (v); glVertex3d(vt.x,vt.y,vt.z); }
		#define Norm(u,v) { Vd n = ~((u)*(v)); glNormal3d(n.x,n.y,n.z); }
		#define A { glTexCoord2d(0.0,0.0); }
		#define B { glTexCoord2d(1.0,0.0); }
		#define C { glTexCoord2d(1.0,1.0); }
		#define D { glTexCoord2d(0.0,1.0); }

		Pd o = Vd(0,0,0);
		Pd a = o + u + v + w;

		glBegin(GL_QUADS);
			Norm(u,w); A Vert(o); B Vert(o+u); C Vert(o+u+w); D Vert(o+w);
			Norm(v,u); A Vert(o); B Vert(o+v); C Vert(o+v+u); D Vert(o+u);
			Norm(w,v); A Vert(o); B Vert(o+w); C Vert(o+w+v); D Vert(o+v);
			Norm(u,v); A Vert(a); B Vert(a-u); C Vert(a-u-v); D Vert(a-v);
			Norm(v,w); A Vert(a); B Vert(a-v); C Vert(a-v-w); D Vert(a-w);
			Norm(w,u); A Vert(a); B Vert(a-w); C Vert(a-w-u); D Vert(a-u);
		glEnd();

		#undef Vert
		#undef Norm
		#undef A
		#undef B
		#undef C
		#undef D
	}
};

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

	controller = new Controller(v1.camera, player, 100, 100);
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

	/**/
	controller->frame();
	/** /
	if (controller->move[dirLeft])     cube->origin.x -= 0.1;
	if (controller->move[dirRight])    cube->origin.x += 0.1;
	if (controller->move[dirBackward]) cube->origin.y -= 0.1;
	if (controller->move[dirForward])  cube->origin.y += 0.1;
	if (controller->move[dirUp])       cube->origin.z += 0.1;
	if (controller->move[dirDown])     cube->origin.z -= 0.1;

	window->viewports[0]->camera.lookAt(cube->origin);
	/** /
	Camera &cam = window->viewports[0]->camera;
	Pd &pos = cam.origin;
	Qd &rot = cam.objective;
	
	if (controller->look[dirLeft])     rot = rot * Rd(-.05, Vd(0,0,1));
	if (controller->look[dirRight])    rot = rot * Rd( .05, Vd(0,0,1));
	if (controller->look[dirBackward]) rot = rot * Rd(-.05, Vd(0,1,0));
	if (controller->look[dirForward])  rot = rot * Rd( .05, Vd(0,1,0));
	if (controller->look[dirUp])       rot = rot * Rd(-.05, Vd(1,0,0));
	if (controller->look[dirDown])     rot = rot * Rd( .05, Vd(1,0,0));
	
	if (controller->move[dirLeft])     pos = pos + -rot * Vd(1,0,0) * -0.5;
	if (controller->move[dirRight])    pos = pos + -rot * Vd(1,0,0) *  0.5;
	if (controller->move[dirBackward]) pos = pos + -rot * Vd(0,1,0) * -0.5;
	if (controller->move[dirForward])  pos = pos + -rot * Vd(0,1,0) *  0.5;
	if (controller->move[dirUp])       pos = pos + -rot * Vd(0,0,1) *  0.5;
	if (controller->move[dirDown])     pos = pos + -rot * Vd(0,0,1) * -0.5;
	/**/
	
	Objects::Player * player = TO(Objects::Player,controller->player);
	player->update(controller->camera.objective);

	cube->rotation = cube->rotation * Rd(.1, Vd(0,0,1));
	controller->look.reset();
	window->render();
	fps();
}

//------------------------------------------------------------------------------
void printFps(){
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
		controller->setView(lastview);
	}else{
		//Save view before entering building mode
		lastview = controller->getView();
		terrain->showGrid = true;
		controller->setView(true);
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
				Terrain::GridPoint clicked = world->terrain->getGridCoordinates(cam.origin, cam.origin + -cam.objective * Vd(0,10,0));
				
				//Dit zou ik eigenlijk met std::to_string() willen doen
				stringstream ss;
				ss << "Gridpoint (" << clicked.x << ", " << clicked.y << ") was clicked";
				
				world->hud->messageDisplayer->addMessage(SystemMessage(ss.str()));
			}
		}else{
			input->grabMouse();
		}
	}else{
		input->releaseMouse();
	}
}