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

#include "player.h"

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
	
	bool operator==(const GridPoint& other) const;
	
	bool operator!=(const GridPoint& other) const;
	
	GridPoint operator+(const GridPoint& other) const;
	
	GridPoint operator-(const GridPoint& other) const;
	
	void operator+=(const GridPoint& other);

    bool isValid();
};

//------------------------------------------------------------------------------

//! Represents the terrain of the game
class Terrain: public Object
{

	GLuint gridDL;
	public: NAME(Terrain)


	//! Determines if the ground has been cached in a display list

	//! Determines if the gridlines has been cached in a display list
	bool gridCached;


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
	map<GridPoint, ObjectHandle> structures;

	//! A ghosted tower displayed on the terrain
	pair<GridPoint, ObjectHandle> ghost;

	//! Constructs a new Terrain
	Terrain(double _width, double _height);

	//! Draws the terrain
	virtual void draw();

	//! Draw the terrains chidlren
	virtual void postRender();

	//!Draws the grid
	void drawGround();

	//!Draws the gridlines
	void drawGridLines();

	//! Sets the selected grid points
	void setSelected(GridPoint p);

	//! Returns whether or not a structure can be built on \ref GridPoint p
	
	//! Returns 0 when placing a structure is not possible,
	//! returns 1 when a DefenseTower can be built,
	//!	returns 2 when a Mine can be built
	int canPlaceStructure(GridPoint p);

	//! Safe-places a Structure on the grid

	//! Returns false when adding fails
	bool placeStructure(GridPoint p, ObjectHandle s);

	//! Gives the grid coordinates corresponding to a mouse click
	GridPoint getGridCoordinates(Pd camera, Qd rot);

	//! Converts a GridPoint to a Point<double>
	Pd ToPointD(GridPoint p);

	//! Converts a Point<double> to a GridPoint
	GridPoint ToGrid(Pd point);
};

//------------------------------------------------------------------------------

//! Represents a structure on the terrain
class Structure: public BoundedObject
{
	public: NAME(Structure)
	Structure(BoundingBox B = BoundingBox())
			: BoundedObject(Pd(), Qd(), B) {}
};

//------------------------------------------------------------------------------

//! Represents a mine structure on the terrain
class Mine: public Structure
{
	public: NAME(Mine)
	//! The maxium income that can be generated from this mine
	Resource maxIncome;

	//! Constructs a new mine
	Mine(Pd P = Pd(), Qd R = Qd(), BoundingBox B = BoundingBox(),
	     Resource _maxIncome = 0);

	//! Draws the mine
	virtual void draw();

	//! Model
	struct { ObjectHandle rock; } model;
};

//------------------------------------------------------------------------------

//! Represents a building on the terrain
class Building: public Structure
{
	protected:
	//! If the building is built
	bool built;

	public: NAME(Building)
	//! The GridPoint this building is at
	GridPoint loc;
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
	Player::Id owner;

	//! Constructs a new building
	Building(int _height, BoundingBox B = BoundingBox(),
	         Resource _cost = 0, Resource _income = 0, int _buildTime = 0,
			 int _buildDuration = 0, Power _attackPower = 0, Player::Id _owner = 0)
		: height(_height), Structure(B),
		  cost(_cost), income(_income),
		  buildTime(_buildTime), buildDuration(_buildDuration),
		  attackPower(_attackPower), owner(_owner), loc(GridPoint(-1, -1)) {
			  built = false;
		  }

	//! Sets up translations and rotations
	virtual void preRender();
	
	//! Renders children and resets translations and rotations
	virtual void postRender();

	//! Draws the building
	virtual void draw(){}

};

//------------------------------------------------------------------------------

//! Represents a headquarters
class HeadQuarters: public Building
{
	public: NAME(HeadQuarters)

	//! Constructs HeadQuarters
	HeadQuarters(Player::Id _owner = 0);

	virtual void draw(){}

	//! Model
	struct { ObjectHandle base, socket, core; } model;
};

//------------------------------------------------------------------------------

//! Represents a defense tower
class DefenseTower: public Building
{
	//! The time (in milliseconds since the glut event loop was started) that the last shot was fired 
	int lastshot;

	public: NAME(DefenceTower)
	//! Constructs a DefenseTower
	DefenseTower(Player::Id _owner = 0);

	//! Constructs a ghost DefenseTower
	DefenseTower(int buildTime);

	//! Performs calculations
	void frame();

	//! Draws the DefenseTower
	virtual void draw();

	//! Renders the DefenseTower
	virtual void render();

	//! Model
	struct { ObjectHandle turret; } model;
};

//------------------------------------------------------------------------------

//! Represents a mining tower built over a mine
class ResourceMine: public Building
{
	ObjectHandle rock;

	public: NAME(ResourceMine)
	
	ResourceMine(Player::Id _owner);
	
	ResourceMine(int buildTime);

	virtual void draw();
	
	virtual void postRender();

	//! Model
	struct { ObjectHandle rig, drill; } model;
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _STRUCTURES_H

//------------------------------------------------------------------------------
