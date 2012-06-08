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
	#define WIN32_LEAN_AND_MEAN 1
	#include <windows.h>
	#include <gl\gl.h>
	#include <gl\glu.h>
	#include <gl\glext.h>
#elif defined __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <vector>

#include "base.h"
#include "core.h"

//!Video subsystem
namespace Video {

using namespace Core;

class Window;
class Viewport;
struct Camera;

//------------------------------------------------------------------------------

//! Initializes the video subsystem
void Initialize(int argc, char *argv[]);
//! Terminates the video subsystem
void Terminate();

//! Enters the event loop
//! \note Futher program flow is suspended from that point on.
void StartEventLoop();
//! Stops the event loop
void StopEventLoop();

//! Callback: a frame has passed
extern void(*OnFrame) ();

//! Gets the number of milisecconds passed since Initialize was called
dword ElapsedTime();

//------------------------------------------------------------------------------

//! A window instance
class Window
{
	public:
	bool resized;
	std::vector<Viewport *> viewports; //!< A collection of viewports the window shows
	
	//! Creates a new window width and height and window title and position
	//! \note set the window positions -1 for a default position
	Window(uword width, uword height, const char *title, bool fullscreen = true,
	       word xpos = -1, word ypos = -1);
	~Window();
	
	//! Selects the window
	void select();
	
	//! Renders the assigned viewports
	void render();
	
	friend void Initialize(int argc, char *argv[]);
	friend void StartEventLoop();
	friend void StopEventLoop();
	friend void Terminate();
	friend class Viewport;
	
	//! Gets/sets window size \note leave width and height zero to get
	void size(uword &width, uword &height);
	
	private:
	void *data;
	
	static std::set<Window *> windows;
	static void display();
	static void resize(int width, int height);
	static void timer(int);
};

//------------------------------------------------------------------------------

struct Camera
{
	Point<double> origin;         //!< focal point
	Quaternion<double> objective; //!< direction and focal length^W^W^W
	
	//! Rotates the camera so it looks at the specified point
	void lookAt(const Point<double> &);
};

//------------------------------------------------------------------------------

//! Viewport class, displays on a portion of a window
class Viewport
{
	public:
	Camera camera;      //!< Defines where the viewport view from in the world
	ObjectHandle world; //!< Defines the world the viewport will show
	
	//! create a new viewport
	//! \note the  width, height and position are factors relative to the the window sise
	Viewport(double width, double height, double xpos = 0, double ypos = 0,
	         double fov = 45.0);
	~Viewport();
	
	//! Changes viewport offset (relative to screen)
	void move(double xpos, double ypos);
	//! Resizes viewport
	void resize(double width, double height);
	//! Changes the field of view of the viewport
	void setfov(double fov);
	
	//! Actuates viewport
	void select(Window *);
	//! Draws screne viewed from camera in viewport
	void render();
	
	private:
	void *data;
};

//------------------------------------------------------------------------------

//! FPS counter
class FPS
{
	public:
	FPS() : frames(0), time(0), fps(0.0) {}
	
	//! Register new frame. \returns current fps
	double operator()();
	
	//! Type cast to numeric gives the current frames-per-second value
	operator double() { return fps; }
	
	private:
	int frames, time;
	double fps;
};

//------------------------------------------------------------------------------

} // namespace Video

#endif // _VIDEO_H

//------------------------------------------------------------------------------
