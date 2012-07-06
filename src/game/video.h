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

#define FRATE (Video::GameSpeed / Video::CurrentFPS())

//!Video subsystem
namespace Video {

using namespace Core;

class Window;
class Viewport;
struct Camera;

extern double GameSpeed;

//------------------------------------------------------------------------------

//! Initializes the video subsystem
void Initialize(int* argc, char *argv[]);
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
//! Gets the current number of frames per second
double CurrentFPS();

    
//------------------------------------------------------------------------------
//Culling methods
    
/*! Loads a viewing volume from an array, the array should contain 8 points
 *  The array is interpreted as follows:
 *  {left_bottom_near, right_bottom_near, right_upper_near, left_upper_near,
 *       left_bottom_far, right_bottom_far, right_upper_far, left_upper_far}
 *  In other words, we start at the leftbottom near point and continue counterclockwise
 *  and go to the far plane as soon as we reacht the left_bottom point again.
 *  The pointer p should not be deleted or changed, it will be deleted when clearViewVolume() is used.
 */
void loadViewVolume(Point<double> *p);
    
/*! loads an orthogonal viewing volume
 *  - left, right, bottom, top and depth are defined similar to glOrtho and intuitively defined
 *  - oversizing scales the viewing volume in all directions, this is in particular
 *    useful when using the model objects to apply effects like shadows, in
 *    which objects outside the viewingvolume can still produces shadows
 */
void loadOrthogonalVolume(double left, double right, double bottom, double top, double depth, double overSizing = 0.1);

/*! loads an orthogonal viewing volume
 *  - fovy, aspect, depth are defined as in gluPerspective:
 *      Therefore, fovy is the viewing angle in the vertical direction!
 *  - oversizing scales the viewing volume in all directions, this is in particular
 *    useful when using the model objects to apply effects like shadows, in
 *    which objects outside the viewingvolume can still produces shadows
 */
void loadPerspectiveVolume(double fovy, double aspect, double depth, double overSizing = 0.1);

//! clears the viewing volume
void clearViewVolume();

/*! Tries to determine if the boundingbox is inside the viewing volume
 * - The boundingbox is transformed using the modelviewmatrix
 * - The function is not exact:
 *   - a true-value implies the boundingbox is outside the viewing-volume;
 *   - a false-value does not imply the boundingbox is not outside the viewing-volume
 *     \note This is due to performance reasons.
 */
bool outsideViewingVolume(BoundingBox b);

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
	
	friend void Initialize(int *argc, char *argv[]);
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
