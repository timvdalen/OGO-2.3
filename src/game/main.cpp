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

using namespace Core;
using namespace Base::Alias;

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
	Video::Window w1(640, 480, "Game");
	Video::Viewport v1(1,1);
	w1.viewports.push_back(&v1);
	
	v1.camera.origin = Pd(0,0,0);
	v1.camera.objective = Rd(0.0 * Deg2Rad, Vd(0,0,1));
	
	ObjectHandle cube = Cuboid(Pd(1,3,-7));
	
	Materials::ShadedMaterial mat = Materials::ShadedMaterial(Cf(1,0,0,1));
	mat.emissive = Cf(0,.5,0,1);
	cube->material = mat;
	
	v1.world = Objects::World(100,100);
	v1.world->children.insert(cube);
	
	v1.camera.lookAt(cube->origin);
	
	Video::StartEventLoop();
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
