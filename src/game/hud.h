#ifndef _H_HUD
#define _H_HUD

namespace HUD_objects{
    class HUD;
}

#include <string>
#include "core.h"
#include "video.h"
#include "objects.h"
#include "player.h"
#include "input.h"
#include "world.h"

//! Contains HUD objects
namespace HUD_objects{

using namespace std;
using namespace Core;
using namespace Objects;
using namespace Movement;

class MessageDisplayer;
class StatusDisplayer;
class BuildingSelection;
class CrossHair;
class Widget;

//! Main class for the Heads-up display
class HUD: public Object{
	public:

	//! Width of the screen in pixels
	int width;

	//! Height of the screen in pixels
	int height;

	//! The main MessageDisplayer for this HUD
	MessageDisplayer *messageDisplayer;
    
    //!The main StatusDisplayer for this hhud
    StatusDisplayer *statusDisplayer;
    
    //!The main buildselector for this hud
    BuildingSelection *buildselector;
    
	//! The main CrossHair for this HUD
	CrossHair *crossHair;
    
    Player *currentPlayer;
    
    Team *currentTeam;

	//! Constructs the HUD with width _width and height _height. 
	HUD(int _width, int _height, World* _w);

	//! Notifies the HUD that the screen size has changed
	void resize(int _width, int _height);

	//! Sets up 2D drawing mode
	void preRender();

	//! Draws its children and switches back to 3D drawing mode
	void postRender();
};

//! Object for display on the HUD
class Widget: public Object{
	public:
	//! X offset of this widget in pixels
	int xOffset;

	//! Y offset of this widget in pixels
	int yOffset;

	//! Width of this widget in pixels
	int width;

	//! Height of this widget in pixels
	int height;

	//! Constructs a new widget with a custom Material
	Widget(int _x, int _y, int _width, int _height, MaterialHandle M);

	//! Replaces the widget
	void replace(int _x, int _y);

	//! Resizes the widget
	void resize(int _width, int _height);

	//! Renders the widget
	virtual void render();
};

//! Represents a displayable message
class DisplayMessage{
	public:
	//! Gives a textual representation of this message
	virtual string toString(){return "emp";}
};

//! Represents a chat message
class ChatMessage: public DisplayMessage{
	public:
	//! The player that sent this message
	Player player;

	//! The actual message
	string message;

	//! Constructs a new ChatMessage
	ChatMessage(Player _player, string _message);

	//! Formats this message as <[player.name]>: [message]
	virtual string toString();
};

//! Represents a kill made by a player
class PlayerFragMessage: public DisplayMessage{
	public:
	//! The player that made the frag
	Player killer;

	//! The player that was fragged
	Player victim;

	//! Constructs a new PlayerFragMessage
	PlayerFragMessage(Player _killer, Player _victim);

	//! Formats this message as <[killer.name]> fragged <[victim.name]>
	virtual string toString();
};

//! Represents a kill made by a tower
class TowerFragMessage: public DisplayMessage{
	public:
	//! The player fragged by a tower
	Player player;

	//TODO: Add tower position or name
	
	
	//! Constructs a new TowerFragMessage
	TowerFragMessage(Player _player);

	//! Formats this message as <[victim.name]> was fragged by a tower
	virtual string toString();
};

//! Represents a system message
class SystemMessage: public DisplayMessage{
	public:
	//! The message
	string message;
	
	//! Constructs a new SystemMessage
	SystemMessage(string _message);
	
	//! Formats this message as ** <message> **
	virtual string toString();
};

//! Displays a variety of messages
class MessageDisplayer: public Widget{
	//! The last time a message was added
	int lastMessage;

	public:
	//! Circluar array that contains the messages
	Handle<DisplayMessage> messages[10];

	//! Array index that points to the current head
	
	//! On the head is the last message that was added
	int curr;

	//! Number of items that are filled
	int full;
	
	//! Whether or not to always show this displayer
	bool showAlways;

	public:
	//! Constructs a new MessageDisplayer
	MessageDisplayer(int _x, int _y, int _width, int _height);

	//! Adds a message to the queue
	void addMessage(Handle<DisplayMessage> m);
	
	//! Whether or not to always show this displayer
	void setShowAlways(bool show = true);
	
	//! Whether or not to always show this displayer
	bool getShowAlways();

	//! Displays the messages
	virtual void draw();

	//! Renders this object
	virtual void render();
};

class CrossHair: public Widget{
	public:

	//! Constructs the crosshair
	CrossHair(int _x, int _y, int _width, int _height);

	//! Displays the crosshair
	virtual void draw();
};

class BuildingSelection: public Widget{
    public:
    
    bool show;
    
    //! 0 for no selection, 1 for mine, 2 for tower, etc...
    int selected;
    
    //! Constructs the status displayer
    BuildingSelection(int _x, int _y, int _width, int _height);
        
    //! Displays the status
    virtual void draw();
    
    //! Renders this object
    virtual void render();
};
    
    
class StatusDisplayer: public Widget{
    public:
        
        Player* p;
        Team* t;
    //! Constructs the status displayer
    StatusDisplayer(int _x, int _y, int _width, int _height, Team* _t, Player *_p);
        
    //! Displays the status
    virtual void draw();
        
    //! Renders this object
    virtual void render();
};

class TextInput: public Widget{
	//! The \ref Input this TextInput reads from
	Input *input;
	
	public:
	//! Marks the TextInput as done
	bool done;
	
	//! Constructs a new TextInput widget
	TextInput(Input *_input, int _x, int _y, int _width, int _height);
	
	//! Displays the TextInput
	void draw();
};
    
class MiniMap: public Widget{
    
    World* w;
    
    public:     
    //! Constructs the status displayer
    MiniMap(int _x, int _y, int _width, int _height, World* _w);
        
    //! Displays the status
    virtual void draw();
        
    //! Renders this object
    virtual void render();
};

}

#endif

