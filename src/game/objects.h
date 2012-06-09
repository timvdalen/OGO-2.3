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

class BoundedObject;typedef Handle<BoundedObject> BoundedObjectHandle;
class Terrain;
class Player;

//------------------------------------------------------------------------------

//! Represents a bounding box.

//! Point variables follow xyz where x is l(left) or r(ight), y is t(op) or b(ottom) and z is l(ow) or h(igh).
struct BoundingBox
{
	Point<double> lbl, rbl, ltl, rtl, lbh, rbh, lth, rth;

	BoundingBox(const BoundingBox &B)
		: lbl(B.lbl), rbl(B.rbl), ltl(B.ltl), rtl(B.rtl),
		  lbh(B.lbh), rbh(B.rbh), lth(B.lth), rth(B.rth) {}

	BoundingBox(Pd _lbl = Pd(), Pd _rbl = Pd(), Pd _ltl = Pd(), Pd _rtl = Pd(),
	            Pd _lbh = Pd(), Pd _rbh = Pd(), Pd _lth = Pd(), Pd _rth = Pd())
		: lbl(_lbl), rbl(_rbl), ltl(_ltl), rtl(_rtl),
		  lbh(_lbh), rbh(_rbh), lth(_lth), rth(_rth) {}
};

//------------------------------------------------------------------------------

//! Represents an object with a bounding box
class BoundedObject: public Object
{
    //!Checks if the point p is inside the box defined by the lowerleft vertex a and the upperright vertex b
    bool insideBox(Point<double> p, Point<double> a, Point<double> b);
    pair<ObjectHandle, double> findCollision(Point<double> origin, Vector<double> direction);
    
    public:
	//! The boundingbox for this object
	BoundingBox bb;

	//! Constructs a new bounded object
	BoundedObject(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox(), MaterialHandle M = Material())
		: Object(P, R, M), bb(B) {}

	virtual ~BoundedObject() {}

	//! Checks if a line from origin to direction collides with this object or one of its children.
	//! If there is a collision, this function returns a handle to the object the line collides with
	//! if not, it returns null.
	ObjectHandle checkCollision(Point<double> origin, Vector<double> direction);

	//! Draws this object
	virtual void draw() {}
};

//------------------------------------------------------------------------------

//! Represents a team
class Team: public Object
{
	public:
	//! The amount of resources this team has
	Resource resources;

	//! List of players in this team
	vector<Player> players;
};

//------------------------------------------------------------------------------

// These will be moved to player.h in the future

enum ToolType
{
	toolNone,
	toolSomething,
	toolCount
};

enum WeaponType
{
	weapNone,
	weapLaser,
	weapWrench,
	weapCount
};

//! Represents a player
class Player: public BoundedObject
{
	//!Set relative position of seperate elements
	void translateModel();

	public:
	typedef unsigned int Id;

	//! The ID of this player
	Id id;

	//! The team of this player
	unsigned char team;

	//! The name of this player
	string name;
	
	//! Maximum health for this player
	int maxHealth;

	//! Health of this player
	int health;

	//! The time of the last shot this player fired
	time_t lastShot;
	
	//! Currently selected weapon to fire
	WeaponType weapon;
	
	//! Currently selected building type to build
	ToolType tool;

	//! Velocity of the player (y-axis)
	Vd velocity, oldVelocity;
	static const Vd maxVelocity;

	//! Model
	struct { ObjectHandle head, body, weapon, tool, wheel; } model;

	//! Constructs a player
	Player(Id _id = 0, unsigned char _team = 'a', string _name = "player", Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox());

	//! Update model transformations according to velocity and the camera direction
	void update(const Qd &camobj);
};

//------------------------------------------------------------------------------

//! Represents a laser beam
class LaserBeam: public Object
{
	public:
	//! Wether or not the parent can remove this LaserBeam
	bool done;
	
	//! The time in milliseconds since the start of the glut event loop this laser was fired
	int fireTime;

	//! The time in milliseconds this laser lives
	int ttl;
	
	//! The direction (in normalized object coordinates) that his laser was fired in
	Vd direction;
	
	//! Constructs a new LaserBeam, the origin is the point from which this laser was fired
	LaserBeam(Pd _origin, Vd _direction, int _fireTime = Video::ElapsedTime(), int _ttl = 300);
	
	//! Sets up the translations and material for the LaserBeam
	virtual void preRender();
	
	//! Draws the LaserBeam
	virtual void draw();
	
};

//------------------------------------------------------------------------------

//! Cube like object (it is in reality a parallelepiped) to test with
struct Cuboid : public BoundedObject
{
	Vector<double> u, v, w;

	Cuboid(Pd origin = Pd(), double S = 1);

	void rotate(Quaternion<double> Q) { Qd q = ~Q; u = q*u ;v = q*v; w = q*w; }

	virtual void draw();
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _OBJECTS_H

//------------------------------------------------------------------------------
