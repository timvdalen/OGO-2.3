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
#include "hud.h"

//------------------------------------------------------------------------------

//! Contains the game objects
namespace Objects{

using namespace Core;
using namespace Base::Alias;
using namespace Materials;
using namespace HUD_objects;

struct Boundingbox;
class BoundedObject;
class World;
class Team;
class Player;
class LaserBeam;
class Terrain;
class Droppables;

//! Represents a bounding box.

//! Point variables follow xyz where x is l(left) or r(ight), y is t(op) or b(ottom) and z is l(ow) or h(igh).
struct BoundingBox{
	Point<double> lbl, rbl, ltl, rtl, lbh, rbh, lth, rth;

	BoundingBox(const BoundingBox &B)
		: lbl(B.lbl), rbl(B.rbl), ltl(B.ltl), rtl(B.rtl),
		  lbh(B.lbh), rbh(B.rbh), lth(B.lth), rth(B.rth) {}

	BoundingBox(Pd _lbl = Pd(), Pd _rbl = Pd(), Pd _ltl = Pd(), Pd _rtl = Pd(),
	            Pd _lbh = Pd(), Pd _rbh = Pd(), Pd _lth = Pd(), Pd _rth = Pd())
		: lbl(_lbl), rbl(_rbl), ltl(_ltl), rtl(_rtl),
		  lbh(_lbh), rbh(_rbh), lth(_lth), rth(_rth) {}
};

//! Represents an object with a bounding box
class BoundedObject: public Object{
    
    //!Checks if the point p is inside the box defined by the lowerleft vertex a and the upperright vertex b
    bool insideBox(Point<double> p, Point<double> a, Point<double> b);
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


//! Represents the world of the game
class World: public BoundedObject{
	public:
		//! Size of the world in the X direction.

		//! Needs to be a multiple of GRID_SIZE
		double width;

		//! Size of the world in the Y direction.

		//! Needs to be a multiple of GRID_SIZE
		double height;

		//! Terrain associated with this World
		Terrain *terrain;

		//! HUD associated with this World
		HUD *hud;

		//! Constructs a new world
		World(double _width, double _height);

		//! Draws the world
		virtual void draw();
};

//! Represents a team
class Team: public Object{
	public:
		//! The amount of resources this team has
		Resource resources;

		//! List of players in this team
		vector<Player> players;
};

//! Represents a player
class Player: public BoundedObject{
		//!Set relative position of seperate elements
		void translateModel();

	public:
		//! Maximum health for this player
		int maxHealth;

		//! Health of this player
		int health;

		//! The time of the last shot this player fired
		time_t lastShot;

		//! Velocity of the player (y-axis)
		Vd velocity;
		static const Vd maxVelocity;

		//! Model
		ObjectHandle head, body, weapon, tool, wheel;

		//! Constructs a player
		Player(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox());

		//! Update model transformations according to velocity and the camera direction
		void update(const Qd &camobj);
};

//! Represents a laser beam
class LaserBeam: public BoundedObject{
	public:
		//! The time this laser was fired
		time_t fireTime;

		//! The time this laser lives
		time_t ttl;
};


//! Represents a droppable object
class Droppable: public BoundedObject{
	public:
		//! The worth of this droppable
		Resource worth;

		//! The time this object was dropped
		time_t dropped;

		//! The time this object lives
		time_t ttl;

		//! The event fired when this droppable is picked up
		void onPickup(World w){}
};

}

//------------------------------------------------------------------------------

#endif // _OBJECTS_H

//------------------------------------------------------------------------------
