/*
 * Movement module -- see header file for more info
 */

#include "movement.h"

namespace Movement {

//------------------------------------------------------------------------------

Controller::Controller(Camera &C, Player &P) : camera(C), player(P)
{
}

//------------------------------------------------------------------------------

void Controller::moveX(Direction dir)
{
	if (dir == dirLeft)
	{
	}
	else if (dir == dirRight)
	{
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
	}
	else if (dir == dirBackward)
	{
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
	}
	else if (dir == dirDown)
	{
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
