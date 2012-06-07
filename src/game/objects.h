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

//------------------------------------------------------------------------------

//! Contains the game objects
namespace Objects{

using namespace Core;
using namespace Base::Alias;
using namespace Materials;

struct Boundingbox;
class BoundedObject;
class World;
class Team;
class Player;
class LaserBeam;
class Terrain;
class Structure;
class Mine;
class Building;
class HeadQuarters;
class DefenseTower;
class ResourceMine;
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

//! Represents a point on a grid
struct GridPoint{
	int x,y;
};

//! Represents an object with a bounding box
class BoundedObject: public Object{
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

//! Represents the terrain of the game
class Terrain: public Object{
	public:
		//! Size of the world in the X direction.

		//! Needs to be a multiple GRID_SIZE
		double width;

		//! Size of the world in the Y direction.

		//! Needs to be a multiple GRID_SIZE
		double height;

		//! Selected square of the grid.

		//! If this is not on the grid, it will not show up.
		GridPoint selected;

		//! Whether or not to draw a grid on this terrain
		bool showGrid;

		//! Represents the \ref Structure "Structure" on the grid of the terrain
		map<Point<double>, Structure> structures;

		//! Constructs a new Terrain
		Terrain(double _width, double _height);

		//! Draws the terrain
		virtual void draw();

		//! Gives the grid coordinates corresponding to a mouse click
		pair<int, int> getGridCoordinates(Vd camera, Vd pos);
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
	public:
		//! Maximum health for this player
		int maxHealth;

		//! Health of this player
		int health;

		//! The time of the last shot this player fired
		time_t lastShot;

		//! Velocity of the player (y-axis)
		Vd velocity;

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

//! Represents a structure on the terrain
class Structure: public BoundedObject
{
	public:
	Structure(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
			: BoundedObject(P, R, B) {}
};

//! Represents a mine structure on the terrain
class Mine: public Structure{
	public:
		//! The maxium income that can be generated from this mine
		Resource maxIncome;

		//! Constructs a new mine
		Mine(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox(),
		     Resource _maxIncome = 0)
			: Structure(P, R, B), maxIncome(_maxIncome) {}

		//! Draws the mine
		virtual void draw();
};

//! Represents a building on the terrain
class Building: public Structure{
	public:
		//! The cost of this building
		Resource cost;
		//! The income this building generates
		Resource income;
		//! The time at which the construction of this building was started
		time_t buildTime;
		//! The time it takes to completely build this building
		time_t buildDuration;
		//! The attack power of this building
		Power attackPower;


		//! Constructs a new building
		Building(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox(),
		         Resource _cost = 0, Resource _income = 0, time_t _buildTime = 0,
				 time_t _buildDuration = 0, Power _attackPower = 0)
			: Structure(P, R, B),
			  cost(_cost), income(_income),
			  buildTime(_buildTime), buildDuration(_buildDuration),
			  attackPower(_attackPower) {}

		//! Draws the building
		virtual void draw();

};

//! Represents a headquarters
class HeadQuarters: public Building{
	public:
	HeadQuarters(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
		: Building(P, R, B,
		  0, 0,
		  0, 0,
		  0) {}
};

//! Represents a defense tower
class DefenseTower: public Building{
	public:
	DefenseTower(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
		: Building(P, R, B,
		  0, 0,
		  0, 0,
		  0) {}
};

//! Represents a mining tower built over a mine
class ResourceMine: public Building{
	public:
	ResourceMine(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
		: Building(P, R, B,
		  0, 0,
		  0, 0,
		  0) {}
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
