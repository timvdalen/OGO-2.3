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

#include "common.h"
#include "netalg.h"
#include "objects.h"
#include "materials.h"
#include "video.h"
#include "input.h"
#include "movement.h"
#include "hud.h"

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

void Frame();
void KeyUp(Button btn);
void KeyDown(Button btn);
void MouseMove(word x, word y);

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
	Video::Initialize(argc, argv);
	window = new Video::Window(640, 480, "Game");
	Video::Viewport v1(1,1);
	window->viewports.push_back(&v1);
	
	Assets::Initialize(); // after the viewports have been initialized!
	
	v1.camera.origin = Pd(0,0.1,0);
	v1.camera.objective = Rd(0.0 * Deg2Rad, Vd(0,0,1));
	
	cube = Cuboid(Pd(0,3,0));
	ObjectHandle player = Objects::Player();
	player->children.insert(Cuboid(Pd(0,-3,0)));
	player->rotation = Rd(0,Vd(0,0,1));
	
	cube->material = Assets::Test;

	ObjectHandle hud = HUD_objects::HUD(640, 480);
	
	ObjectHandle world = Objects::World(100, 100);

	{
		World *w = dynamic_cast<World *> (&*world);
		w->terrain->showGrid = true;
		w->terrain->selected.x = 4;
		w->terrain->selected.y = 4;
		w->children.insert(cube);
		w->children.insert(hud);
	}
	
	v1.world = world;

	v1.camera.lookAt(cube->origin);
	
	controller = new Controller(v1.camera, player);
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
	
	/**/
	controller->frame();
	/** /
	if (controller->look[dirLeft])     cube->origin.x -= 0.5;
	if (controller->look[dirRight])    cube->origin.x += 0.5;
	if (controller->look[dirBackward]) cube->origin.y -= 0.5;
	if (controller->look[dirForward])  cube->origin.y += 0.5;
	if (controller->look[dirUp])       cube->origin.z -= 0.1;
	if (controller->look[dirDown])     cube->origin.z += 0.1;
	
	Camera &cam = window->viewports[0]->camera;
	Vd dir = cube->origin + Vd(.5,.5,.5) - cam.origin;
	dir.z = 0;
	cam.objective = Rd(Vd(0,1,0), ~dir);
	/**/
	
	controller->look.reset();
	window->render();
	fps();
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
		case btnKeyF:       printf("FPS: %2.f\n", (double) fps); break;
		case btnMouseRight: input->releaseMouse();               break;
		case btnMouseLeft:  input->grabMouse();                  break;
	}
}

//------------------------------------------------------------------------------

void MouseMove(word x, word y)
{
	//printf("mouse move: %d %d\n", x, y);
}

//------------------------------------------------------------------------------
