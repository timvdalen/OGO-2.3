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
	
	BoundingBox(const BoundingBox &B){
		lbl = B.lbl;
		rbl = B.rbl;
		ltl = B.ltl;
		rtl = B.rtl;
		lbh = B.lbh;
		rbh = B.rbh;
		lth = B.lth;
	       	rth = B.rth;	
	}

	BoundingBox(Point<double> _lbl = Point<double>(), Point<double> _rbl = Point<double>(), Point<double> _ltl, Point<double> _rtl, Point<double> _lbh, Point<double> _bh, Point<double> _lth, Point<double> _rth){
		lbl = _lbl;
		rbl = _rbl;
		ltl = _ltl;
		rtl = _rtl;
		lbh = _lbh;
		rbh = _rbh;
		lth = _lth;
		rth = _rth;
	}
};

//! Represents an object with a bounding box
class BoundedObject: public Object{
	public:
		//! The boundingbox for this object
		BoundingBox bb;
	
	
		//! Constructs a new bounded object
		BoundedObject(Point<double> P = Point<double>(), Quaternion<double> R = Quaternion<double>(), BoundingBox B) : Object(P, R), bb(B) {}


		//! Checks if a line from origin to direction collides with this object or one of its children.
		//! If there is a collision, this function returns a handle to the object the line collides with
		//! if not, it returns null.
		ObjectHandle checkCollision(Point<double> origin, Vector<double> direction);

		//! Draws this object
		void draw();
};


//! Represents the world of the game
class World: public BoundedObject{
	public:
		//! Size of the world in the X direction
		double width;

		//! Size of the world in the Y direction
		double height;

		//! Constructs a new world
		World(double _width, double _height) : BoundedObject(Point<double>(), Quaternion<double>(), BoundingBox(Point<double>(0,0,0), Point<double>(_width,0,0), Point<double>(0,_height,0), Point<double>(_width,_height,0)) {
			width = _width;
			height = _height;
		}

		//! Draws the world
		void draw();
};

//! Represents the terrain of the game
class Terrain: public Object{
	public:
		//! Represents the \ref Structure "Structure" on the grid of the terrain
		map<Point<double>, Structure> structures;

		//! Draws the terrain
		void draw();
};

//! Represents a structure on the terrain
class Structure: public Object{};

//! Represents a mine structure on the terrain
class Mine: public Structure{
	public:
		//! The maxium income that can be generated from this mine
		Resource maxIncome;

		//! Constructs a new mine
		Mine(Point<double> P = Point<double>(), Quaternion R = Quaternion<double>(), BoundingBox B = BoundingBox(), Resource _maxIncome) : BoundedObject(P, R, B){
			maxIncome = _maxIncome;
		}

		//! Draws the mine
		void draw();
};

//! Represents a building on the terrain
class Building: public Structure{
	public:
		//! The cost of this building
		Resource cost;
		//! The income this building generates
		Resource income;
		//! The time at which the construction of this building was started
		time_t buildTime
		//! The time it takes to completely build this building
		time_t buildDuration;
		//! The attack power of this building
		Power attackPower;


		//! Constructs a new building
		Building(Point<double> P = Point<double>(), Quaternion R = Quaternion<double>(), BoundingBox B = BoundingBox(), Resource _cost, Resource _income, time_t _buildTime, time_t _buildDuration, Power _attackPower) : BoundedObject(P, R, B){
			cost = _cost;
			income = _income;
			buildTime = _buildTime;
			buildDuration = _buildDuration;
			attackPower = _attackPower;
		}

		//! Draws the building
		void draw();

};

//! Represents a headquarters
class HeadQuarters: Building{

};

//! Represents a defense tower
class DefenseTower: Building{

};

//! Represents a mining tower built over a mine
class ResourceMine: Building{

};

}

//------------------------------------------------------------------------------

#endif // _OBJECTS_H

//------------------------------------------------------------------------------
