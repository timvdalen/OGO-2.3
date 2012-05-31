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

#include "input.h"

#define PRIV(T,x) if (!data) return; T *x = (T *) data;

#define CALL(x, ...) { if (x) (x)(__VA_ARGS__); }

namespace Movement {

void keyboard_event(unsigned char key, int x, int y);
void special_event(int key, int x, int y);
void mouse_event(int button, int state, int x, int y);
void motion_event(int x, int y);

//------------------------------------------------------------------------------

Input::Input(Window &window)
{
	window.select();
	
	glutSpecialFunc(special_event);
	glutMouseFunc(mouse_event);
	glutKeyboardFunc(keyboard_event);
	glutPassiveMotionFunc(motion_event);
	glutMotionFunc(motion_event);
}

//------------------------------------------------------------------------------

Input::~Input()
{
}

//------------------------------------------------------------------------------

void keyboard_event(unsigned char key, int x, int y)
{
}

//------------------------------------------------------------------------------

void special_event(int key, int x, int y)
{
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
