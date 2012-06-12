/*
 * World object -- see header file for more info
 */

#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <GL/freeglut.h>

#include "world.h"

namespace Objects {

#define HIGH 100

class Sky;
class Star;

//------------------------------------------------------------------------------

World::World(double _width, double _height)
	: BoundedObject(Pd(), Qd(),
	  BoundingBox(Pd(), Pd(_width,0,0), Pd(0,_height,0), Pd(_width,_height,0)),
	  Assets::WorldMaterial)
{
	ObjectHandle tHandle;
	tHandle = Terrain(_width, _height);
	terrain = TO(Terrain, tHandle);
	children.insert(tHandle);

	ObjectHandle hudHandle;
	hudHandle = HUD(640, 480, this);
	hud = TO(HUD, hudHandle);
	children.insert(hudHandle);

	children.insert(Sky(_width, _height));

	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void World::draw(){
	double halfWidth = width/2;
	double halfHeight = height/2;


	glBegin(GL_QUADS);
		//Back side
		glNormal3i(0, 1, 0);
		glTexCoord2f(0.33, 0);
		glVertex3f(-halfWidth, -halfHeight, 0.0f);
		glTexCoord2f(0.33, 0.33);
		glVertex3f(-halfWidth, -halfHeight, HIGH);
		glTexCoord2f(0.66, 0.33);
		glVertex3f(halfWidth, -halfHeight, HIGH);
		glTexCoord2f(0.66, 0);
		glVertex3f(halfWidth, -halfHeight, 0.0f);

		//Left side
		glNormal3i(1, 0, 0);
		glTexCoord2f(0, 0.66);
		glVertex3f(-halfWidth, halfHeight, 0.0f);
		glTexCoord2f(0.33, 0.66);
		glVertex3f(-halfWidth, halfHeight, HIGH);
		glTexCoord2f(0.33, 0.33);
		glVertex3f(-halfWidth, -halfHeight, HIGH);
		glTexCoord2f(0, 0.33);
		glVertex3f(-halfWidth, -halfHeight, 0.0f);

		//Right side
		glNormal3i(-1, 0, 0);
		glTexCoord2d(1, 0.33);
		glVertex3f(halfWidth, -halfHeight, 0.0f);
		glTexCoord2d(0.66, 0.33);
		glVertex3f(halfWidth, -halfHeight, HIGH);
		glTexCoord2d(0.66, 0.66);
		glVertex3f(halfWidth, halfHeight, HIGH);
		glTexCoord2d(1, 0.66);
		glVertex3f(halfWidth, halfHeight, 0.0f);

		//Front side
		glNormal3i(0, -1, 0);
		glTexCoord2d(0.66, 1);
		glVertex3f(halfWidth, halfHeight, 0.0f);
		glTexCoord2d(0.66, 0.66);
		glVertex3f(halfWidth, halfHeight, HIGH);
		glTexCoord2d(0.33, 0.66);
		glVertex3f(-halfWidth, halfHeight, HIGH);
		glTexCoord2d(0.33, 1);
		glVertex3f(-halfWidth, halfHeight, 0.0f);

		//Top side
		glNormal3i(0, 0, -1);
		glTexCoord2d(0.66, 0.66);
		glVertex3f(halfWidth, halfHeight, HIGH);
		glTexCoord2d(0.66, 0.33);
		glVertex3f(halfWidth, -halfHeight, HIGH);
		glTexCoord2d(0.33, 0.33);
		glVertex3f(-halfWidth, -halfHeight, HIGH);
		glTexCoord2d(0.33, 0.66);
		glVertex3f(-halfWidth, halfHeight, HIGH);
	glEnd();
}

//------------------------------------------------------------------------------

void World::postRender(){
	//Check for the ttl of all active LaserBeams and render them
	vector<ObjectHandle>::iterator it;
	for(it = laserBeams.begin(); it != laserBeams.end(); ){
		LaserBeam *curr = TO(LaserBeam, *it);
		if(!curr){
			//This situation can only happen when an ObjectHandle is manually
			//added to laserBeams
			it = laserBeams.erase(it);
		}else{
			if(curr->done){
				it = laserBeams.erase(it);
			}else{
				curr->render();
				it++;
			}
		}
	}
	
	//Go on to render children and pop matrix
	Object::postRender();
}

//------------------------------------------------------------------------------

ObjectHandle World::trace(Point<double> origin, Vector<double> &path)
{
	return ObjectHandle();// Todo: implement
}

//------------------------------------------------------------------------------

void World::addLaserBeam(ObjectHandle laserBeam){
	LaserBeam *lb = TO(LaserBeam, laserBeam);
	//Check if the ObjectHandle passed actually points to a LaserBeam
	if(lb){
		laserBeams.push_back(laserBeam);
	}
}

//------------------------------------------------------------------------------

Star::Star(Pd P)
	: Object(P, Qd(), Assets::Star)
{
	big = false;
}

//------------------------------------------------------------------------------

void Star::draw(){
	glBegin(GL_POINTS);
		glVertex3i(0, 0, 0);
		if(big){
			glVertex3f(-0.1, 0, 0);
			glVertex3f(0, -0.1, 0);
			glVertex3f(0.1, 0, 0);
			glVertex3f(0, 0.1, 0);
		}
	glEnd();
}

//------------------------------------------------------------------------------

Sky::Sky(int _width, int _height){
	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void Sky::frame(){
	if(stars.size() < 300){
		int res = rand()%100;
		if(res == 0){
			int x = rand()%width - (int)width/2;
			int y = rand()%height - (int)height/2;
			int min = HIGH - (int)(HIGH/10);
			int z = rand()%(HIGH-min) + min;
			stars.push_back(Star(Pd(x, y, z)));
		}
	}
	if(stars.size() > 150){
		int res = rand()%200;
		if(res == 0){
			stars.erase(stars.begin() + (rand()%stars.size()));
		}
	}
	if(stars.size() > 100){
		int res = rand()%60;
		if(res == 0){
			vector<Star>::iterator it;
			it = stars.begin() + (rand()%stars.size());
			it->big = !it->big;
		}
	}
}

//------------------------------------------------------------------------------

void Sky::render(){
	frame();
	vector<Star>::iterator it;
	for(it = stars.begin(); it != stars.end(); it++){
		(*it).render();
	}
}

//------------------------------------------------------------------------------

#undef HIGH

} // namespace Objects

//------------------------------------------------------------------------------
