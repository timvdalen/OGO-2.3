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

#include <map>
#include <string>

#include "core.h"
#include "objects.h"
#include "world.h"

#define PLAYER(id) (game.players.count(id) ? TO(Player, game.players[id]) : 0)

//! Definition of the game action
//! \warning Do not use anything if not initialized \see Initialize
namespace Game {

using namespace Core;
using namespace Objects;

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

//! Game state data set
struct GameState
{
	World *world;
	Player *player;
	string gameName;
	
	map<Player::Id,ObjectHandle> players;
	
	GameState() : world(0), player(0) {}
} extern game; //! Current game state

//------------------------------------------------------------------------------
// General

void Echo(string msg);   //!< Prints a message in the hud
void Notice(string msg); //!< Prints a notice on screen
void Prompt(string cmd); //!< Opens the console with the specified string in place

void Quit();              //!< Quits to game
void RQuit(string msg);   //!< Rage quits with the supplied swear words

//! Adds a bind to the default binding
//! \see Binding::bind
void Bind(string button, string line);

//! Executes each line of a file
void Exec(string filename);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Network

void Connect(string address); //!< Connects to the specified address
void Disconnect();            //!< Disconnects and starts listening for clients

void Say(string msg);         //!< Sends a chat message to all
void TeamSay(string msg);     //!< Sends a chat message to all team members

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Movement
//! \invariant (0.0 <= speed <= 1.0)

void MoveX(double speed); //!< Moves the player left and right
void MoveY(double speed); //!< Moves the player forward and backward
void MoveZ(double speed); //!< Moves the player up and down

void LookX(double speed); //!< Looks left and right
void LookY(double speed); //!< Zooms in and out
void LookZ(double speed); //!< Looks up and down

void Jump(); //!< Makes the player jump (possibly not implemented)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Player actions

void Fire();  //!< Fires the primary weapon in the current look direction
void Build(); //!< Builds with the primary tool at the seelcted grid unit

void Weapon(WeaponType weapon); //!< Sets the primary weapon
void Tool(ToolType tool);       //!< Sets the primary build tool

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Events

void DisplayChatMsg(Player *, string line);
void DisplayTeamMsg(Player *, string line);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Diagnostics

void PrintFPS();
void PrintCPS();

//------------------------------------------------------------------------------

} // namespace Game

#endif // _GAME_H

//------------------------------------------------------------------------------
