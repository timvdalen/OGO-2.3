/*
 * World object
 *
 * Date: 08-06-12 16:00
 *
 * Description: Object that defines the root object of the game
 *
 */

#ifndef _WORLD_H
#define _WORLD_H


namespace Objects{
    class World;
}

#include <vector>
#include "objects.h"
#include "structures.h"
#include "hud.h"

namespace Objects {

using namespace HUD_objects;

//------------------------------------------------------------------------------

//! Represents the world of the game
class World: public BoundedObject
{
	public: NAME(World)
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
	
	//! A vector of temporary Objects in this World
	vector<ObjectHandle> temporary;
	
	//! Constructs a new world
	World(double _width, double _height);

	//! Draws the world
	virtual void draw();
	
	//! Renders the Worlds children (including those in laserBeams) and pops the matrix
	virtual void postRender();
	
	//! Finds the first colliding object going from a point in a direction 
	//! \par origin will define the starting point in the world for the lookpath
	//! \par path is a vector that is searched
	//! \note the norm of path defines the length that is searched.
	//!       When the function returns with an object the path value will be set
	//!       to the vector pointing to collision spot
	//! \returns the first colliding object or an empty handle when no object was found
	ObjectHandle trace(Point<double> origin, Vector<double> &path, Object* ignore);
	
	//! Gets a set of objects that touch the given object
	//! \note When the traget is not bounded it only check for the origin
	set<ObjectHandle> sense(ObjectHandle &target);
	
	//! Corrects origin in such a way that the camera cannot be placed outside the world bounds or inside a structure
	Point<double> getCorrectedOrigin(Qd q, Pd p);
	
	//! Adds a LaserBeam to this World
	void addLaserBeam(ObjectHandle laserBeam);
	
};

//------------------------------------------------------------------------------

//! Represents a droppable object
class Droppable: public BoundedObject
{
	public: NAME(Droppable)
	SERIAL(type() | convert(origin)         | convert(rotation)
	              | convert((double) worth) | convert((long)ttl) | convert(id))
	UNSERIAL(arg, 5,
		origin = ToPoint(arg[0]);
		rotation = ToQuaternion(arg[1]);
		worth = ToFloat(arg[2]);
		dropped = ToInteger(arg[3]); 
		id = ToInteger(arg[4]);
	)
	
	long id;
	
	//! Whether or not the Droppable can be removed from the \ref World
	bool done;

	//! The worth of this droppable
	Resource worth;

	//! The time in milliseconds since the start of the glut event loop this object was dropped
	long dropped;

	//! The time in milliseconds this droppable lives
	long ttl;

	//! Model
	struct { ObjectHandle coin; } model;

	//! Creates a new Droppable
	Droppable(Pd _origin = Pd(), Resource _worth = 0, long _dropped = Video::ElapsedTime(), long _ttl = 15000);

	//! Called before draw()
	void preRender();
};

//------------------------------------------------------------------------------

//! Represents a star in the \ref Sky
class Star: public Object
{
	public: NAME(Star)
	//! Whether this star is big
	bool big;

	//! Constructs the star
	Star(Pd P = Pd());

	//! Draws the star
	virtual void draw();
};

//------------------------------------------------------------------------------

class Sky: public Object
{
	public: NAME(Sky)
	//! The width of the sky
	int width;

	//! The height of the sky
	int height;

	//! Contains the stars in the sky
	vector<Star> stars;

	//! Constructs the sky
	Sky(int _width, int _height);

	//! Adds new stars, lets old ones fade
	void frame();

	//! Renders the sky
	virtual void render();
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _WORLD_H

//------------------------------------------------------------------------------
