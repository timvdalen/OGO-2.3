/*
 * Structure objects
 *
 * Date: 07-06-12 15:57
 *
 * Description: Building and structural objects
 *
 */

#ifndef _STRUCTURES_H
#define _STRUCTURES_H

namespace Objects {

class Structure;

//------------------------------------------------------------------------------

//! Represents the terrain of the game
class Terrain: public Object
{
	public:
	
	//! Represents a point on a grid
	struct GridPoint
	{
		int x, y;
		
		GridPoint(int X = 0, int Y = 0) : x(X), y(Y) {}
	};
	
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
	GridPoint getGridCoordinates(Vd camera, Vd pos);
};

//------------------------------------------------------------------------------

//! Represents a structure on the terrain
class Structure: public BoundedObject
{
	public:
	Structure(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
			: BoundedObject(P, R, B) {}
};

//------------------------------------------------------------------------------

//! Represents a mine structure on the terrain
class Mine: public Structure
{
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

//------------------------------------------------------------------------------

//! Represents a building on the terrain
class Building: public Structure
{
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

//------------------------------------------------------------------------------

//! Represents a headquarters
class HeadQuarters: public Building
{
	public:
	HeadQuarters(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
		: Building(P, R, B,
		  0, 0,
		  0, 0,
		  0) {}
};

//------------------------------------------------------------------------------

//! Represents a defense tower
class DefenseTower: public Building
{
	public:
	DefenseTower(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
		: Building(P, R, B,
		  0, 0,
		  0, 0,
		  0) {}
};

//------------------------------------------------------------------------------

//! Represents a mining tower built over a mine
class ResourceMine: public Building
{
	public:
	ResourceMine(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox())
		: Building(P, R, B,
		  0, 0,
		  0, 0,
		  0) {}
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _STRUCTURES_H

//------------------------------------------------------------------------------
