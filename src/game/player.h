/*
 * Player object
 *
 * Date: 09-06-12 17:18
 *
 * Description: Representation of a player in game
 *
 */

#ifndef _PLAYER_H
#define _PLAYER_H

#include "time.h"

#include <map>

#include "objects.h"

namespace Objects {

using namespace Core;
using namespace Base::Alias;
using namespace Materials;

//------------------------------------------------------------------------------

//! Building tools a player can use
enum ToolType
{
	toolNone,
	toolSomething,
	toolCount
};

//------------------------------------------------------------------------------

//! Weapons a player can use
enum WeaponType
{
	weapNone,
	weapLaser,
	weapWrench,
	weapCount
};

//------------------------------------------------------------------------------

//! Represents a player
class Player: public BoundedObject, public Destroyable
{
	//!Set relative position of seperate elements
	void translateModel();

	public: NAME(Player) SERIAL(type() | convert(origin)   | convert(rotation)
	                                   | convert((long)id) | convert((long)team)
									   | name              | convert((long)health) )
	UNSERIAL(arg, 6,
		origin = ToPoint(arg[0]);   rotation = ToQuaternion(arg[1]);
		id     = ToInteger(arg[2]); team     = ToInteger(arg[3]);
		name   = arg[4];            health   = ToInteger(arg[5]);
	)
	
	typedef unsigned int Id;

	//! The ID of this player
	Id id;

	//! The team of this player
	unsigned char team;

	//! The name of this player
	string name;

	//! The time of the last shot this player fired
	time_t lastShot;
	
	//! Currently selected weapon to fire
	WeaponType weapon;
	
	//! Currently selected building type to build
	ToolType tool;

	//! Velocity of the player (y-axis)
	Vd velocity, oldVelocity;
	static const Vd maxVelocity;

	//! Model
	struct { ObjectHandle head, body, weapon, tool, wheel; } model;

	//! Constructs a player
	Player(Id _id = (Id)0, unsigned char _team = 'a', string _name = "player",
		Pd P = Pd(), Qd R = Qd());

	//! Updates team colours
	void updateTextures();

	//! Destroys a player
	virtual ~Player();

	//! Update model transformations according to velocity and the camera direction
	void update(const Qd &camobj);
    
    //! Renders the player
    virtual void render();
	
	//! Called every frame
	virtual void frame();

	//! Draw the joints between the tools and the body and the player name and health bar
	virtual void draw();
	
	void interpolate();
};

//------------------------------------------------------------------------------

} // namespace Objects

#endif // _PLAYER_H

//------------------------------------------------------------------------------
