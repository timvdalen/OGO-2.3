#ifndef _MINION_H
#define _MINION_H

#include "objects.h"
#include "player.h"
#include "structures.h"

namespace Objects{

//! Represents a minion
class Minion: public BoundedObject{
	protected:
	
	//! Waypoints for this Minion to follow
	vector<GridPoint> waypoints;
	
	//! Creates waypoints for this minion
	void moveTo(GridPoint p);
	
	//! Indicates the next target for this minion
	virtual void setNewTarget();
	
	public: NAME(Minion)
	
	//! Constructs a new minion
	Minion(Pd origin, Qd rotation);
	
	//! Called before every render
	virtual void frame();
	
	//! Renders the minion
	virtual void render();
	
	//! Draws the minion
	virtual void draw();
};

//! A minion that roams the edges of the arena
class ArenaGuard: public Minion{
	ObjectHandle world;

	protected:
	//! Gets the new target
	virtual void setNewTarget();
	
	public: NAME(ArenaGuard)
	
	//! Constructs a new AreaGuard
	ArenaGuard(ObjectHandle _world, Pd origin, Qd rotation);
};

//! A minion that will follow its owner around
class DefenseMinion: public Minion{
	//! The owner of this defense minion
	ObjectHandle owner;
	
	//! The world this defende minion is placed in
	ObjectHandle world;
	
	//! Last location of the minion
	GridPoint last;

	protected:
	//TODO: Make this but for the entire team
	//! Gets the new target
	virtual void setNewTarget();
	
	public: NAME(DefenseMinion)
	
	//! Constructs a new AreaGuard
	DefenseMinion(ObjectHandle _owner, ObjectHandle _world, Pd origin, Qd rotation);
};

}

#endif
