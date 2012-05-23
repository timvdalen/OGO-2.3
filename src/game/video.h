/*
 * Video subsystem
 *
 * Date: 19-05-12 15:47
 *
 * Description: Wrapper for hardware accelerated graphics rendering.
 *              Currently uses OpenGL
 *
 */

#ifndef _VIDEO_H
#define _VIDEO_H

#if defined _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <gl\gl.h>
	#include <gl\glu.h>
	#include <gl\freeglut.h>
	#include <gl\glext.h>
#elif defined __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/freeglut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/freeglut.h>
#endif

#include <set>

#include "base.h"
#include "core.h"

//!Video subsystem
namespace Video {

using namespace Core;

class Window;
class Viewport;
struct Camera;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[]);
void Terminate();

void StartEventLoop();
void StopEventLoop();

//------------------------------------------------------------------------------

class Window
{
	public:
	std::set<Viewport *> viewports;
	
	Window(uword width, uword height,
	       const char *title, word xpos = -1, word ypos = -1);
	~Window();
	
	void render();
	
	friend void Initialize(int argc, char *argv[]);
	friend void Terminate();
	
	private:
	void *data;
	
	static std::set<Window *> windows;
	static void display();
};

//------------------------------------------------------------------------------

struct Camera
{
	Point<double> origin;         //!< focal point
	Quaternion<double> objective; //!< direction and focal length^W^W^W
};

//------------------------------------------------------------------------------

class Viewport
{
	public:
	Camera camera;
	ObjectHandle world;
	
	Viewport(uword width, uword height, word xpos = 0, word ypos = 0,
	         double fov = 45.0);
	~Viewport();
	
	//! Changes viewport offset (relative to screen)
	void move(word xpos, word ypos);
	//! Resizes viewport
	void resize(uword width, uword height);
	//! Changes the field of view of the viewport
	void setfov(double fov);
	
	//! Actuates viewport
	void select();
	//! Draws screne viewed from camera in viewport
	void render();
	
	private:
	void *data;
};

//------------------------------------------------------------------------------

} // namespace Video

#endif // _VIDEO_H

//------------------------------------------------------------------------------
