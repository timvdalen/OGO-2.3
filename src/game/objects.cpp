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
    bool collision;
    //--- We only check lbl rbh, could be improved-----
    Point<double> a = bb.lbl;
    Point<double> b = bb.rbh;
    //--- Origin does not have to be rotated
    Point<double> p = origin - this->origin;
    //--- Vector only needs to be rotated
    Vector<double> v = rotation*direction;
    //--- Now check if we have a collision in the x direction
    double lambda1, lambda2;
    if(v.x != 0 && !collision){
        lambda1 = (a.x - p.x)/(v.x);//intersection with axis in lbl.z
        lambda2 = (b.x -p.x)/(v.x);
        collision = insideBox(p + v*lambda1, a, b) || insideBox(p + v*lambda2, a, b);
    }
    //---- y direction
    if(v.y != 0 && !collision){
        lambda1 = (a.y - p.y)/(v.y);
        lambda2 = (b.y - p.y)/(v.y);
        collision = insideBox(p + v*lambda1, a, b) || insideBox(p + v*lambda2, a, b);
    }
    //--- z direction
    if(v.z != 0 && !collision){
        lambda1 = (a.z - p.z)/(v.z);//intersection with axis in lbl.z
        lambda2 = (b.z - p.z)/(v.z);
        collision = insideBox(p + v*lambda1, a, b) || insideBox(p + v*lambda2, a, b);
    }
    if(collision){
        set<ObjectHandle>::iterator it;
        for (it = children.begin(); it != children.end(); ++it){
            BoundedObject* child = dynamic_cast<BoundedObject *>(&**it);
            if(child){
                ObjectHandle childcollision = child->checkCollision(p, v);
                if(childcollision){ //We have a collision with a child
                    return childcollision;
                }
                childcollision.clear();
            }
        }
        return (ObjectHandle)(*this);
    }
    return ObjectHandle();
}

bool BoundedObject::insideBox(Point<double> p, Point<double> a, Point<double> b){
        return a.x <= p.x && p.x <= b.x//Inside x-interval
            && a.y <= p.y && p.y <= b.y//Inside y-interval

    && a.z <= p.z && p.z <= b.z;//Inside z-interval
}

//------------------------------------------------------------------------------

World::World(double _width, double _height)
	: BoundedObject(Pd(), Qd(),
	  BoundingBox(Pd(), Pd(_width,0,0), Pd(0,_height,0), Pd(_width,_height,0)),
	  Assets::WorldMaterial)
{
	children.insert(Terrain(_width, _height));
	terrain = dynamic_cast<Terrain *>(&**children.begin());
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

	//Set normal
	glNormal3i(0, 0, 1);

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

// This function draws the line from camera to pos. It then finds the intersection
// with the ground: the corresponding grid coordinates are returned.
pair<int, int> Terrain::getGridCoordinates(Vd camera, Vd pos){
    Vd dir = pos + -camera;

    double lambda = -camera.z / dir.z;
    double intersecx = camera.x + dir.x * lambda;
    double intersecy = camera.y + dir.y * lambda;

    int x = floor(intersecx / GRID_SIZE);
    int y = floor(intersecy / GRID_SIZE);
    return pair<int, int>(x, y);
}

//------------------------------------------------------------------------------

Player::Player(Pd P, Qd R, BoundingBox B) : BoundedObject(P, R, B) {
	const string path = "assets/models/";
	head = Assets::HeadObj;
	body = Assets::BodyObj;
	weapon = Assets::GunObj;
	tool = Assets::WrenchObj;
	wheel = Assets::WheelObj;
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

	//translateModel();
}

//------------------------------------------------------------------------------

inline void translate(ObjectHandle o, double x, double y, double z) {
	o->origin = o->origin + Vd(x,y,z);
}

void Player::translateModel() {
	translate(head,0,0,1.95);
	translate(body,0,0,0.3);
	translate(weapon,-0.037,-0.499,1.333);
	translate(tool,-0.037,0.544,1.333);
	translate(wheel,0,0,0.3);
}

void Player::update(const Qd &camobj) {

	head->rotation = camobj;
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
