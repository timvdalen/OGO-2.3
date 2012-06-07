/*
 * Objects -- see header
 */

#include <stdio.h>
#include "objects.h"
#include "video.h"
#include "assets.h"

namespace Objects {

//------------------------------------------------------------------------------

ObjectHandle BoundedObject::checkCollision(Point<double> origin, Vector<double> direction)
{
	return (Object());
}

//------------------------------------------------------------------------------

World::World(double _width, double _height)
	: BoundedObject(Pd(), Qd(),
	  BoundingBox(Pd(), Pd(_width,0,0), Pd(0,_height,0), Pd(_width,_height,0)),
	  Assets::Cloud)
{
	children.insert(Terrain(_width, _height));
	terrain = dynamic_cast<Terrain *>(&**children.begin());
	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void World::draw(){
	#define HIGH 500
	
	double halfWidth = width/2;
	double halfHeight = height/2;


	glBegin(GL_TRIANGLES);
		glTexCoord2d(0, 0);
		glVertex3f(-1 * halfWidth, -1 * halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(-1 * halfWidth, halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(halfWidth, halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(halfWidth, -1* halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(-1 * halfWidth, halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(halfWidth, halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(halfWidth, -1 * halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(-1 * halfWidth, -1 * halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);
		
	glEnd();

	#undef HIGH
}

//------------------------------------------------------------------------------

Terrain::Terrain(double _width, double _height) 
	: Object(Pd(), Qd(), Assets::Grass)
{
	width = _width;
	height = _height;

	showGrid = false;
	selected.x = -1;
	selected.y = -1;
}

//------------------------------------------------------------------------------

void Terrain::draw()
{
	int noX = (int) width/GRID_SIZE;
	int noY = (int) height/GRID_SIZE;

	//Draw all columns
	for(int i=0; i < noX; i++){
		//Draw all cells in this column
		for(int j=0; j < noY; j++){
			double halfWidth = width/2;
			double halfHeight = height/2;
			glBegin(GL_QUADS);
				glTexCoord2d(0, 0);
				glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
				glTexCoord2d(1, 0);
				glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
				glTexCoord2d(1, 1);
				glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
				glTexCoord2d(0, 1);
				glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
			glEnd();
			if(showGrid){
				MaterialHandle gridMat;
				if(selected.x == i && selected.y == j){
					gridMat = Assets::SelectedGrid;
				}else{
					gridMat = Assets::Grid;
				}
				material->unselect();
				gridMat->select();
				glBegin(GL_LINES);
					glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);

					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);

					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
					glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);

					glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
					glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
				glEnd();
				gridMat->unselect();
				material->select();
			}
		}
	}
}

//------------------------------------------------------------------------------

Player::Player(Pd P, Qd R, BoundingBox B) : BoundedObject(P, R, B) {
	const string path = "assets/models/";
	head = Loader::ModelObject(Pd(), Qd(), path + "head.3DS");
	body = Loader::ModelObject(Pd(), Qd(), path + "body.3DS");
	weapon = Loader::ModelObject(Pd(), Qd(), path + "gun.3DS");
	tool = Loader::ModelObject(Pd(), Qd(), path + "wrench.3DS");
	wheel = Loader::ModelObject(Pd(), Qd(), path + "wheel.3DS");
	children.insert(head);
	children.insert(body);
	children.insert(weapon);
	children.insert(tool);
	children.insert(wheel);

	//set position of seperate elements
	velocity = Vd(0,0,0);
	update(R);

	//textures
	head->material = Assets::Head;
	body->material = Assets::Body;
	weapon->material = Assets::Gun;
}

//------------------------------------------------------------------------------

inline void translate(ObjectHandle o, double x, double y, double z) {
	o->origin = o->origin + Vd(x,y,z);
}

void Player::update(const Qd &camobj) {
	translate(head,0,0,1.95);
	translate(body,0,0,0.3);
	translate(weapon,-0.037,-0.499,1.333);
	translate(tool,-0.037,0.544,1.333);
	translate(wheel,0,0,0.3);
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
