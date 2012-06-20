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
#include "video.h"
#include "input.h"
#include "movement.h"
#include "objects.h"
#include "world.h"
#include "player.h"

#define PLAYER(id) (game.players.count(id) ? TO(Player, game.players[id]) : 0)

//! Definition of the game action
//! \warning Do not use anything if not initialized \see Initialize
namespace Game {

using namespace Core;
using namespace Video;
using namespace Objects;
using namespace Movement;

//------------------------------------------------------------------------------

//! Set the game in its initial state
void Initialize(int argc, char *argv[]);
//! Destroys the current game state
void Terminate();

//! Call a gamefunction dynamicly
void Call(std::string command);
//! Returns whether given string contains a command
bool Callable(std::string command);

Point<double> getSpawn(const char team);

//------------------------------------------------------------------------------

//! Game state data set
struct GameState
{
	Window *window;
	Input *input;
	Controller *controller;
	
	ObjectHandle root;
	World *world;
	Player *player;
	string gameName;
	
	map<Player::Id,ObjectHandle> players;
	map<unsigned char,Team> teams;
	
	Player::Id topId;
	bool connecting;
	
	GameState() : window(0), input(0), controller(0), world(0), player(0), topId(1) {}
} extern game; //! Current game state

//------------------------------------------------------------------------------
// General

void Echo(string msg);   //!< Prints a message in the hud
void Notice(string msg); //!< Prints a notice on screen
void Prompt(string cmd); //!< Opens the console with the specified string in place

void ShowLog();          //!< Shows the message log
void HideLog();          //!< Hides the message log

void Quit();             //!< Quits to game
void RQuit(string msg);  //!< Rage quits with the supplied swear words

//! Adds a bind to the default binding
//! \see Binding::bind
void Bind(string button, string line);

void Exec(string filename);         //!< Executes each line of a file
void Get(string key);               //!< Gets a value from the config file
void Set(string key, string value); //!< Sets a value in the config file

void GrabMouse();        //!< Takes hold of the cursor and locks it in place
void ReleaseMouse();     //!< Releases the cursor when grabbed

// Commands not in header: Chain, Toggle

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

void Goto(string cmd); //!< Cheat command

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Events

void DisplayChatMsg(Player *, string line); //!< Displays a chat message in the log
void DisplayTeamMsg(Player *, string line); //!< Displays a team chat message in the log
void DisplayFragMsg(Player *, Player*);     //!< Displays a tower/player kill message

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Diagnostics

void PrintFPS(); //!< Prints current frame rate (frames per second)
void PrintCPS(); //!< Prints current net speed (cycles per second)
void GameSpeed(double speed);

void NetDebug();
void PrintWorld();
void PrintPlayers();
void Test(string str);

//------------------------------------------------------------------------------

} // namespace Game

#endif // _GAME_H

//------------------------------------------------------------------------------
