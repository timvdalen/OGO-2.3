/*
 * Movement module -- see header file for more info
 */

#include "movement.h"

namespace Movement {

	double movespeed = 0.5;
	double jetpackspeed = 0.2;
	double lookspeed = 0.035;
	double zoomspeed = 0.5;

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
		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));
		double yaw = atan2(vec.x, vec.y);

		yaw -= (.5 * Pi);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);

		//test
		player->origin = pos;
	}
	else if (move[dirRight])
	{
		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));
		double yaw = atan2(vec.x, vec.y);

		yaw += (.5 * Pi);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::moveY()
{
	if (move[dirForward])
	{
		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));
		double yaw = atan2(vec.x, vec.y);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);

		//test
		player->origin = pos;
	}
	else if (move[dirBackward])
	{
		Vector<double> vec = ~(player->rotation * Vector<double>(0,-1,0));
		double yaw = atan2(vec.x, vec.y);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::moveZ()
{
	if (move[dirUp])
	{
		pos.z = pos.z + jetpackspeed;
		camera.origin.z = camera.origin.z + jetpackspeed;

		//test
		player->origin = pos;
	}
	else if (move[dirDown])
	{
		pos.z = pos.z - jetpackspeed;
		camera.origin.z = camera.origin.z - jetpackspeed;

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::lookX()
{
	if (look[dirLeft])
	{
		player->rotation = Qd(Rd(lookspeed, Vd(0,0,1))) * player->rotation;

		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirRight])
	{
		player->rotation = Qd(Rd(-lookspeed, Vd(0,0,1))) * player->rotation;

		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::lookY()
{
	if (look[dirForward]) // Zoom in
	{
		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));

		if (zoom > 5.0)
		{
			zoom -= zoomspeed;
		}
		else if (zoom <= 5.0)
		{
			fps = true;
		}

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirBackward]) // Zoom out
	{
		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));

		if (zoom < 15.0) 
		{
			zoom += zoomspeed;
		}
		
		if (fps == true)
		{
			fps = false;
			zoom = 5.0;
		}

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::lookZ()
{
	if (look[dirUp])
	{
		Vector<double> mystery = ~(player->rotation * Vector<double>(0,1,0));
		double mysteryYaw = atan2(mystery.x, mystery.y);

		player->rotation = Qd(Rd(lookspeed, Vd(cos(mysteryYaw),-sin(mysteryYaw),0))) * player->rotation;

		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirDown])
	{
		Vector<double> mystery = ~(player->rotation * Vector<double>(0,1,0));
		double mysteryYaw = atan2(mystery.x, mystery.y);

		player->rotation = Qd(Rd(-lookspeed, Vd(cos(mysteryYaw),-sin(mysteryYaw),0))) * player->rotation;

		Vector<double> vec = ~(player->rotation * Vector<double>(0,1,0));

		if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
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
