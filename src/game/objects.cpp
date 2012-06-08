/*
 * Objects -- see header
 */

#include <stack>
#include <limits>

#include "objects.h"
#include "structures.h"
#include "video.h"
#include "assets.h"

namespace Objects {

//------------------------------------------------------------------------------

pair<BoundedObjectHandle, double> BoundedObject::checkCollision(Point<double> origin, Vector<double> direction)
{
    double collision = numeric_limits<double>::infinity();
    //--- We only check lbl rbh, could be improved-----
    Point<double> a = bb.lbl;
    Point<double> b = bb.rth;
    //--- Origin does not have to be rotated
    Point<double> p = origin - this->origin;
    //--- Vector only needs to be rotated
    Vector<double> v = rotation*direction;
    //--- Now check if we have a collision in the x direction
    double lambda1, lambda2;
    if(v.x != 0){
        lambda1 = (a.x - p.x)/(v.x);
        lambda2 = (b.x -p.x)/(v.x);
        if(insideBox(p + v*lambda1, a, b) && 0 < lambda1 && lambda1 < collision){
            collision = lambda1;
        }
        if (insideBox(p + v*lambda2, a, b) && 0 < lambda2 && lambda2 < collision){
            collision = lambda2;
        }
    }
    //---- y direction
    if(v.y != 0){
        lambda1 = (a.y - p.y)/(v.y);
        lambda2 = (b.y - p.y)/(v.y);
        if(insideBox(p + v*lambda1, a, b) && 0 < lambda1 && lambda1 < collision){
            collision = lambda1;
        }
        if(insideBox(p + v*lambda2,a,b) && 0 < lambda2 && lambda2 < collision){
            collision = lambda2;
        }
    }
    //--- z direction
    if(v.z != 0 && !collision){
        lambda1 = (a.z - p.z)/(v.z);//intersection with axis in lbl.z
        lambda2 = (b.z - p.z)/(v.z);
        if(insideBox(p + v*lambda1, a, b) && 0 < lambda1 && lambda1 < collision){
            collision = lambda1;
        }
        if(insideBox(p + v*lambda2, a, b) && 0 < lambda2 && lambda2 < collision){
            collision = lambda2;
        }
    }
    //find a collision with a child
    if(collision < std::numeric_limits<double>::infinity()){
        set<ObjectHandle>::iterator it;
        BoundedObjectHandle colobject = *this;
        for (it = children.begin(); it != children.end(); ++it){
            BoundedObject* child = TO(BoundedObject, *it);
            if(child){
                pair<BoundedObjectHandle, int> childcollision = child->checkCollision(p, v);
                if(childcollision.second < collision){ //We have a collision with a child
                    colobject.clear();
                    collision = childcollision.second;
                    colobject = childcollision.first;
                }else{
                    childcollision.first.clear();
                }
//                childcollision.clear();
            }
        }
        return make_pair(colobject,collision);
    }
    return make_pair(BoundedObjectHandle(),collision);
}

//------------------------------------------------------------------------------

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

ObjectHandle World::trace(Point<double> origin, Vector<double> path)
{
	// Todo: implement
}

//------------------------------------------------------------------------------

Player::Player(Pd P, Qd R, BoundingBox B) : BoundedObject(P, R, B) {
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

	translateModel();
}

//------------------------------------------------------------------------------

inline void translate(ObjectHandle o, double x, double y, double z) {
	o->origin = o->origin + Vd(x,y,z);
}

void Player::translateModel() {
	translate(head,0,0,1.95);
	translate(body,0,0,0.3);
	translate(weapon,-0.499,-0.037,1.333);
	translate(tool,0.544,-0.037,1.333);
	translate(wheel,0,0,0.3);
}

//------------------------------------------------------------------------------

const Vd Player::maxVelocity = Vd(0,1,0);

void Player::update(const Qd &camobj) {
	//if (velocity.y < 1) velocity.y += 0.01;

	if (velocity.y != oldVelocity.y) {
		oldVelocity = velocity;

		rotation = Qd();
		double angle = velocity.y / maxVelocity.y * 0.15f * Pi;
		rotation = rotation * Rd(angle,Vd(1,0,0));
		
		weapon->rotation = -rotation;
		tool->rotation = -rotation;
	}

	head->rotation = camobj;
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
