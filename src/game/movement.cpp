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

void Controller::frame()
{
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
