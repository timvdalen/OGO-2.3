#include "world.h"

namespace Objects {

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
	hudHandle = HUD(640, 480);
	hud = TO(HUD, hudHandle);
	children.insert(hudHandle);

	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void World::draw(){
	#define HIGH 100

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

	#undef HIGH
}

//------------------------------------------------------------------------------

ObjectHandle World::trace(Point<double> origin, Vector<double> &path)
{
	// Todo: implement
}

}