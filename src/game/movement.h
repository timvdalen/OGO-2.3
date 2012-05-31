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

//! Camera controller
class Controller
{
	public:
	Camera &camera;  //!< Assigned camera
	Player &player;  //!< Assigned player
	
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
	
	bitset<dirLast> move; //!< Current comera movement
	bitset<dirLast> look; //!< Current camera target movement
	
	//! Construct controller by assigning a player and camera
	Controller(Camera &C, Player &P);
	
	void frame();          //!< Called everytime a frame passes in game
	
	private:
	Point<double> pos;
	double zoom;
	bool fps;

	void moveX();
	void moveY();
	void moveZ();

	void lookX();
	void lookY();
	void lookZ();
};

//------------------------------------------------------------------------------

} // namespace Movement

#endif // _MOVEMENT_H

//------------------------------------------------------------------------------
