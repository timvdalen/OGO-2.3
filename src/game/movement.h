/*
 * Movement module
 *
 * Date: 30-05-12 16:44
 *
 * Description:
 */

#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <map>

#include "objects.h"
#include "video.h"
#include "world.h"

//! Movement module
//! Takes care of camera movement
namespace Movement {

using namespace std;
using namespace Video;
using namespace Objects;

const double MoveSpeed = 0.3;
const double JetpackSpeed = 1.2;
const double LookSpeed = 0.0175;
const double ZoomSpeed = 0.3;

//------------------------------------------------------------------------------

//! Movement directions
enum Direction
{
	dirX,   
	dirY,
	dirZ,
	dirLast
};

//------------------------------------------------------------------------------

//! Camera controller
class Controller
{
	public:
	Camera &camera;             //!< Assigned camera
	ObjectHandle player;        //!< Assigned player
    
	map<Direction,double> move; //!< Current comera movement
	map<Direction,double> look; //!< Current camera target movement
	
	bool blocked;
	
	bool firstPerson;           //!< selects first person view instead of third person
	
	//! Construct controller by assigning a player and camera
	Controller(Camera &C, ObjectHandle P);
	
	void moveX(double speed); //!< Moves the player left and right
	void moveY(double speed); //!< Moves the player forwards and backwards
	void moveZ(double speed); //!< Moves the player up and down

	void lookX(double speed); //!< Rotates the camera left and right
	void lookY(double speed); //!< Zooms the camera in and out
	void lookZ(double speed); //!< Rotates the camera up and down
	
	void frame();             //!< Called everytime a frame passes in game
    
	Point<double> target;     //!< The point the camera looks at
	
	//! Sets the view
	void setView(bool view = true);
	
	//! Restores the view to the previous state
	void restoreView();

	private:
	//! The last view (first person or third person) that was active before the current one
	bool lastView;
	Qd camAngle;
	double zoom;
	
    bool walkAble(Point<double> old, Point<double> p);
};

//------------------------------------------------------------------------------

} // namespace Movement

#endif // _MOVEMENT_H

//------------------------------------------------------------------------------
