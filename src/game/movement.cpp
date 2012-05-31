/*
 * Movement module -- see header file for more info
 */

#include "movement.h"

namespace Movement {

//------------------------------------------------------------------------------

Controller::Controller(Camera &C, Player &P) : camera(C), player(P)
{
	pos.x = player.origin.x;
	pos.y = player.origin.y;
	pos.z = player.origin.z;

	Vector<double> vec = player.rotation * Vector<double>(0,1,0);
	~vec;

	zoom = 10.0;

	camera.origin = pos - (vec * zoom);
	camera.lookAt(pos);
}

//------------------------------------------------------------------------------

void Controller::moveX(Direction dir)
{
	if (dir == dirLeft)
	{
		Vector<double> vec = player.rotation * Vector<double>(-1,0,0);
		~vec;

		pos = pos + vec;
		camera.origin = camera.origin + vec;

		//test
		player.origin = pos;
	}
	else if (dir == dirRight)
	{
		Vector<double> vec = player.rotation * Vector<double>(1,0,0);
		~vec;

		pos = pos + vec;
		camera.origin = camera.origin + vec;

		//test
		player.origin = pos;
	}
	else // Key released
	{
	}
}

//------------------------------------------------------------------------------

void Controller::moveY(Direction dir)
{
	if (dir == dirForward)
	{
		Vector<double> vec = player.rotation * Vector<double>(0,1,0);
		~vec;

		pos = pos + vec;
		camera.origin = camera.origin + vec;

		//test
		player.origin = pos;
	}
	else if (dir == dirBackward)
	{
		Vector<double> vec = player.rotation * Vector<double>(0,-1,0);
		~vec;

		pos = pos + vec;
		camera.origin = camera.origin + vec;

		//test
		player.origin = pos;
	}
	else // Key released
	{
	}
}

//------------------------------------------------------------------------------

void Controller::moveZ(Direction dir)
{
	if (dir == dirUp)
	{
		Vector<double> vec = player.rotation * Vector<double>(0,0,1);
		~vec;

		pos = pos + vec;
		camera.origin = camera.origin + vec;

		//test
		player.origin = pos;
	}
	else if (dir == dirDown)
	{
		Vector<double> vec = player.rotation * Vector<double>(0,0,-1);
		~vec;

		pos = pos + vec;
		camera.origin = camera.origin + vec;

		//test
		player.origin = pos;
	}
	else // Key released
	{
	}
}

//------------------------------------------------------------------------------

void Controller::lookX(Direction dir)
{
	if (dir == dirLeft)
	{
		Vector<double> vec = player.rotation * Vector<double>(0,1,0);
		~vec;
		camera.origin = camera.origin + (vec * zoom);

		player.rotation = player.rotation * Rotation<double>(0.001, Vector<double>(0,0,-1));
	}
	else if (dir == dirRight)
	{
	}
	else // Key released
	{
	}
}

//------------------------------------------------------------------------------

void Controller::lookY(Direction dir)
{
	if (dir == dirForward) // Zoom in
	{
	}
	else if (dir == dirBackward) // Zoom out
	{
	}
	else // Key released
	{
	}
}

//------------------------------------------------------------------------------

void Controller::lookZ(Direction dir)
{
	if (dir == dirUp)
	{
	}
	else if (dir == dirDown)
	{
	}
	else // Key released
	{
	}
}

//------------------------------------------------------------------------------

void Controller::frame()
{
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
