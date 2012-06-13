/*
 * Input module
 *
 * Date: 31-05-12 10:57
 *
 * Description: Catches keyboard and mouse activity
 *
 */

#ifndef _INPUT_H
#define _INPUT_H

#include <map>
#include <string>

#include "base.h"
#include "video.h"

namespace Movement {

using namespace Video;

//------------------------------------------------------------------------------

//! Button enumeration
enum Button
{
	btnUnknown = 0,
	btnMouseLeft = 192, btnMouseRight, btnMouseMiddle, btnMouseScrollUp,
	btnMouseScrollDown, btnMouseMoveLeft, btnMouseMoveRight, btnMouseMoveUp,
	btnMouseMoveDown,
	
	btnKeySpace = 32, btnKeyEscape = 27, btnKeyEnter = 13, btnKeyTab = 9,
	
	btnKey0 = 48, btnKey1, btnKey2, btnKey3, btnKey4, btnKey5, btnKey6,
	btnKey7, btnKey8, btnKey9,
	
	btnKeyA = 65, btnKeyB, btnKeyC, btnKeyD, btnKeyE, btnKeyF, btnKeyG,
	btnKeyH, btnKeyI, btnKeyJ, btnKeyK, btnKeyL, btnKeyM, btnKeyN, btnKeyO,
	btnKeyP, btnKeyQ, btnKeyR, btnKeyS, btnKeyT, btnKeyU, btnKeyV, btnKeyW,
	btnKeyX, btnKeyY, btnKeyZ,
	
	btnKeyArrowUp = 128, btnKeyArrowDown, btnKeyArrowLeft, btnKeyArrowRight
};

//! Gets the button from a specified string
Button ToButton(std::string);
//! Gets the string alias of given button
std::string convert(Button);

//------------------------------------------------------------------------------

//! Input module
//! Detects keyboard and mouse activity
class Input 
{
	public:
	Input(Window &); //!< Assigns an input listener to a window
	~Input();        //!< Destroys input listener
	
	word mouseX;    //!< Current horizontal mouse position
	word mouseY;    //!< Current vertical mouse position
	Window &window; //!< Assigned window
	bool grabbing;  //!< If the mouse is grabbed or not
	bool textMode;  //!< If text mode is enabled or not
	std::string text; //!< Text recorded in text mode so far
	
	void (*onKeyUp) (Button);             //!< Callback: a key was released
	void (*onKeyDown) (Button);           //!< Callback: a key was pressed
	void (*onMouseMove) (word x, word y); //!< Callback: the mouse was moved
	void (*onText) (std::string text);    //!< Callback: text mode ended (on release or enter was pressed)
	
	//! Locks (and hides) the mouse pointer in place
	void grabMouse();
	//! Releases the mouse pointer to the user
	void releaseMouse();
	
	//! Grabs text input
	void grabText();
	//! Stops grabbing text input and fire the onText event.
	void releaseText();
	
	//! Processes internal mouse events
	void mouseEvents();
	
	private:
	void *data;
};

//------------------------------------------------------------------------------

//! Maps commands to key presses and releases
class Binding
{
	public:
	typedef map<Button,std::string> Bind; //!< Binding type
	Bind down;                //!< Binding on key press
	Bind up;                  //!< Binding on key release
	
	//! Create a new binding from a button and command string
	//! \note use a '|' to specify an up event
	void bind(std::string button, std::string line);
	
	void processUp(Button);   //!< Execute command bound on key release
	void processDown(Button); //!< Execute command bound on key press
} extern binds; //!< Default binding

//------------------------------------------------------------------------------

} // namespace Movement

#endif // _INPUT_H

//------------------------------------------------------------------------------
