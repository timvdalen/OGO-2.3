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

#include "core.h"
#include <map>
#include <time.h>

//------------------------------------------------------------------------------

//! Contains the game objects
namespace Objects{

using namespace Core;
using namespace Base::Alias;

struct Boundingbox;
class BoundedObject;
class World;
class Terrain;
class Structure;
class Mine;
class Building;
class HeadQuarters;
class DefenseTower;
class ResourceMine;

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
	public:
		//! The boundingbox for this object
		BoundingBox bb;
	
		//! Constructs a new bounded object
		BoundedObject(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
			: Object(P, R), bb(B) {}

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
		//! Size of the world in the X direction
		double width;

		//! Size of the world in the Y direction
		double height;

		//! Constructs a new world
		World(double _width, double _height)
			: BoundedObject(Pd(), Qd(),
				BoundingBox(Pd(), Pd(_width,0,0),
					Pd(0,_height,0), Pd(_width,_height,0)))
		{
			width = _width;
			height = _height;
		}

		//! Draws the world
		virtual void draw() {}
};

//! Represents the terrain of the game
class Terrain: public Object{
	public:
		//! Represents the \ref Structure "Structure" on the grid of the terrain
		map<Point<double>, Structure> structures;

		//! Draws the terrain
		virtual void draw();
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

}

//------------------------------------------------------------------------------

#endif // _OBJECTS_H

//------------------------------------------------------------------------------
