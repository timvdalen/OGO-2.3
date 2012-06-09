/*
 * Game module
 *
 * Date: 04-06-12 14:00
 *
 * Description: Handles game logic
 *
 */

#ifndef _GAME_H
#define _GAME_H

#include <string>

#include "core.h"
#include "objects.h"
#include "world.h"

namespace Game {

using namespace Core;

//------------------------------------------------------------------------------

//! Set the game in its initial state
void Initialize(int argc, char *argv[]);
//! Destroys the current game state
void Terminate();

//! Call a gamefunction dynamicly
void Call(std::string command);
//! Returns whether given string contains a command
bool Callable(std::string command);

//------------------------------------------------------------------------------

struct GameState
{
	ObjectHandle world;
	ObjectHandle player;
	string gameName;
};

//------------------------------------------------------------------------------

void Echo(string msg); //!< Prints a message in the hud

//------------------------------------------------------------------------------

} // namespace Game

#endif // _GAME_H

//------------------------------------------------------------------------------
