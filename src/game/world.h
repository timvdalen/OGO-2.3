#ifndef _WORLD_H
#define _WORLD_H

#include "objects.h"
#include "structures.h"
#include "hud.h"

namespace Objects{

using namespace HUD_objects;

//------------------------------------------------------------------------------

//! Represents the world of the game
class World: public BoundedObject
{
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
	
	//! Finds the first colliding object going from a point in a direction 
	//! \par origin will define the starting point in the world for the lookpath
	//! \par path is a vector that is searched
	//! \note the norm of path defines the length that is searched.
	//!       When the function returns witha object the path value will be set
	//!       to the vector pointing to collision spot
	//! \returns the first colliding object or an empty handle when no object was found
	ObjectHandle trace(Point<double> origin, Vector<double> &path);
};

//------------------------------------------------------------------------------

//! Represents a droppable object
class Droppable: public BoundedObject
{
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

//------------------------------------------------------------------------------

}

#endif