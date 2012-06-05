#ifndef _H_HUD
#define _H_HUD

#include "core.h"
#include "video.h"

//! Contains HUD objects
namespace HUD_objects{

//! Main class for the Heads-up display
class HUD: public Object{
	public:

	//! Width of the screen in pixels
	int width;

	//! Height of the screen in pixels
	int height;

	//! Constructs the HUD with width _width and height _height. 
	HUD(int _width, int _height);

	//! Notifies the HUD that the screen size has changed
	void resize(int _width, int _height);

	//! Sets up 2D drawing mode
	void preRender();

	//! Switches back to 3D drawing mode
	void postRender();
};

}

#endif
