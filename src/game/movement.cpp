/*
 * Movement module -- see header file for more info
 */

#include <string.h>

#include <limits>

#include "movement.h"

namespace Movement {

	double movespeedmultiplier = 0.5;
	double jetpackspeedmultiplier = 0.2;
	double lookspeedmultiplier = 0.035;
	double zoomspeedmultiplier = 0.5;

	Point<double> offset = Pd(.5,.5,2);

//------------------------------------------------------------------------------

Controller::Controller(Camera &C, ObjectHandle P, ObjectHandle W) : camera(C), player(P), world(W)
{
	firstPerson = false;
	lastView = false;
	target = player->origin + offset;

	Vector<double> vec = camAngle * Vector<double>(0,1,0);

	zoom = 10.0;
	camera.origin = target - (~vec * zoom);
	camera.lookAt(target);

	camAngle = Qd();
}

//------------------------------------------------------------------------------

void Controller::moveX(double movespeed)
{
	movespeed = movespeedmultiplier * movespeed;

    Point<double> posrollback = Point<double>(target);

	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));
	double yaw = atan2(vec.x, vec.y);

	yaw += (.5 * Pi);

	target.x = target.x + movespeed * sin(yaw);
	target.y = target.y + movespeed * cos(yaw);
       if(!walkAble(posrollback, target)){
           target = posrollback;
           return;
       }
	camera.origin.x = camera.origin.x + movespeed * sin(yaw);
	camera.origin.y = camera.origin.y + movespeed * cos(yaw);
        
	//test
	player->origin = target - Pd(.75 * sin(yaw + .25*Pi), .75 * cos(yaw + .25*Pi), 2);
}

//------------------------------------------------------------------------------

void Controller::moveY(double movespeed)
{
	movespeed = movespeedmultiplier * movespeed;

    Point<double> posrollback = Point<double>(target);

	Vector<double> vec = ~(-player->rotation * Vector<double>(0,1,0));
	double yaw = atan2(vec.x, vec.y);

	target.x = target.x + movespeed * sin(yaw);
	target.y = target.y + movespeed * cos(yaw);
        
       if(!walkAble(posrollback, target)){
           target = posrollback;
           return;
       }
	camera.origin.x = camera.origin.x + movespeed * sin(yaw);
	camera.origin.y = camera.origin.y + movespeed * cos(yaw);
	//test
	player->origin = target - Pd(.75 * sin(yaw + .25*Pi), .75 * cos(yaw + .25*Pi), 2);
}

//------------------------------------------------------------------------------

void Controller::moveZ(double jetpackspeed)
{
	jetpackspeed = jetpackspeedmultiplier * jetpackspeed;

	Vector<double> vec = ~(-player->rotation * Vector<double>(0,-1,0));
	double yaw = atan2(vec.x, vec.y);

	target.z = target.z + jetpackspeed;
	camera.origin.z = camera.origin.z + jetpackspeed;

	//test
	player->origin = target - Pd(-.75 * sin(yaw + .25*Pi), -.75 * cos(yaw + .25*Pi), 2);
}

//------------------------------------------------------------------------------

void Controller::lookX(double lookspeed)
{
	lookspeed = lookspeedmultiplier * -lookspeed;

	camAngle = Qd(Rd(lookspeed, Vd(0,0,1))) * camAngle;

	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

	if (firstPerson == true)
	{
		camera.origin = player->origin + Pd(0,0,2) + vec;
		camera.lookAt(target + (vec * 5.0));
	}
	else
	{
		camera.origin = target - (vec * zoom);
		camera.lookAt(target);
	}
}

//------------------------------------------------------------------------------

void Controller::lookY(double zoomspeed)
{
	zoomspeed = zoomspeedmultiplier * zoomspeed;

	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

	zoom -= zoomspeed;

	if (zoom > 15.0)
	{
		zoom = 15.0;
	}
	else if (zoom <= 5.0)
	{
		firstPerson = true;
		zoom = 5.0;
	}
	else
	{
		firstPerson = false;
	}

	if (firstPerson == true)
	{
		camera.origin = player->origin + Pd(0,0,2) + vec;
		camera.lookAt(target + (vec * 5.0));
	}
	else
	{
		camera.origin = target - (vec * zoom);
		camera.lookAt(target);
	}
}

//------------------------------------------------------------------------------

void Controller::lookZ(double lookspeed)
{
	lookspeed = lookspeedmultiplier * lookspeed;

    Qd buffer = camAngle;//Used to rollback if out of bounds

	Vector<double> mystery = ~(camAngle * Vector<double>(0,1,0));
	double mysteryYaw = atan2(mystery.x, mystery.y);

	camAngle = Qd(Rd(lookspeed, Vd(cos(mysteryYaw),-sin(mysteryYaw),0))) * camAngle;
       if((camAngle*Vector<double>(0,1,0)).z > 0.99 || (camAngle*Vector<double>(0,1,0)).z < -0.99){
           camAngle = buffer;
           return;
       }
	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

	if (firstPerson == true)
	{
		camera.origin = player->origin + Pd(0,0,2) + vec;
		camera.lookAt(target + (vec * 5.0));
	}
	else
	{
		camera.origin = target - (vec * zoom);
		camera.lookAt(target);
	}
}

//------------------------------------------------------------------------------
    
void Controller::frame()
{
	//if (move[dirX] != 0.0) moveX(move[dirX]);
	if (move[dirY] != 0.0) moveY(move[dirY]);
	if (move[dirZ] != 0.0) moveZ(move[dirZ]);
	
	if (look[dirX] != 0.0) lookX(look[dirX]);
	if (look[dirY] != 0.0) lookY(look[dirY]);
	if (look[dirZ] != 0.0) lookZ(look[dirZ]);
	
	if (move[dirY] != 0.0)
	{
		if (move[dirX] < 0.0) player->rotation = player->rotation * Rd(-0.05,Vd(0,0,1));
		else if (move[dirX] > 0.0) player->rotation = player->rotation * Rd(0.05,Vd(0,0,1));
		else {
            Vd camv = camAngle*Vd(0,1,0);
            Vd plav = player->rotation*Vd(0,1,0);
            camv.z = 0; camv = ~camv;
            plav.z = 0; plav = ~plav;
            double angle = atan2(camv.y - plav.y, camv.x - plav.x);
            double angle2 = (angle < 0? -angle : angle);
            double axis = angle2 >  0.5*Pi ^ angle > 0? 1 : -1;
            angle2 = angle2 > 0.5*Pi ? Pi - angle2 : angle2;
            angle2 = angle2 < 0.05? angle2 : 0.05;
			if (axis != 0) {
                player->rotation = player->rotation * Rd(-angle2,Vd(0,0,axis));
            }
		}
	}
	
	Objects::Player * p = TO(Objects::Player,player);
	p->velocity = Vd(0,movespeedmultiplier,0);
}

//------------------------------------------------------------------------------

bool Controller::walkAble(Point<double> old, Point<double> updated){
    World *w = TO(World, world);
	double extrabounding = 0.1*GRID_SIZE;
    if(!((-(w->width)/2 +extrabounding) < updated.x && updated.x < ((w->width)/2 - extrabounding)//Inside x-interval
        && (-(w->height)/2 +extrabounding) < updated.y && updated.y < ((w->height)/2) - extrabounding))//Inside y-interval
    {
        return false;
    }
    Terrain *t = w->terrain; 
    //check every terrain item, we return false if and only if updated is in a object
    //and old is not in that object
    multimap<GridPoint, ObjectHandle>::iterator it;
	for(it = t->structures.begin(); it != t->structures.end(); it++){
		GridPoint p = it->first;
        double worldx = GRID_SIZE*p.x - (w->width)/2;
        double worldy = GRID_SIZE*p.y - (w->height)/2;
		//Process them
        //TODO:Maybe use bounding boxes? Should be straightforward to implement this
        if(   (worldx -extrabounding) < updated.x && updated.x < (worldx + GRID_SIZE + extrabounding)
           && (worldy - extrabounding) < updated.y && updated.y < (worldy + GRID_SIZE +extrabounding)//update in bounds
           &&!(   (worldx - extrabounding) < old.x && old.x < (worldx + GRID_SIZE +extrabounding)
               && (worldy -extrabounding) < old.y && old.y < (worldy + GRID_SIZE +extrabounding) )       //old not in bounds
           ){
            return false;
        }
	}
    
    return true;
}
    
//------------------------------------------------------------------------------

void Controller::setView(bool view){
	lastView = firstPerson;
	firstPerson = view;
}

//------------------------------------------------------------------------------

void Controller::restoreView(){
	firstPerson = lastView;
}

//------------------------------------------------------------------------------

} // namespace Movement

//------------------------------------------------------------------------------
