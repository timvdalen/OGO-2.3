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
	: window(W), onKeyDown(NULL), onKeyUp(NULL), onMouseMove(NULL)
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

void keyboard_down_event(unsigned char key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	if ((key >= 'a') && (key <= 'z'))
		CALL(input->onKeyDown, (Input::Button) (key - ('a'-'A')))
	else if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
		CALL(input->onKeyDown, (Input::Button) (key))
	else if (key == ' ')
		CALL(input->onKeyDown, Input::btnKeySpace);
}

//------------------------------------------------------------------------------

void keyboard_up_event(unsigned char key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	if ((key >= 'a') && (key <= 'z'))
		CALL(input->onKeyUp, (Input::Button) (key - ('a'-'A')))
	else if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
		CALL(input->onKeyUp, (Input::Button) (key))
	else if (key == ' ')
		CALL(input->onKeyUp, Input::btnKeySpace);
}

//------------------------------------------------------------------------------

void special_down_event(int key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	switch (key)
	{
		case GLUT_KEY_LEFT: CALL(input->onKeyDown, Input::btnKeyArrowLeft); break;
		case GLUT_KEY_RIGHT: CALL(input->onKeyDown, Input::btnKeyArrowRight); break;
		case GLUT_KEY_UP: CALL(input->onKeyDown, Input::btnKeyArrowUp); break;
		case GLUT_KEY_DOWN: CALL(input->onKeyDown, Input::btnKeyArrowDown); break;
	}
}

//------------------------------------------------------------------------------

void special_up_event(int key, int x, int y)
{
	if (inputs.empty()) return;
	Input *input = *inputs.begin();
	
	switch (key)
	{
		case GLUT_KEY_LEFT: CALL(input->onKeyUp, Input::btnKeyArrowLeft); break;
		case GLUT_KEY_RIGHT: CALL(input->onKeyUp, Input::btnKeyArrowRight); break;
		case GLUT_KEY_UP: CALL(input->onKeyUp, Input::btnKeyArrowUp); break;
		case GLUT_KEY_DOWN: CALL(input->onKeyUp, Input::btnKeyArrowDown); break;
	}
}
//------------------------------------------------------------------------------

void mouse_event(int button, int state, int x, int y)
{
}

//------------------------------------------------------------------------------

void motion_event(int x, int y)
{
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
