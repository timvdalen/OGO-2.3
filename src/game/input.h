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

#include "base.h"
#include "video.h"

namespace Movement {

using namespace Video;

//------------------------------------------------------------------------------

//! Input module
//! Detects keyboard and mouse activity
class Input 
{
	public:
	//! Button enumeration
	enum Button
	{
		btnUnknown = 0,
		btnMouseLeft = 1, btnMouseRight, btnMouseMiddle, btnMouseScrollUp,
		btnMouseScrollDown,
		
		btnKeySpace = 32,
		
		btnKey0 = 48, btnKey1, btnKey2, btnKey3, btnKey4, btnKey5, btnKey6,
		btnKey7, btnKey8, btnKey9,
		
		btnKeyA = 65, btnKeyB, btnKeyC, btnKeyD, btnKeyE, btnKeyF, btnKeyG,
		btnKeyH, btnKeyI, btnKeyJ, btnKeyK, btnKeyL, btnKeyM, btnKeyN, btnKeyO,
		btnKeyP, btnKeyQ, btnKeyR, btnKeyS, btnKeyT, btnKeyU, btnKeyV, btnKeyW,
		btnKeyX, btnKeyY, btnKeyZ,
		
		btnKeyArrowUp = 128, btnKeyArrowDown, btnKeyArrowLeft, btnKeyArrowRight
	};
	
	Input(Window &); //!< Assigns an input listener to a window
	~Input();        //!< Destroys input listener
	
	void (*onKeyUp) (Button);             //!< Callback: a key was released
	void (*onKeyDown) (Button);           //!< Callback: a key was pressed
	void (*onMouseMove) (word x, word y); //!< Callback: the mouse was moved
	
	private:
	Window &window;
	void *data;
};

//------------------------------------------------------------------------------

} // namespace Movement

#endif // _INPUT_H

//------------------------------------------------------------------------------
