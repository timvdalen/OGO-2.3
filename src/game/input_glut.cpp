/*
 * Input module -- see header file for more info
 */

#include <stdlib.h>

#include <set>

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include "input.h"

#define PRIV(T,x) if (!data) return; T *x = (T *) data;

#define CALL(x, ...) { if (x) (x)(__VA_ARGS__); }

namespace Movement {

using namespace std;

set<Input *>inputs;

void keyboard_down_event(unsigned char key, int x, int y);
void keyboard_up_event(unsigned char key, int x, int y);
void special_down_event(int key, int x, int y);
void special_up_event(int key, int x, int y);
void mouse_event(int button, int state, int x, int y);
void motion_event(int x, int y);

//------------------------------------------------------------------------------

Input::Input(Window &W)
	: window(W), onKeyDown(NULL), onKeyUp(NULL), onMouseMove(NULL),
	  mouseX(0), mouseY(0), dx(0), dy(0), grabbing(false)
{
	window.select();
	
	glutSpecialFunc(special_down_event);
	glutSpecialUpFunc(special_up_event);
	glutMouseFunc(mouse_event);
	glutKeyboardFunc(keyboard_down_event);
	glutKeyboardUpFunc(keyboard_up_event);
	glutPassiveMotionFunc(motion_event);
	glutMotionFunc(motion_event);
	
	inputs.insert(this);
}

//------------------------------------------------------------------------------

Input::~Input()
{
	inputs.erase(this);
	window.select();
	
	glutSpecialFunc(NULL);
	glutSpecialUpFunc(NULL);
	glutMouseFunc(NULL);
	glutKeyboardFunc(NULL);
	glutKeyboardUpFunc(NULL);
	glutPassiveMotionFunc(NULL);
	glutMotionFunc(NULL);
}
//------------------------------------------------------------------------------

void Input::grabMouse()
{
	dx = 0.0;
	dy = 0.0;
	grabbing = true;
	
	uword width, height;
	window.size(width = 0, height = 0);
	glutWarpPointer(width / 2, height / 2);
	glutSetCursor(GLUT_CURSOR_NONE);
}

//------------------------------------------------------------------------------

void Input::releaseMouse()
{
	grabbing = false;
	
	glutSetCursor(GLUT_CURSOR_INHERIT);
}

//------------------------------------------------------------------------------

void Input::frame()
{
	if (!grabbing) return;
	
	const word &r = MouseResolution;
	
	uword width, height;
	window.size(width = 0, height = 0);
	width /= 2;
	height /= 2;
	
	dx += mouseX - width;
	dy += mouseY - height;
	
	if (dx < -r)
	{
		CALL(onKeyDown, btnMouseMoveLeft);
		CALL(onKeyUp,   btnMouseMoveLeft);
	}
	else if (dx > r)
	{
		CALL(onKeyDown, btnMouseMoveRight);
		CALL(onKeyUp,   btnMouseMoveRight);
	}
	
	if (dy < -r)
	{
		CALL(onKeyDown, btnMouseMoveUp);
		CALL(onKeyUp,   btnMouseMoveUp);
	}
	else if (dy > r)
	{
		CALL(onKeyDown, btnMouseMoveDown);
		CALL(onKeyUp,   btnMouseMoveDown);
	}
	
	dx *= .75;
	dy *= .75;
	
	
	glutWarpPointer(width, height);
}

//------------------------------------------------------------------------------

void keyboard_down_event(unsigned char key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	if ((key >= 'a') && (key <= 'z'))
		CALL(input->onKeyDown, (Button) (key - ('a'-'A')))
	else if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
		CALL(input->onKeyDown, (Button) (key))
	else if ((key == ' ') || (key == '\e'))
		CALL(input->onKeyDown, (Button) key);
}

//------------------------------------------------------------------------------

void keyboard_up_event(unsigned char key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	if ((key >= 'a') && (key <= 'z'))
		CALL(input->onKeyUp, (Button) (key - ('a'-'A')))
	else if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
		CALL(input->onKeyUp, (Button) (key))
	else if ((key == ' ') || (key == '\e'))
		CALL(input->onKeyUp, (Button) key);
}

//------------------------------------------------------------------------------

void special_down_event(int key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	switch (key)
	{
		case GLUT_KEY_LEFT:  CALL(input->onKeyDown, btnKeyArrowLeft);  break;
		case GLUT_KEY_RIGHT: CALL(input->onKeyDown, btnKeyArrowRight); break;
		case GLUT_KEY_UP:    CALL(input->onKeyDown, btnKeyArrowUp);    break;
		case GLUT_KEY_DOWN:  CALL(input->onKeyDown, btnKeyArrowDown);  break;
	}
}

//------------------------------------------------------------------------------

void special_up_event(int key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	switch (key)
	{
		case GLUT_KEY_LEFT:  CALL(input->onKeyUp, btnKeyArrowLeft);  break;
		case GLUT_KEY_RIGHT: CALL(input->onKeyUp, btnKeyArrowRight); break;
		case GLUT_KEY_UP:    CALL(input->onKeyUp, btnKeyArrowUp);    break;
		case GLUT_KEY_DOWN:  CALL(input->onKeyUp, btnKeyArrowDown);  break;
	}
}
//------------------------------------------------------------------------------

void mouse_event(int button, int state, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	Button btn;
	switch (button)
	{
		case GLUT_LEFT_BUTTON:   btn = btnMouseLeft;       break;
		case GLUT_RIGHT_BUTTON:  btn = btnMouseRight;      break;
		case GLUT_MIDDLE_BUTTON: btn = btnMouseMiddle;     break;
		case 3:                  btn = btnMouseScrollUp;   break;
		case 4:                  btn = btnMouseScrollDown; break;
		default: return;
	}
	
	if (state == GLUT_DOWN)
		CALL(input->onKeyDown, btn)
	else if (state == GLUT_UP)
		CALL(input->onKeyUp, btn)
}

//------------------------------------------------------------------------------

void motion_event(int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	CALL(input->onMouseMove, (word) x, (word) y)
	
	input->mouseX = x;
	input->mouseY = y;
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
