#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include <cstdlib>
#include <stdio.h>
#include <cmath>

#include "game.h"
#include "minion.h"
#include "player.h"
#include "world.h"
#include "video.h"

namespace Objects{

using namespace Game;

void Minion::moveTo(GridPoint p){
	waypoints.clear();
	//Dumb way for now
	GridPoint current;
	int i = 0;
	for(current = game.world->terrain->ToGrid(origin); current != p; i++){
		//Greedy find next point
		GridPoint difference = p-current;
		if(abs(difference.x) > abs(difference.y)){
			//Move into X direction
			if(difference.x > 0){
				//Move to the right
				current += GridPoint(1, 0);
			}else{
				//Move to the left
				current += GridPoint(-1, 0);
			}
		}else{
			//Move into Y direction
			if(difference.y > 0){
				//Move to the top
				current += GridPoint(0, 1);
			}else{
				//Move to the bottom
				current += GridPoint(0, -1);
			}
		}
		waypoints.push_back(current);
	}
}

void Minion::setNewTarget(){}

Minion::Minion(Pd origin, Qd rotation)
	: BoundedObject(origin, rotation)
{
	moveTo(GridPoint(0, 0));
}

void Minion::frame(){
	if(waypoints.size() == 0){
		setNewTarget();
		return;
	}
	
	const double movespeed = 0.25;
	GridPoint current = waypoints.front();
	if(current == game.world->terrain->ToGrid(origin)){
		waypoints.erase(waypoints.begin());
		setNewTarget();
	}else{
		//Start moving
		Pd target = game.world->terrain->ToPointD(current);
		Pd difference = target-origin;
		if(abs(difference.x) > abs(difference.y)){
			//printf("x > y\n");
			//Move into X direction
			if(difference.x > 0){
				//printf("move right\n");
				//Move to the right
				origin.x += movespeed;
			}else{
				//printf("move left\n");
				//Move to the left
				origin.x -= movespeed;
			}
		}else{
			//printf("y > x (%f, %f)\n", difference.x, difference.y);
			//Move into Y direction
			if(difference.y > 0){
				//printf("move up\n");
				//Move to the top
				origin.y += movespeed;
			}else{
				//printf("move down\n");
				//Move to the bottom
				origin.y -= movespeed;
			}
		}
	}
}

void Minion::render(){
	frame();
	Object::render();
}

void Minion::draw(){
	glTranslatef(0, 0, 0.5);
	glutSolidSphere(0.5, 15, 15);
}

ArenaGuard::ArenaGuard(ObjectHandle _world, Pd origin, Qd rotation)
	: Minion(origin, rotation), world(_world)
{}

void ArenaGuard::setNewTarget(){
	const double gw = (game.world->terrain->width/GRID_SIZE);
	const double gh = (game.world->terrain->height/GRID_SIZE);
	
	GridPoint curr = game.world->terrain->ToGrid(origin);
	
	if(curr == GridPoint(0, 0)){
		moveTo(GridPoint(gw, 0));
	}else if(curr == GridPoint(gw, 0)){
		moveTo(GridPoint(gw, gh+1));
	}else if(curr == GridPoint(gw, gh)){
		moveTo(GridPoint(0, gh));
	}else if(curr == GridPoint(0, gh)){
		moveTo(GridPoint(0, 0));
	}
	
	set<ObjectHandle>::iterator it;
	World *w = TO(World, world);
	if(w){
		for(it = w->children.begin(); it != w->children.end(); it++){
			Player *p = TO(Player, *it);
			if(p){
				Pd difference = p->origin - origin;
				if(abs(difference.x) < 10 && abs(difference.y) < 10){
					//TODO: Calculate quaternion
					w->addLaserBeam(ObjectHandle(LaserBeam(origin + Pd(0, 0, 1), Qd(1, 1, 1, 0))));
				}
			}
		}
	}
}

void DefenseMinion::setNewTarget(){
	moveTo(game.world->terrain->ToGrid(owner->origin));
	
	if(last != game.world->terrain->ToGrid(origin)){
		set<ObjectHandle>::iterator it;
		World *w = TO(World, world);
		if(w){
			for(it = w->children.begin(); it != w->children.end(); it++){
				Player *p = TO(Player, *it);
				Player *o = TO(Player, owner);
				if(p && o){
					Pd difference = p->origin - origin;
					if(abs(difference.x) < 10 && abs(difference.y) < 10 && p->team != o->team){
						//TODO: Calculate quaternion
						w->addLaserBeam(ObjectHandle(LaserBeam(origin + Pd(0, 0, 1), Qd(1, 1, 1, 0))));
					}
				}
			}
		}
	}
	last = game.world->terrain->ToGrid(origin);
}

DefenseMinion::DefenseMinion(ObjectHandle _owner, ObjectHandle _world, Pd origin, Qd rotation)
	: Minion(origin, rotation), owner(_owner), world(_world)
{
	last = game.world->terrain->ToGrid(origin);
}

}
