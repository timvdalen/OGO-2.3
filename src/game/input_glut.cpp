/*
 * Input module -- see header file for more info
 */

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <set>
#include <map>

#include "game.h"
#include "input.h"

#define PRIV(T,x) if (!data) return; T *x = (T *) data;

#define CALL(x, ...) { if (x) (x)(__VA_ARGS__); }

#define ESC 27

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

string to_lower_case(string str);

struct ButtonAlias
{
	typedef map<string,Button> List;
	static List list;
	ButtonAlias(string str, Button btn) { list[to_lower_case(str)] = btn; }
	ButtonAlias(char start, char end, Button first)
	{
		for (int i = first; start <= end; ++start)
			list[string(1, start)] = (Button) i++;
	}
};

ButtonAlias::List ButtonAlias::list;

#define BTN(x,y) ButtonAlias _ ## x(#x,y);

ButtonAlias   alpha('a', 'z', btnKeyA);
ButtonAlias numeric('0', '9', btnKey0);

BTN(LeftClick,   btnMouseLeft)
BTN(RightClick,  btnMouseRight)
BTN(MiddleClick, btnMouseMiddle)
BTN(ScrollUp,    btnMouseScrollUp)
BTN(ScrollDown,  btnMouseScrollDown)

BTN(MouseLeft, btnMouseMoveLeft) BTN(MouseRight, btnMouseMoveRight)
BTN(MouseUp,   btnMouseMoveUp)   BTN(MouseDown,  btnMouseMoveDown)
	
BTN(Space,  btnKeySpace)
BTN(Escape, btnKeyEscape)
BTN(Enter,  btnKeyEnter)
BTN(Tab,    btnKeyTab)

BTN(UpArrow,   btnKeyArrowUp)   BTN(DownArrow,  btnKeyArrowDown)
BTN(LeftArrow, btnKeyArrowLeft) BTN(RightArrow, btnKeyArrowRight)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Button ToButton(std::string str)
{
	str = to_lower_case(str);
	if (!ButtonAlias::list.count(str))
		return btnUnknown;
	else
		return ButtonAlias::list[str];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

std::string convert(Button btn)
{
	ButtonAlias::List::iterator it;
	for (it = ButtonAlias::list.begin(); it != ButtonAlias::list.end(); ++it)
		if (it->second == btn)
			return it->first;
	
	return "";
}

//------------------------------------------------------------------------------

Input::Input(Window &W)
	: window(W), onKeyDown(NULL), onKeyUp(NULL), onMouseMove(NULL),
	  mouseX(0), mouseY(0), grabbing(false), textMode(false), text()
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

void Input::grabText()
{
	textMode = true;
}

//------------------------------------------------------------------------------

void Input::releaseText()
{
	textMode = false;
	CALL(onText, text);
	text = "";
}

//------------------------------------------------------------------------------

void Input::mouseEvents()
{
	if (!grabbing) return;

	uword width, height;
	window.size(width = 0, height = 0);
	width /= 2;
	height /= 2;
	
	word dx = mouseX - width;
	word dy = mouseY - height;

	if (dx < 0)
	{
		CALL(onKeyDown, btnMouseMoveLeft);
		CALL(onKeyUp,   btnMouseMoveLeft);
	}
	else if (dx > 0)
	{
		CALL(onKeyDown, btnMouseMoveRight);
		CALL(onKeyUp,   btnMouseMoveRight);
	}
	
	if (dy < 0)
	{
		CALL(onKeyDown, btnMouseMoveUp);
		CALL(onKeyUp,   btnMouseMoveUp);
	}
	else if (dy > 0)
	{
		CALL(onKeyDown, btnMouseMoveDown);
		CALL(onKeyUp,   btnMouseMoveDown);
	}
	
	if (dx || dy)
		glutWarpPointer(width, height);
}

//------------------------------------------------------------------------------

Binding binds;

void Binding::processUp(Button btn)
{
	if (!up.count(btn)) return;
	Game::Call(up[btn]);
}

//------------------------------------------------------------------------------

void Binding::processDown(Button btn)
{
	if (!down.count(btn)) return;
	Game::Call(down[btn]);
}

//------------------------------------------------------------------------------

void Binding::bind(string button, string cmd)
{	
	Button btn = ToButton(button);
	if (btn == btnUnknown) return;
	
	size_t pos = cmd.find('|');
	if (pos != string::npos)
	{
		size_t pos2 = pos;
		while (cmd[--pos]  == ' ');
		while (cmd[++pos2] == ' ');
		down[btn] = cmd.substr(0, pos + 1);
		up[btn] = cmd.substr(pos2);
	}
	else
	{
		down[btn] = cmd;
		up.erase(btn);
	}
}

//------------------------------------------------------------------------------

void keyboard_down_event(unsigned char key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	if (input->textMode)
	{
		if (key == ESC) //Aborted
		{
			input->text = "";
			CALL(input->onText, input->text);
			input->textMode = false;
		}
		else if (key == '\r' || key == '\n') // Enter
		{
			CALL(input->onText, input->text);
			input->text = "";
			input->textMode = false;
		}
		else if (key == '\b')
		{
			if(!input->text.empty())
				input->text.erase(input->text.end() - 1, input->text.end());
		}
		else
			input->text.push_back(key);
		return;
	}
	
	if ((key >= 'a') && (key <= 'z'))
		CALL(input->onKeyDown, (Button) (key - ('a'-'A')))
	else if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
		CALL(input->onKeyDown, (Button) (key))
	else if ((key == ' ') || (key == ESC) || (key == '\t'))
		CALL(input->onKeyDown, (Button) key)
	else if ((key == '\r') || (key == '\n'))
		CALL(input->onKeyDown, btnKeyEnter)
}

//------------------------------------------------------------------------------

void keyboard_up_event(unsigned char key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	if (input->textMode) return;
	
	if (key == '\r') key = '\n';
	
	if ((key >= 'a') && (key <= 'z'))
		CALL(input->onKeyUp, (Button) (key - ('a'-'A')))
	else if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
		CALL(input->onKeyUp, (Button) (key))
	else if ((key == ' ') || (key == ESC) || (key == '\t'))
		CALL(input->onKeyUp, (Button) key)
	else if ((key == '\r') || (key == '\n'))
		CALL(input->onKeyUp, btnKeyEnter)
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
	input->mouseEvents();
}

//------------------------------------------------------------------------------

string to_lower_case(string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
