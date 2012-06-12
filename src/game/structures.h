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

#define GRID_SIZE 10

namespace Objects {

class Structure;
class Building;
class DefenseTower;
class Mine;
class HeadQuarters;
class ResourceMine;
    

//------------------------------------------------------------------------------

//! Represents a point on a grid
struct GridPoint
{
	int x, y;
	
	GridPoint(int X = 0, int Y = 0) : x(X), y(Y) {}
	
	bool operator<(const GridPoint& p2) const;
    
    bool isValid();
};

//------------------------------------------------------------------------------

//! Represents the terrain of the game
class Terrain: public Object
{
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

	//! Represents the \ref Structure "Structures" on the grid of the terrain
	multimap<GridPoint, ObjectHandle> structures;
	
	//! Constructs a new Terrain
	Terrain(double _width, double _height);

	//! Draws the terrain
	virtual void draw();
	
	//! Draw the terrains chidlren
	virtual void postRender();
	
	//! Safe-places a Structure on the grid
	
	//! Returns false when adding fails
	bool placeStructure(GridPoint p, ObjectHandle s);

	//! Gives the grid coordinates corresponding to a mouse click
	GridPoint getGridCoordinates(Vd camera, Vd pos);
};

//------------------------------------------------------------------------------

//! Represents a structure on the terrain
class Structure: public BoundedObject
{
	public:
	Structure(BoundingBox B = BoundingBox())
			: BoundedObject(Pd(), Qd(), B) {}
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
		: Structure(B), maxIncome(_maxIncome) {}

	//! Draws the mine
	virtual void draw() {}
};

//------------------------------------------------------------------------------

//! Represents a building on the terrain
class Building: public Structure
{
	public:
	//! Height of this building in local object coordinates
	int height;
	//! The cost of this building
	Resource cost;
	//! The income this building generates
	Resource income;
	//! The time in milliseconds since the start of the glut event loop at which the construction of this building was started
	int buildTime;
	//! The time in milliseconds it takes to completely build this building
	int buildDuration;
	//! The attack power of this building
	Power attackPower;
	//! Owner of the building (player)
	ObjectHandle owner;

	//! Constructs a new building
	Building(int _height, BoundingBox B = BoundingBox(),
	         Resource _cost = 0, Resource _income = 0, int _buildTime = 0,
			 int _buildDuration = 0, Power _attackPower = 0, ObjectHandle _owner = ObjectHandle())
		: height(_height), Structure(B),
		  cost(_cost), income(_income),
		  buildTime(_buildTime), buildDuration(_buildDuration),
		  attackPower(_attackPower), owner(_owner) {}

	//! Sets up translations and rotations
	virtual void preRender();
		  
	//! Draws the building
	virtual void draw(){}

};

//------------------------------------------------------------------------------

//! Represents a headquarters
class HeadQuarters: public Building
{
	public:
	HeadQuarters(BoundingBox B = BoundingBox())
		: Building(10, B,
		  0, 0,
		  0, 0,
		  0) {}
	
		  
	virtual void draw(){}
};

//------------------------------------------------------------------------------

//! Represents a defense tower
class DefenseTower: public Building
{
	public:
	DefenseTower();
		  
	virtual void draw();
	
	//! Model
	struct { ObjectHandle turret; } model;
};

//------------------------------------------------------------------------------

//! Represents a mining tower built over a mine
class ResourceMine: public Building
{
	public:
	ResourceMine(BoundingBox B = BoundingBox())
		: Building(15, B,
		  0, 0,
		  0, 0,
		  0) {}
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _STRUCTURES_H

//------------------------------------------------------------------------------
