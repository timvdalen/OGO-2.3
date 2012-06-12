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
#include "world.h"

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
	ObjectHandle world;  //!< Needed to be able to check were we can walk
    
	bitset<dirLast> move; //!< Current comera movement
	bitset<dirLast> look; //!< Current camera target movement
	
	bool firstPerson;     //!< selects first person view instead of third person
	
	//! Construct controller by assigning a player and camera
	Controller(Camera &C, ObjectHandle P, ObjectHandle W);
	
	void moveX(double speed);
	void moveY(double speed);
	void moveZ(double speed);

	void lookX(double speed);
	void lookY(double speed);
	void lookZ(double speed);
	
	void frame();         //!< Called everytime a frame passes in game
    
	Point<double> target;

	private:
	Qd camAngle;
	double zoom;
	
    bool walkAble(Point<double> old, Point<double> p);
};

//------------------------------------------------------------------------------

} // namespace Movement

#endif // _MOVEMENT_H

//------------------------------------------------------------------------------
