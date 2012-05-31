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

using namespace Core;
using namespace Base::Alias;
using namespace Movement;

Window *window = NULL;
Controller *controller = NULL;
map<Input::Button,Controller::Direction> movebind;
map<Input::Button,Controller::Direction> lookbind;

void Frame();
void KeyUp(Input::Button btn);
void KeyDown(Input::Button btn);
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
		
		Pd o = Vd(0,0,0);
		Pd a = o + u + v + w;
		
		glBegin(GL_QUADS);
			Norm(u,w); Vert(o); Vert(o+u); Vert(o+u+w); Vert(o+w);
			Norm(v,u); Vert(o); Vert(o+v); Vert(o+v+u); Vert(o+u);
			Norm(w,v); Vert(o); Vert(o+w); Vert(o+w+v); Vert(o+v);
			Norm(u,v); Vert(a); Vert(a-u); Vert(a-u-v); Vert(a-v);
			Norm(v,w); Vert(a); Vert(a-v); Vert(a-v-w); Vert(a-w);
			Norm(w,u); Vert(a); Vert(a-w); Vert(a-w-u); Vert(a-u);
		glEnd();
		
		#undef Vert
		#undef Norm
	}
};

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	Video::Initialize(argc, argv);
	window = new Video::Window(640, 480, "Game");
	Video::Viewport v1(1,1);
	window->viewports.push_back(&v1);
	
	v1.camera.origin = Pd(0,0,0);
	v1.camera.objective = Rd(0.0 * Deg2Rad, Vd(0,0,1));
	
	ObjectHandle cube = Cuboid(Pd(0,3,0));
	ObjectHandle player = Objects::Player();
	
	Materials::ShadedMaterial mat = Materials::ShadedMaterial(Cf(1,0,0,1));
	mat.emissive = Cf(0,.5,0,1);
	cube->material = mat;
	
	v1.world = Objects::World(100,100);
	v1.world->children.insert(cube);
	
	v1.camera.lookAt(cube->origin);
	
	controller = new Controller(v1.camera, player);
	Input input(*window);
	input.onKeyUp = KeyUp;
	input.onKeyDown = KeyDown;
	input.onMouseMove = MouseMove;
	
	movebind[Input::btnKeyA] = Controller::dirLeft;
	movebind[Input::btnKeyD] = Controller::dirRight;
	movebind[Input::btnKeyW] = Controller::dirForward;
	movebind[Input::btnKeyS] = Controller::dirBackward;
	
	movebind[Input::btnKeyArrowLeft] = Controller::dirLeft;
	movebind[Input::btnKeyArrowRight] = Controller::dirRight;
	movebind[Input::btnKeyArrowUp] = Controller::dirForward;
	movebind[Input::btnKeyArrowDown] = Controller::dirBackward;
	
	movebind[Input::btnKeySpace] = Controller::dirUp;
	movebind[Input::btnKeyC] = Controller::dirDown;
	
	OnFrame = Frame;
	
	Video::StartEventLoop();
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

void Frame()
{
	if (!window) return;
	if (!controller) return;
	
	controller->frame();
	window->render();
}

//------------------------------------------------------------------------------

void KeyUp(Input::Button btn)
{
	printf("key up: %d\n", btn);
	if (!controller) return;
	
	if (movebind.count(btn))
		controller->move[movebind[btn]] = false;
	
	if (lookbind.count(btn))
		controller->look[lookbind[btn]] = false;
}

//------------------------------------------------------------------------------

void KeyDown(Input::Button btn)
{
	printf("key down: %d\n", btn);
	if (!controller) return;
	
	if (movebind.count(btn))
		controller->move[movebind[btn]] = true;
	
	if (lookbind.count(btn))
		controller->look[lookbind[btn]] = true;
}

//------------------------------------------------------------------------------

void MouseMove(word x, word y)
{
}

//------------------------------------------------------------------------------
