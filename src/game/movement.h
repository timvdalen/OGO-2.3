/*
 * Movement module
 *
 * Date: 30-05-12 16:44
 *
 * Description:
 */

#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <bitset>

#include "objects.h"
#include "video.h"

//! Movement module
//! Takes care of camera movement
namespace Movement {

using namespace std;
using namespace Video;
using namespace Objects;

//------------------------------------------------------------------------------

//! Movement directions
enum Direction
{
	dirUp,   
	dirDown,
	dirLeft,
	dirRight,
	dirBackward,
	dirForward,
	dirLast
};

//------------------------------------------------------------------------------

//! Camera controller
class Controller
{
	public:
	Camera &camera;      //!< Assigned camera
	ObjectHandle player; //!< Assigned player
	double width;          //!< Size of the grid in the x direction
    double height;          //!< Size of the grid in the y direction
    
	bitset<dirLast> move; //!< Current comera movement
	bitset<dirLast> look; //!< Current camera target movement
	
	//! Construct controller by assigning a player and camera
	Controller(Camera &C, ObjectHandle P, double width, double height);
	
	void frame();         //!< Called everytime a frame passes in game
	
	//! Sets to view to first or third person
	void setView(bool fp);
	
	//! Gets the view
	bool getView();
	
	private:
	Point<double> pos;
	Qd camAngle;
	double zoom;
	bool fps;

	void moveX();
	void moveY();
	void moveZ();

	void lookX();
	void lookY();
	void lookZ();
    bool insideBounds(Point<double> p);
};

//------------------------------------------------------------------------------

} // namespace Movement

#endif // _MOVEMENT_H

//------------------------------------------------------------------------------
