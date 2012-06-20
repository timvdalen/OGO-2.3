/*
 * Movement module -- see header file for more info
 */

#include <string.h>

#include <limits>

#include "game.h"
#include "movement.h"

namespace Movement {

//------------------------------------------------------------------------------

Controller::Controller(Camera &C, ObjectHandle P) : camera(C), player(P)
{
	firstPerson = false;
	lastView = false;
	target = player->origin + Pd(-.5,-.5,2);

	Vector<double> vec = camAngle * Vector<double>(0,1,0);

	zoom = 10.0;
	camera.origin = target - (~vec * zoom);
	camera.lookAt(target);

	camAngle = Qd();
	blocked = false;
}

//------------------------------------------------------------------------------

void Controller::moveX(double speed)
{
	return;//You dont move in the x-direction you just turn
	/*
	speed *= MoveSpeed;

    Point<double> posrollback = Point<double>(player->origin);
	Point<double> tarrollback = Point<double>(target);
	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));
	double yaw = atan2(vec.x, vec.y);

	yaw += (.5 * Pi);

	target.x = target.x + speed * sin(yaw);
	target.y = target.y + speed * cos(yaw);
	player->origin = target - Pd(.75 * sin(yaw - .25*Pi), .75 * cos(yaw - .25*Pi), 2);
	
    if(!walkAble(posrollback, player->origin)){
        player->origin = posrollback;
		target = tarrollback;
    	return;
    }
	camera.origin.x = camera.origin.x + speed * sin(yaw);
	camera.origin.y = camera.origin.y + speed * cos(yaw);*/
}

//------------------------------------------------------------------------------

void Controller::moveY(double speed)
{
	speed *= MoveSpeed;
	speed *= FRATE;

    Point<double> posrollback = Point<double>(player->origin);
	Point<double> tarrollback = Point<double>(target);
	Vector<double> vec = (-player->rotation * Vector<double>(0,1,0));
	vec.z = 0;
	vec = ~vec;
	double yaw = atan2(vec.x, vec.y);
	player->origin = player->origin + vec*speed;
	target = player->origin + Pd(.75 * sin(yaw - .25*Pi), .75 * cos(yaw-.25*Pi), 2);
	
	if(!walkAble(posrollback, player->origin)){
		player->origin = posrollback;
    	target = tarrollback;
		blocked = true;
    	return;
	}
	vec = ~(camAngle * Vector<double>(0,1,0));
	if (firstPerson)
	{
		camera.origin = player->origin + Pd(0,0,2.5) + vec;
		camera.lookAt(camera.origin + (vec * 5.0));
	}
	else
	{
		camera.origin = target - (vec * zoom);
		camera.lookAt(target);
	}
	blocked = false;
}

//------------------------------------------------------------------------------

void Controller::moveZ(double speed)
{
	speed *= JetpackSpeed;
	speed *= FRATE;

	player->origin.z += speed;
	target.z = target.z + speed;
	camera.origin.z = camera.origin.z + speed;
}

//------------------------------------------------------------------------------

void Controller::lookX(double speed)
{
	speed *= -LookSpeed;
	speed *= FRATE;

	camAngle = Qd(Rd(speed, Vd(0,0,1))) * camAngle;

	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

	if (firstPerson == true)
	{
		camera.origin = player->origin + Pd(0,0,2.5) + vec;
		camera.lookAt(camera.origin + (vec * 5.0));
	}
	else
	{
		camera.origin = target - (vec * zoom);
		camera.lookAt(target);
	}
}

//------------------------------------------------------------------------------

void Controller::lookY(double speed)
{
	speed *= ZoomSpeed;
	speed *= FRATE;

	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

	zoom -= speed;

	if (zoom > 25.0)
	{
		zoom = 25.0;
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
		camera.origin = player->origin + Pd(0,0,2.5) + vec;
		camera.lookAt(camera.origin + (vec * 5.0));
	}
	else
	{
		camera.origin = target - (vec * zoom);
		camera.lookAt(target);
	}
}

//------------------------------------------------------------------------------

void Controller::lookZ(double speed)
{
	speed *= LookSpeed;
	speed *= FRATE;

    Qd buffer = camAngle;//Used to rollback if out of bounds

	Vector<double> mystery = ~(camAngle * Vector<double>(0,1,0));
	double mysteryYaw = atan2(mystery.x, mystery.y);

	camAngle = Qd(Rd(speed, Vd(cos(mysteryYaw),-sin(mysteryYaw),0))) * camAngle;
       if((camAngle*Vector<double>(0,1,0)).z > 0.99 || (camAngle*Vector<double>(0,1,0)).z < -0.99){
           camAngle = buffer;
           return;
       }
	Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

	if (firstPerson == true)
	{
		camera.origin = player->origin + Pd(0,0,2.5) + vec;
		camera.lookAt(camera.origin + (vec * 5.0));
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
	if (move[dirY] != 0.0) moveY(move[dirY]);
	if (move[dirZ] != 0.0) moveZ(move[dirZ]);
	
	if (look[dirX] != 0.0) lookX(look[dirX]);
	if (look[dirY] != 0.0) lookY(look[dirY]);
	if (look[dirZ] != 0.0) lookZ(look[dirZ]);
	
	if (move[dirY] != 0.0)
	{
		if ((move[dirX] < 0.0 && move[dirY] > 0.0)
		||  (move[dirX] > 0.0 && move[dirY] < 0.0))
			player->rotation = player->rotation * Rd(-0.05,Vd(0,0,1));
		else if ((move[dirX] > 0.0 && move[dirY] > 0.0)
		     ||  (move[dirX] < 0.0 && move[dirY] < 0.0))
			player->rotation = player->rotation * Rd(0.05,Vd(0,0,1));
		else
		{
            Vd camv = camAngle*Vd(0,1,0);
            Vd plav = player->rotation*Vd(0,1,0);
            camv.z = 0; camv = ~camv;
            plav.z = 0; plav = ~plav;
            double angle = atan2(camv.y - plav.y, camv.x - plav.x);
            double angle2 = (angle < 0? -angle : angle);
            double axis = angle2 >  0.5*Pi ^ angle > 0? 1 : -1;
            angle2 = angle2 > 0.5*Pi ? Pi - angle2 : angle2;
            angle2 = angle2 < 0.05? angle2 : 0.05;
			if (axis != 0)
                player->rotation = player->rotation * Rd(-angle2,Vd(0,0,axis));
		}
	}
	Vector<double> vec = (-player->rotation * Vector<double>(0,1,0));
	vec.z = 0;
	vec = ~vec;
	double yaw = atan2(vec.x, vec.y);
	target = player->origin + Pd(.75 * sin(yaw - .25*Pi), .75 * cos(yaw-.25*Pi), 2);
	vec = ~(camAngle * Vector<double>(0,1,0));
	if (firstPerson)
	{
		camera.lookAt(camera.origin + (vec * 5.0));
	}
	else
	{
		camera.lookAt(target);
	}
	Objects::Player * p = TO(Objects::Player,player);
	p->velocity = Vd(0,MoveSpeed,0);
}

//------------------------------------------------------------------------------

bool Controller::walkAble(Point<double> old, Point<double> updated)
{
    World *w = Game::game.world;
	double extrabounding = 0.5;
    if(!((-(w->width)/2 +extrabounding) < updated.x && updated.x < ((w->width)/2 - extrabounding)//Inside x-interval
        && (-(w->height)/2 +extrabounding) < updated.y && updated.y < ((w->height)/2) - extrabounding))//Inside y-interval
    {
        return false;
    }
    Terrain *t = w->terrain; 
    //check every terrain item, we return false if and only if updated is in a object
    //and old is not in that object
    map<GridPoint, ObjectHandle>::iterator it;
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
