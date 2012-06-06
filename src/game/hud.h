#ifndef _H_HUD
#define _H_HUD

#include <string>
#include "core.h"
#include "video.h"

//! Contains HUD objects
namespace HUD_objects{

using namespace std;
using namespace Core;

//! Main class for the Heads-up display
class HUD: public Object{
	public:

	//! Width of the screen in pixels
	int width;

	//! Height of the screen in pixels
	int height;

	//! Constructs the HUD with width _width and height _height. 
	HUD(int _width, int _height);

	//! Notifies the HUD that the screen size has changed
	void resize(int _width, int _height);

	//! Sets up 2D drawing mode
	void preRender();

	//! Switches back to 3D drawing mode
	void postRender();
};

//! Represents a displayable message
class Message: public Object{
	public:
	//! Gives a textual representation of this message
	virtual string toString() = 0;
};

//! Represents a chat message
class ChatMessage: public Message{
	public:
	//! The player that sent this message
	Core::Player player;

	//! The actual message
	string message;

	//! Constructs a new ChatMessage
	ChatMessage(Core::Player _player, string _message);

	//! Formats this message as <[player.name]>: [message]
	string toString();
};

//! Represents a kill made by a player
class PlayerFragMessage: public Message{
	public:
	//! The player that made the frag
	Core::Player killer;

	//! The player that was fragged
	Core::Player victim;

	//! Constructs a new PlayerFragMessage
	PlayerFragMessage(Core::Player _killer, Core::Player _victim);

	//! Formats this message as <[killer.name]> fragged <[victim.name]>
	string toString();
};

//! Represents a kill made by a tower
class TowerFragMessage: public Message{
	public:
	//! The player fragged by a tower
	Core::Player player;

	//TODO: Add tower position or name
	
	
	//! Constructs a new TowerFragMessage
	TowerFragMessage(Core::Player _player);

	//! Formats this message as <[victim.name]> was fragged by a tower
	string toString();
};

}

#endif
