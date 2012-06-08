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

pair<ObjectHandle, double> BoundedObject::findCollision(Point<double> origin, Vector<double> direction)
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
        ObjectHandle colobject = *this;
        double collision2 = numeric_limits<double>::infinity();
        for (it = children.begin(); it != children.end(); ++it){
            BoundedObject* child = TO(BoundedObject, *it);
            if(child){
                pair<ObjectHandle, int> childcollision = child->findCollision(p, v);
                if(childcollision.second < collision2){ //We have a collision with a child
                    colobject.clear();
                    collision2 = childcollision.second;
                    colobject = childcollision.first;
                }else{
                    childcollision.first.clear();
                }
            }
        }
        if(collision2 == numeric_limits<double>::infinity()){
            collision2 = collision;
        }
        return make_pair(colobject,collision2);
    }
    return make_pair(ObjectHandle(),collision);
}
    
    ObjectHandle BoundedObject::checkCollision(Point<double> origin, Vector<double> direction){
        return findCollision(origin,direction).first;
    }

//------------------------------------------------------------------------------

bool BoundedObject::insideBox(Point<double> p, Point<double> a, Point<double> b){
        return a.x <= p.x && p.x <= b.x//Inside x-interval
            && a.y <= p.y && p.y <= b.y//Inside y-interval
            && a.z <= p.z && p.z <= b.z;//Inside z-interval
}

//------------------------------------------------------------------------------

Player::Player(Id _id, unsigned char _team, string _name, Pd P, Qd R, BoundingBox B) : BoundedObject(P, R, B) {
	id = id;
	team = _team;
	name = _name;

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
