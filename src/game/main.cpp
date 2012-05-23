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
	                                         w(Vd(0,0,S)) {}
	
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
	
	Video::Viewport *v1 = new Video::Viewport(1, 1);
	v1->world = Objects::World(100,100);
	ObjectHandle cube = Cuboid(Pd(0,0,0));
	v1->world->children.insert(cube);
	
	w1.viewports.insert(v1);
	
	Video::StartEventLoop();
	
	/*{
		Net::Initialize();
		
		unsigned short port = 23;
		scanf("%d", &port);
		
		Protocol::Clique cq(port);
		while (cq.connected())
		{
			printf("> ");
			char buffer[128];
			fgets(buffer, sizeof (buffer), stdin);
			if (*buffer == '>')
				cq.connect(Net::Address(buffer + 1));
			else
				cq.shout(std::string(buffer));
			
			if (cq.select(1))
			{
				Net::Address remote;
				Protocol::Message msg;
				if (cq.recvfrom(remote, msg))
				{
					remote.string(buffer);
					printf("%s > %s\n", buffer, ((std::string) msg).c_str());
				}
				if (cq.entry(remote))
				{
					remote.string(buffer);
					printf("%s connected!\n", buffer);
				}
				if (cq.loss(remote))
				{
					remote.string(buffer);
					printf("%s disconnected!\n", buffer);
				}
			}
		}
		puts("Connection closed!");
		
		Net::Terminate();
	}*/
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
