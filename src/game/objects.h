/*
 * Objects
 *
 * Date: 01-05-12 15:09
 *
 * Description:
 *
 */

#ifndef _OBJECTS_H
#define _OBJECTS_H

#include <time.h>
#include <map>
#include <vector>

#include "core.h"
#include "assets.h"
#include "materials.h"

//! Contains the game objects
namespace Objects {

using namespace Core;
using namespace Base::Alias;
using namespace Materials;

class BoundedObject;
class Terrain;
class Player;

//------------------------------------------------------------------------------

void applyBillboarding();

//------------------------------------------------------------------------------

//! Represents a bounding box.

//! Point variables follow xyz where x is l(left) or r(ight), y is t(op) or b(ottom) and z is l(ow) or h(igh).
struct BoundingBox
{
	Point<double> lbl, rbl, ltl, rtl, lbh, rbh, lth, rth;

	BoundingBox(const BoundingBox &B)
		: lbl(B.lbl), rbl(B.rbl), ltl(B.ltl), rtl(B.rtl),
		  lbh(B.lbh), rbh(B.rbh), lth(B.lth), rth(B.rth) {}

	BoundingBox(Pd _lbl, Pd _rbl, Pd _ltl, Pd _rtl = Pd(),
	            Pd _lbh = Pd(), Pd _rbh = Pd(), Pd _lth = Pd(), Pd _rth = Pd())
		: lbl(_lbl), rbl(_rbl), ltl(_ltl), rtl(_rtl),
		  lbh(_lbh), rbh(_rbh), lth(_lth), rth(_rth) {}
	
	BoundingBox(Pd _lbl = Pd(), Pd _rth = Pd())
		: lbl(_lbl), rbl(0), ltl(0), rtl(0),
		  lbh(0), rbh(0), lth(0), rth(_rth) {}
};

//------------------------------------------------------------------------------

//! Represents something that can be destroyed
class Destroyable
{
	public:
	//! The maximum health of this thing
	double maxHealth;
	
	//! The current health of this thing
	double health;
	
	//! Creates a new Destroyable thing
	Destroyable(double _maxHealth)
		:maxHealth(_maxHealth), health(_maxHealth){}
		
	//! Creates a new non-Destroyable thing
	Destroyable()
		:maxHealth(-1.0), health(-1.0){}
		
	//! Checks is the thing is destroyed
	bool isDestroyed();
	
	//! Adds damage
	void damage(double dmg);
	
	//! Heals the thing
	void heal(double _health);

	//! Restores the thing to full health
	void fullHeal();
};

//------------------------------------------------------------------------------

//! Represents an object with a bounding box
class BoundedObject: public Object
{

    public: NAME(BoundedObject)
	//! The boundingbox for this object
	BoundingBox bb;

    //!Checks if the point p is inside the box defined by the lowerleft vertex a and the upperright vertex b
    bool insideBox(Point<double> p, Point<double> a, Point<double> b);

	//! Constructs a new bounded object
	BoundedObject(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox(), MaterialHandle M = Material())
		: Object(P, R, M), bb(B) {}

	virtual ~BoundedObject() {}

	//! Checks if a line from origin to direction collides with this object or one of its children.
	//! If there is a collision, this function returns a handle to the object the line collides with
	//! if not, it returns null.
	virtual pair<ObjectHandle, double> checkCollision(Pd origin, Vd direction);
	ObjectHandle checkCollision2(Pd origin, Vd direction);
	
	bool checkCollision(const ObjectHandle &target);
	
	//! Draws this object
	virtual void draw() {}
};

//------------------------------------------------------------------------------

//! Represents an object that enables inheritance of materials
class ModelObjectContainer: public Object
{
	public: NAME(MOC)
	//! Override the render function in Object to enable inheritance of materials
	virtual void render();
};

//------------------------------------------------------------------------------

//! Represents a team
class Team
{
	public:
	//! The id of this team
	unsigned char id;
	
	//! The amount of resources this team has
	Resource resources;
	
	Team(unsigned char _id = 0) : id(_id), resources(200) {}
};

//------------------------------------------------------------------------------

//! Represents a laser beam
class LaserBeam: public Object
{
	public: NAME(LaserBeam)
	SERIAL(type() | convert(origin) | convert(direction) | convert(collision))
	UNSERIAL(arg, 3,
		origin = ToPoint(arg[0]);
		direction = ToQuaternion(arg[1]);
		collision = ToFloat(arg[2]);
	)
	
	//! Wether or not the parent can remove this LaserBeam
	bool done;

	//! The time in milliseconds since the start of the glut event loop this laser was fired
	int fireTime;

	//! The time in milliseconds this laser lives
	int ttl;

	//! The direction (in normalized object coordinates) that his laser was fired in
	Qd direction;

	//! The distance to the collision point of the laser, if there is one
	double collision;

	//! Constructs a new LaserBeam, the origin is the point from which this laser was fired
	LaserBeam(Pd _origin = Pd(), Qd _direction = Qd(), double _collision = 0.0,
		int _fireTime = Video::ElapsedTime(), int _ttl = 300);

	//! Sets up the translations and material for the LaserBeam
	virtual void preRender();

	//! Draws the LaserBeam
	virtual void draw();

};

//------------------------------------------------------------------------------

//! Cube like object (it is in reality a parallelepiped) to test with
struct Cuboid : public BoundedObject
{
	NAME(Cuboid)
	Vector<double> u, v, w;

	Cuboid(Pd origin = Pd(), double S = 1);

	void rotate(Quaternion<double> Q) { Qd q = ~Q; u = q*u ;v = q*v; w = q*w; }

	virtual void draw();
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _OBJECTS_H

//------------------------------------------------------------------------------
