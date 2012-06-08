/*
 * Movement module -- see header file for more info
 */

#include "movement.h"
#include <iostream>
namespace Movement {

	double movespeed = 0.5;
	double jetpackspeed = 0.2;
	double lookspeed = 0.035;
	double zoomspeed = 0.5;

//------------------------------------------------------------------------------

Controller::Controller(Camera &C, ObjectHandle P, ObjectHandle W) : camera(C), player(P), world(W)
{
	firstPerson = false;
	pos = player->origin;

	Vector<double> vec = camAngle * Vector<double>(0,1,0);

	zoom = 10.0;
	camera.origin = pos - (~vec * zoom);
	camera.lookAt(pos);

	camAngle = Qd();
}

//------------------------------------------------------------------------------

void Controller::moveX()
{
    Point<double> posrollback = Point<double>(pos);
	if (move[dirLeft])
	{
		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));
		double yaw = atan2(vec.x, vec.y);

		yaw -= (.5 * Pi);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
        if(!walkAble(posrollback, pos)){
            pos = posrollback;
            return;
        }
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);
        
		//test
		player->origin = pos;
	}
	else if (move[dirRight])
	{
		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));
		double yaw = atan2(vec.x, vec.y);

		yaw += (.5 * Pi);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
        if(!walkAble(posrollback, pos)){
            pos = posrollback;
            return;
        }
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::moveY()
{
    Point<double> posrollback = Point<double>(pos);
	if (move[dirForward])
	{
		Vector<double> vec = ~(-player->rotation * Vector<double>(0,1,0));
		double yaw = atan2(vec.x, vec.y);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
        
        if(!walkAble(posrollback, pos)){
            pos = posrollback;
            return;
        }
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);
		//test
		player->origin = pos;
	}
	else if (move[dirBackward])
	{
		Vector<double> vec = ~(-player->rotation * Vector<double>(0,-1,0));
		double yaw = atan2(vec.x, vec.y);

		pos.x = pos.x + movespeed * sin(yaw);
		pos.y = pos.y + movespeed * cos(yaw);
        if(!walkAble(posrollback, pos)){
            pos = posrollback;
            return;
        }
		camera.origin.x = camera.origin.x + movespeed * sin(yaw);
		camera.origin.y = camera.origin.y + movespeed * cos(yaw);

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::moveZ()
{
	if (move[dirUp])
	{
		pos.z = pos.z + jetpackspeed;
		camera.origin.z = camera.origin.z + jetpackspeed;

		//test
		player->origin = pos;
	}
	else if (move[dirDown])
	{
		pos.z = pos.z - jetpackspeed;
		camera.origin.z = camera.origin.z - jetpackspeed;

		//test
		player->origin = pos;
	}
}

//------------------------------------------------------------------------------

void Controller::lookX()
{
	if (look[dirLeft])
	{
		camAngle = Qd(Rd(lookspeed, Vd(0,0,1))) * camAngle;

		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

		if (firstPerson == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirRight])
	{
		camAngle = Qd(Rd(-lookspeed, Vd(0,0,1))) * camAngle;

		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

		if (firstPerson == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::lookY()
{
	if (look[dirForward]) // Zoom in
	{
		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

		if (zoom > 5.0)
		{
			zoom -= zoomspeed;
		}
		else if (zoom <= 5.0)
		{
			firstPerson = true;
		}

		if (firstPerson == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirBackward]) // Zoom out
	{
		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

		if (zoom < 15.0) 
		{
			zoom += zoomspeed;
		}
		
		if (firstPerson == true)
		{
			firstPerson = false;
			zoom = 5.0;
		}

		if (firstPerson == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------

void Controller::lookZ()
{
    Qd buffer = camAngle;//Used to rollback if out of bounds
	if (look[dirUp])
	{
		Vector<double> mystery = ~(camAngle * Vector<double>(0,1,0));
		double mysteryYaw = atan2(mystery.x, mystery.y);
		camAngle = Qd(Rd(lookspeed, Vd(cos(mysteryYaw),-sin(mysteryYaw),0))) * camAngle;
        if((camAngle*Vector<double>(0,1,0)).z > 0.99){
            camAngle = buffer;
            return;
        }
		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

		if (firstPerson == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
	else if (look[dirDown])
	{
		Vector<double> mystery = ~(camAngle * Vector<double>(0,1,0));
		double mysteryYaw = atan2(mystery.x, mystery.y);

		camAngle = Qd(Rd(-lookspeed, Vd(cos(mysteryYaw),-sin(mysteryYaw),0))) * camAngle;
        if((camAngle*Vector<double>(0,1,0)).z < -0.99){
            camAngle = buffer;
            return;
        }
		Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));

		if (firstPerson == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
	}
}

//------------------------------------------------------------------------------
    
void Controller::frame()
{
	/*if (move[dirLeft] || move[dirRight])
	{
		moveX();
	}*/
	if (move[dirForward] || move[dirBackward])
	{
		moveY();
		if (move[dirLeft]) player->rotation = player->rotation * Rd(-0.05,Vd(0,0,1));
		else if (move[dirRight]) player->rotation = player->rotation * Rd(0.05,Vd(0,0,1));
		else {
			double axis = Rd(player->rotation/camAngle).v.z;
            Vd camv = camAngle*Vd(0,1,0);
            Vd plav = player->rotation*Vd(0,1,0);
            camv.z = 0; camv = ~camv;
            plav.z = 0; plav = ~plav;
            double angle = atan2(camv.y - plav.y, camv.x - plav.x);
            double angle2 = (angle < 0? -angle : angle);
            angle2 = angle2 > 0.5*Pi ? Pi - angle2 : angle2;
            angle2 = angle2 < 0.05? angle2 : 0.05;
			if (axis != 0) {
                player->rotation = player->rotation * Rd(-angle2,Vd(0,0,axis));
            }
		}
	}

	if (move[dirUp] || move[dirDown])
	{
		moveZ();
	}


	if (look[dirLeft] || look[dirRight])
	{
		lookX();
	}

	if (look[dirForward] || look[dirBackward])
	{
		lookY();
	}

	if (look[dirUp] || look[dirDown])
	{
		lookZ();
	}

	
Objects::Player * p = dynamic_cast<Objects::Player*>(&*player);
	p->velocity = Vd(0,movespeed,0);
}

//------------------------------------------------------------------------------

bool Controller::walkAble(Point<double> old, Point<double> updated){
    World *w = TO(World, world);
    if(!(-(w->width)/2 < updated.x && updated.x < (w->width)/2//Inside x-interval
        && -(w->height)/2 < updated.y && updated.y < (w->height)/2))//Inside y-interval
    {
        return false;
    }
    Terrain *t = w->terrain; 
    //check every terrain item, we return false if and only if updated is in a object
    //and old is not in that object
    multimap<GridPoint, StructureHandle>::iterator it;
	for(it = t->structures.begin(); it != t->structures.end(); it++){
		GridPoint p = it->first;
        double worldx = GRID_SIZE*p.x - (w->width)/2;
        double worldy = GRID_SIZE*p.y - (w->height)/2;
		//Process them
        //TODO:Maybe use bounding boxes? Should be straightforward to implement this
        if(   worldx < updated.x && updated.x < worldx + GRID_SIZE
           && worldy < updated.y && updated.y < worldy + GRID_SIZE//update in bounds
           &&!(   worldx < old.x && old.x < worldx + GRID_SIZE
               && worldy < old.y && old.y < worldy + GRID_SIZE )       //old not in bounds
           ){
            return false;
        }
	}
    
    return true;
}

//------------------------------------------------------------------------------

void Controller::avoidPulverizebyBuilding(){
    World *w = TO(World, world); 
    Terrain *t = w->terrain; 
    int xlength = w->width/GRID_SIZE;
    int ylength = w->height/GRID_SIZE;
    bool containsBuilding[xlength][ylength];
    memset(containsBuilding, 0, sizeof containsBuilding);
    //create a mapping with all used grids
    multimap<GridPoint, StructureHandle>::iterator it;
    for(it = t->structures.begin(); it != t->structures.end(); it++){
        GridPoint p = it->first;
        containsBuilding[p.x][p.y] = true;
    }
    //O(n^2) could be faster, alot faster, pruning could be applied extensively
    int gridx = (pos.x + w->width/2)/GRID_SIZE;
    int gridy = (pos.y + w->height/2)/GRID_SIZE;
    if(containsBuilding[gridx][gridy]){
        double min_distance = std::numeric_limits<double>::infinity();
        int i, j;
        for(i = 0; i < xlength; i++){
            for(j = 0; j < ylength; j++){
                if(!containsBuilding[i][j]){
                    double x = GRID_SIZE*(i+0.5) - (w->width)/2;
                    double y = GRID_SIZE*(j+0.5) - (w->height)/2;
                    double d = (pos.x - x)*(pos.x - x) + (pos.y - y)*(pos.y - y);
                    if(d < min_distance){
                        min_distance = d;
                        gridx = i;
                        gridy = j;
                    }
               }
            }
        }
        pos.x = GRID_SIZE*gridx - (w->width)/2 + GRID_SIZE*0.5;;
        pos.y = GRID_SIZE*gridy - (w->height)/2 + GRID_SIZE*0.5;
        player->origin = pos;
        Vector<double> vec = ~(camAngle * Vector<double>(0,1,0));
        if (fps == true)
		{
			camera.origin = player->origin;
			camera.lookAt(pos + (vec * 5.0));
		}
		else
		{
			camera.origin = pos - (vec * zoom);
			camera.lookAt(pos);
		}
    }
}

    
//------------------------------------------------------------------------------
} // namespace Movement

//------------------------------------------------------------------------------
