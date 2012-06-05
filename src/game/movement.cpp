/*
 * Movement module -- see header file for more info
 */

#include "movement.h"

namespace Movement {

//------------------------------------------------------------------------------

Controller::Controller(Camera &C, ObjectHandle P) : camera(C), player(P)
{
	fps = false;

	pos = player->origin;

	Vector<double> vec = player->rotation * Vector<double>(0,1,0);

	zoom = 10.0;

	camera.origin = pos - (~vec * zoom);
	camera.lookAt(pos);
}

//------------------------------------------------------------------------------

void Controller::moveX()
{
	if (move[dirLeft])
	{
		Vector<double> vec = player->rotation * Vector<double>(-1,0,0);

		pos = pos + ~vec;
		camera.origin = camera.origin + ~vec;

		//test
		player->origin = pos;
	}
	else if (move[dirRight])
	{
		Vector<double> vec = player->rotation * Vector<double>(1,0,0);

		pos = pos + ~vec;
		camera.origin = camera.origin + ~vec;

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::moveY()
{
	if (move[dirForward])
	{
		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		pos = pos + ~vec;
		camera.origin = camera.origin + ~vec;

		//test
		player->origin = pos;
	}
	else if (move[dirBackward])
	{
		Vector<double> vec = player->rotation * Vector<double>(0,-1,0);

		pos = pos + ~vec;
		camera.origin = camera.origin + ~vec;

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::moveZ()
{
	if (move[dirUp])
	{
		Vector<double> vec = player->rotation * Vector<double>(0,0,1);

		pos = pos + ~vec;
		camera.origin = camera.origin + ~vec;

		//test
		player->origin = pos;
	}
	else if (move[dirDown])
	{
		Vector<double> vec = player->rotation * Vector<double>(0,0,-1);

		pos = pos + ~vec;
		camera.origin = camera.origin + ~vec;

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::lookX()
{
	if (look[dirLeft])
	{
		player->rotation = Qd(Rd(0.01, Vd(0,0,1))) * player->rotation;

		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (~vec * 5.0));
		}
		else
		{
			camera.origin = pos - (~vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirRight])
	{
		player->rotation = Qd(Rd(-0.01, Vd(0,0,1))) * player->rotation;

		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (~vec * 5.0));
		}
		else
		{
			camera.origin = pos - (~vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::lookY()
{
	if (look[dirForward]) // Zoom in
	{
		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		if (zoom > 5.0)
		{
			zoom -= 0.1;
		}
		else if (zoom <= 5.0)
		{
			fps = true;
		}

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (~vec * 5.0));
		}
		else
		{
			camera.origin = pos - (~vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirBackward]) // Zoom out
	{
		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		if (zoom < 15.0) 
		{
			zoom += 0.1;
		}
		
		if (fps == true)
		{
			fps = false;
			zoom = 5.0;
		}

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (~vec * 5.0));
		}
		else
		{
			camera.origin = pos - (~vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::lookZ()
{
	if (look[dirUp])
	{
		player->rotation = Qd(Rd(0.01, Vd(1,0,0))) * player->rotation;

		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (~vec * 5.0));
		}
		else
		{
			camera.origin = pos - (~vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirDown])
	{
		player->rotation = Qd(Rd(-0.01, Vd(1,0,0))) * player->rotation;

		Vector<double> vec = player->rotation * Vector<double>(0,1,0);

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (~vec * 5.0));
		}
		else
		{
			camera.origin = pos - (~vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::frame()
{
	if (move[dirLeft] || move[dirRight])
	{
		moveX();
	}

	if (move[dirForward] || move[dirBackward])
	{
		moveY();
	}

	if (move[dirUp] || move[dirDown])
	{
		moveZ();
	}


	if (look[dirLeft] || look[dirRight])
	{
		lookX();
	}

	if (look[dirForward] || look[dirBackward])
	{
		lookY();
	}

	if (look[dirUp] || look[dirDown])
	{
		lookZ();
	}
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
