/*
 * World object -- see header file for more info
 */

#include <stdlib.h>
#include <stdio.h>

#include <limits>
#include <vector>

#include "world.h"

namespace Objects {

#define HIGH 100

class Sky;
class Star;

//------------------------------------------------------------------------------

World::World(double _width, double _height)
	: BoundedObject(Pd(), Qd(),
	  BoundingBox(Pd(-_width/2, -_height/2, 0), Pd(_width,0,0), Pd(0,_height,0), Pd(_width,_height,0), Pd(), Pd(), Pd(), Pd(_width/2, _height/2, HIGH)),
	  Assets::WorldMaterial)
{

	ObjectHandle tHandle;
	tHandle = Terrain(_width, _height);
	terrain = TO(Terrain, tHandle);
	children.insert(tHandle);

	ObjectHandle hudHandle;
	hudHandle = HUD(640, 480);
	hud = TO(HUD, hudHandle);
	children.insert(hudHandle);

	children.insert(Sky((int) _width, (int) _height));

	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void World::draw(){
	double halfWidth = width/2;
	double halfHeight = height/2;

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2,2);
	glBegin(GL_QUADS);
		//Back side
		glNormal3i(0, 1, 0);
		glTexCoord2f(0.33, 0);
		glVertex3f(-halfWidth, -halfHeight, 0.0f);
		glTexCoord2f(0.33, 0.33);
		glVertex3f(-halfWidth, -halfHeight, HIGH);
		glTexCoord2f(0.66, 0.33);
		glVertex3f(halfWidth, -halfHeight, HIGH);
		glTexCoord2f(0.66, 0);
		glVertex3f(halfWidth, -halfHeight, 0.0f);

		//Left side
		glNormal3i(1, 0, 0);
		glTexCoord2f(0, 0.66);
		glVertex3f(-halfWidth, halfHeight, 0.0f);
		glTexCoord2f(0.33, 0.66);
		glVertex3f(-halfWidth, halfHeight, HIGH);
		glTexCoord2f(0.33, 0.33);
		glVertex3f(-halfWidth, -halfHeight, HIGH);
		glTexCoord2f(0, 0.33);
		glVertex3f(-halfWidth, -halfHeight, 0.0f);

		//Right side
		glNormal3i(-1, 0, 0);
		glTexCoord2d(1, 0.33);
		glVertex3f(halfWidth, -halfHeight, 0.0f);
		glTexCoord2d(0.66, 0.33);
		glVertex3f(halfWidth, -halfHeight, HIGH);
		glTexCoord2d(0.66, 0.66);
		glVertex3f(halfWidth, halfHeight, HIGH);
		glTexCoord2d(1, 0.66);
		glVertex3f(halfWidth, halfHeight, 0.0f);

		//Front side
		glNormal3i(0, -1, 0);
		glTexCoord2d(0.66, 1);
		glVertex3f(halfWidth, halfHeight, 0.0f);
		glTexCoord2d(0.66, 0.66);
		glVertex3f(halfWidth, halfHeight, HIGH);
		glTexCoord2d(0.33, 0.66);
		glVertex3f(-halfWidth, halfHeight, HIGH);
		glTexCoord2d(0.33, 1);
		glVertex3f(-halfWidth, halfHeight, 0.0f);

		//Top side
		glNormal3i(0, 0, -1);
		glTexCoord2d(0.66, 0.66);
		glVertex3f(halfWidth, halfHeight, HIGH);
		glTexCoord2d(0.66, 0.33);
		glVertex3f(halfWidth, -halfHeight, HIGH);
		glTexCoord2d(0.33, 0.33);
		glVertex3f(-halfWidth, -halfHeight, HIGH);
		glTexCoord2d(0.33, 0.66);
		glVertex3f(-halfWidth, halfHeight, HIGH);
	glEnd();
	glPolygonOffset(0,0);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

//------------------------------------------------------------------------------

void World::postRender(){
	//Check for the ttl of all active LaserBeams and render them
	vector<ObjectHandle>::iterator it;
	for(it = temporary.begin(); it != temporary.end(); ){
		LaserBeam *lcurr = TO(LaserBeam, *it);
		
		if(!lcurr){
			//This situation can only happen when an ObjectHandle is manually
			//added to temporary
			Droppable *dcurr = TO(Droppable, *it);
			if(!dcurr){
				it = temporary.erase(it);
			}else{
				if(dcurr->done){
					it = temporary.erase(it);
				}else{
					dcurr->render();
					it++;
				}

			}
		}else{
			if(lcurr->done){
				it = temporary.erase(it);
			}else{
				lcurr->render();
				it++;
			}
		}
	}

	//Go on to render children and pop matrix
	Object::postRender();
}

//------------------------------------------------------------------------------

Point<double> World::getCorrectedOrigin(Qd q, Pd p){
    #define LOWERBOUND 0.25
	#define ZLEVEL 1.2
	#define ZLEVEL2 3.5
	#define INTRNBND GRID_SIZE/5.0
	#define newp (p + (v*lambda))
	Vd v = q*Vd(0,1,0);
	double lambda = 0;
	//Correctoutside
    if(p.z < LOWERBOUND && v.z != 0){
		lambda = (-(p.z-LOWERBOUND)/v.z);
	}
	if(p.x < -width/2 + LOWERBOUND && v.x != 0){
		lambda = max(lambda, (-(width/2 + p.x- LOWERBOUND)/v.x));
	}
	if(p.x > width/2 - LOWERBOUND && v.x != 0){
		lambda = max(lambda, (-(p.x + LOWERBOUND -width/2)/v.x));
	}
	if(p.y < -height/2 + LOWERBOUND && v.y != 0){
		lambda = max(lambda, (-(height/2 + p.y- LOWERBOUND)/v.y));
	}
	if(p.y > height/2 - LOWERBOUND && v.y != 0){
		lambda = max(lambda, (-(p.y + LOWERBOUND -height/2)/v.y));
	}
	map<GridPoint, ObjectHandle> *structs = &terrain->structures;
	map<GridPoint, ObjectHandle>::iterator itt;
	if(p.z < ZLEVEL2){
		for(itt = structs->begin(); itt != structs->end(); itt++){
			GridPoint gp = itt->first;
	        double worldx = GRID_SIZE*gp.x - (width)/2;
    	    double worldy = GRID_SIZE*gp.y - (height)/2;
			double sigma = 100000.0;
			double buff;
			if((worldx + INTRNBND -LOWERBOUND) < p.x && p.x < (worldx + GRID_SIZE + LOWERBOUND -  INTRNBND)
          		 && (worldy + INTRNBND- LOWERBOUND) < p.y && p.y < (worldy - INTRNBND + GRID_SIZE + LOWERBOUND)){
				buff = (-(p.z-ZLEVEL2)/v.z);
				if(0 < buff && buff < sigma){
					sigma = buff;
				}
				buff = (-(p.x-worldx - INTRNBND+ LOWERBOUND)/v.x);
				if(0 < buff && buff < sigma){
					sigma = buff;
				}
				buff = (-(p.x-worldx + INTRNBND -GRID_SIZE- LOWERBOUND)/v.x);
				if(0 < buff && buff < sigma){
					sigma = buff;
				}
				buff = (-(p.y-worldy- INTRNBND + LOWERBOUND)/v.y);
				if(0 < buff && buff < sigma){
					sigma = buff;
				}
				buff = (-(p.y-worldy+INTRNBND -GRID_SIZE-LOWERBOUND)/v.y);
				if(0 < buff && buff < sigma){
					sigma = buff;
				}
				lambda = max(lambda,sigma);
			}
			sigma = 1000000;
			if(p.z < ZLEVEL || newp.z < ZLEVEL){
				if(((worldx - LOWERBOUND) < p.x && p.x < (worldx + GRID_SIZE + LOWERBOUND)
          		 && (worldy - LOWERBOUND) < p.y && p.y < (worldy + GRID_SIZE + LOWERBOUND))
				 || 
				 ((worldx - LOWERBOUND) < newp.x && newp.x < (worldx + GRID_SIZE + LOWERBOUND)
          		 && (worldy - LOWERBOUND) < newp.y && newp.y < (worldy + GRID_SIZE + LOWERBOUND))){
					buff = (-(p.z-ZLEVEL)/v.z);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.x-worldx + LOWERBOUND)/v.x);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.x-worldx-GRID_SIZE- LOWERBOUND)/v.x);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.y-worldy+LOWERBOUND)/v.y);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.y-worldy-GRID_SIZE-LOWERBOUND)/v.y);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					if(sigma != 1000000) lambda = max(lambda,sigma);
				sigma = 100000.0;
				if((worldx + INTRNBND -LOWERBOUND) < newp.x && newp.x < (worldx + GRID_SIZE + LOWERBOUND -  INTRNBND)
          			 && (worldy + INTRNBND- LOWERBOUND) < newp.y && newp.y < (worldy - INTRNBND + GRID_SIZE + LOWERBOUND)){
					buff = (-(p.z-ZLEVEL2)/v.z);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.x-worldx - INTRNBND+ LOWERBOUND)/v.x);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.x-worldx + INTRNBND -GRID_SIZE- LOWERBOUND)/v.x);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.y-worldy- INTRNBND + LOWERBOUND)/v.y);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					buff = (-(p.y-worldy+INTRNBND -GRID_SIZE-LOWERBOUND)/v.y);
					if(lambda < buff && buff < sigma){
						sigma = buff;
					}
					if(sigma != 100000.0) lambda = max(lambda,sigma);
					}
				}
			}

		}
	}
	p = p + (v*lambda);
	return p;
	#undef newp
	#undef INTRNBND
	#undef ZLEVEL
	#undef ZLEVEL2
	#undef LOWERBOUND
}

//------------------------------------------------------------------------------

ObjectHandle World::trace(Point<double> origin, Vector<double> &path)
{
	pair<ObjectHandle, double> closest = make_pair(ObjectHandle(), !path);
	BoundedObject *bo;
	pair<ObjectHandle, double> ret;
	for (Object::iterator it = begin(); it != end(); ++it)
	{
		if (!(bo = TO(BoundedObject,*it))) continue;
		if((!TO(Player, *it)) && (!TO(Terrain, *it))) continue;
		ret = bo->checkCollision(origin, ~path);
		if (ret.second < closest.second)
			closest = ret;
	}
	path = ~path * closest.second;
	return closest.first;
}

//------------------------------------------------------------------------------

set<ObjectHandle> World::sense(ObjectHandle &target)
{
	set<ObjectHandle> objects;
	
	Object::iterator it;
	BoundedObject *bo = TO(BoundedObject,target);
	if (!bo) for (it = begin(); it != end(); ++it)
	{
		BoundedObject *bo = TO(BoundedObject,*it);
		if (bo && bo->checkCollision(target))
			objects.insert(*it);
	}
	else  for (it = begin(); it != end(); ++it)
		if (bo->checkCollision(*it));
	
	return objects;
}

//------------------------------------------------------------------------------

void World::addLaserBeam(ObjectHandle laserBeam){
	LaserBeam *lb = TO(LaserBeam, laserBeam);
	//Check if the ObjectHandle passed actually points to a LaserBeam
	if(lb){
		temporary.push_back(laserBeam);
	}
}

//------------------------------------------------------------------------------

Droppable::Droppable(Pd _origin, Resource _worth, int _dropped, int _ttl)
	: BoundedObject(_origin, Qd(), BoundingBox(Pd(-0.105, -0.5, 0.0), Pd(0.105, 0.5, 1.0)))
{
	model.coin = ModelObjectContainer();
	model.coin->children.insert(Assets::Model::CoinObj);
	children.insert(model.coin);
	model.coin->material = Assets::Model::CoinTex;

	worth = _worth;
	dropped = _dropped;
	ttl = _ttl;
	done = false;
}

//------------------------------------------------------------------------------

void Droppable::preRender(){
	Object::preRender();
	int timelived = Video::ElapsedTime() - dropped;
	if(timelived >= ttl){
		done = true;
		return;
	}
	rotation = rotation * Rd(0.02, Vd(0,0,1));
}

//------------------------------------------------------------------------------

Star::Star(Pd P)
	: Object(P, Qd(), Assets::Star)
{
	big = false;
}

//------------------------------------------------------------------------------

void Star::draw(){
	glBegin(GL_POINTS);
		glVertex3i(0, 0, 0);
		if(big){
			glVertex3f(-0.1, 0, 0);
			glVertex3f(0, -0.1, 0);
			glVertex3f(0.1, 0, 0);
			glVertex3f(0, 0.1, 0);
		}
	glEnd();
}

//------------------------------------------------------------------------------

Sky::Sky(int _width, int _height){
	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void Sky::frame(){
	if(stars.size() < 300){
		int res = rand()%100;
		if(res == 0){
			int x = rand()%width - (int)width/2;
			int y = rand()%height - (int)height/2;
			int min = HIGH - (int)(HIGH/10);
			int z = rand()%(HIGH-min) + min;
			stars.push_back(Star(Pd(x, y, z)));
		}
	}
	if(stars.size() > 150){
		int res = rand()%200;
		if(res == 0){
			stars.erase(stars.begin() + (rand()%stars.size()));
		}
	}
	if(stars.size() > 100){
		int res = rand()%60;
		if(res == 0){
			vector<Star>::iterator it;
			it = stars.begin() + (rand()%stars.size());
			it->big = !it->big;
		}
	}
}

//------------------------------------------------------------------------------

void Sky::render(){
	frame();
	vector<Star>::iterator it;
	for(it = stars.begin(); it != stars.end(); it++){
		(*it).render();
	}
}

//------------------------------------------------------------------------------

#undef HIGH

} // namespace Objects

//------------------------------------------------------------------------------
