/*
 * Structure objects -- see header file for more info
 */


#include <stdlib.h>
#include <stdio.h>

#include "video.h"
#include "objects.h"
#include "structures.h"
#include "materials.h"
#include "player.h"
#include "world.h"
#include "game.h"

namespace Objects {


void drawFoundation(int h);

//------------------------------------------------------------------------------

bool GridPoint::operator<(const GridPoint& p2) const
{
	if(x < p2.x){
		return true;
	}else{
		if(x == p2.x && y < p2.y){
			return true;
		}else{
			return false;
		}
	}
}

//------------------------------------------------------------------------------

bool GridPoint::operator== (const GridPoint& other) const
{
	return (x == other.x && y == other.y);
}

//------------------------------------------------------------------------------

bool GridPoint::operator!= (const GridPoint& other) const
{
	return !(x == other.x && y == other.y);
}

//------------------------------------------------------------------------------

GridPoint GridPoint::operator+ (const GridPoint& other) const
{
	return GridPoint(x+other.x, y+other.y);
}

//------------------------------------------------------------------------------

void GridPoint::operator+= (const GridPoint& other)
{
	x += other.x;
	y += other.y;
}

//------------------------------------------------------------------------------

GridPoint GridPoint::operator- (const GridPoint& other) const
{
	return GridPoint(x-other.x, y-other.y);
}

//------------------------------------------------------------------------------

bool GridPoint::isValid()
{
    return !(x == -1 && y == -1);
}

//------------------------------------------------------------------------------

Terrain::Terrain(double _width, double _height)
	: Object(Pd(), Qd(), Assets::Grass)
{
	width = _width;
	height = _height;
	showGrid = false;
	selected.x = -1;
	selected.y = -1;
	gridCached = false;
}

//------------------------------------------------------------------------------
void Terrain::drawGround()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2,2);
	int noX = (int) width / GRID_SIZE;
	int noY = (int) height / GRID_SIZE;
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3f(-width/2, -height/2,0.0f);
	glTexCoord2f(noX, 0);
	glVertex3f(width/2, -height/2,0.0f);
	glTexCoord2f(noX, noY);
	glVertex3f(width/2, height/2, 0.0f);
	glTexCoord2f(0, noY);
	glVertex3f(-width/2, height/2, 0.0f);
	glEnd();
	glPolygonOffset(0,0);
	glDisable(GL_POLYGON_OFFSET_FILL);
	/*if(groundCached && glIsList(groundDL)){
		glCallList(groundDL);
	}else{
		if(!glIsList(groundDL)){
			groundDL = glGenLists(1);
		}
		int noX = (int) width / GRID_SIZE;
		int noY = (int) height / GRID_SIZE;
		double halfWidth = width/2;
		double halfHeight = height/2;
		glNewList(groundDL,GL_COMPILE_AND_EXECUTE);
		//Draw all columns
		for(int i=0; i < noX; i++){
			//Draw all cells in this column
			for(int j=0; j < noY; j++){
					glBegin(GL_QUADS);
				glTexCoord2d(0, 0);
				glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
				glTexCoord2d(1, 0);
				glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
				glTexCoord2d(1, 1);
				glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
				glTexCoord2d(0, 1);
				glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
						glEnd();
			}
		}
		glEndList();
		groundCached = true;
	}*/
}

void Terrain::drawGridLines(){
	if(gridCached && glIsList(gridDL)){
		glCallList(gridDL);
	}else{
		if(!glIsList(gridDL)){
			gridDL = glGenLists(1);
		}
		material->unselect();
		int noX = (int) width / GRID_SIZE;
		int noY = (int) height / GRID_SIZE;
		//Set normal
		glNormal3i(0, 0, 1);
		double halfWidth = width/2;
		double halfHeight = height/2;
		Assets::Grid->select();
		glLineWidth(2);
		glBegin(GL_LINES);
		for(int i=0; i <= noX; i++){
			glVertex3f(i*GRID_SIZE - halfWidth, -halfHeight,0.0f);
			glVertex3f(i*GRID_SIZE - halfWidth, halfHeight,0.0f);
		}
		glEnd();
		glBegin(GL_LINES);
		for(int i=0; i <= noY; i++){
			glVertex3f(-halfWidth, i*GRID_SIZE-halfHeight,0.0f);
			glVertex3f(halfWidth, i*GRID_SIZE-halfHeight,0.0f);
		}
		glEnd();
		glLineWidth(1);
		Assets::Grid->unselect();
		material->select();
	}
}

//------------------------------------------------------------------------------

void Terrain::draw()
{
	drawGround();
	if(showGrid){
		drawGridLines();
		if(selected.isValid()){
			glPolygonOffset(-5,-5);
			int i = selected.x;
			int j = selected.y;
			double halfWidth = width/2;
			double halfHeight = height/2;
			material->unselect();
			MaterialHandle gridMat;
			float height;
			if(canPlaceStructure(selected)){
				gridMat = Assets::SelectedGrid;
				height = 0;
			}else{
				gridMat = Assets::ErrorGrid;
				height = 1;//For foundation
			}
			gridMat->select();
			glBegin(GL_LINE_STRIP);
			glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, height);
			glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, height);
			glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, height);
			glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, height);
			glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, height);
			glEnd();
			gridMat->unselect();
			material->select();
		}
	}
}

//------------------------------------------------------------------------------

void Terrain::postRender()
{
	map<GridPoint, ObjectHandle>::iterator it;
	for(it = structures.begin(); it != structures.end(); it++){
		GridPoint p = it->first;
		ObjectHandle s = it->second;
		glPushMatrix();
			glTranslated((-(width/2)) + (p.x*GRID_SIZE), (-(height/2)) + (p.y*GRID_SIZE), 0);
			s->render();
		glPopMatrix();
	}
	if(ghost.second){
		GridPoint p = ghost.first;
		glTranslated((-(width/2)) + (p.x*GRID_SIZE), (-(height/2)) + (p.y*GRID_SIZE), 0);
		(ghost.second)->render();
	}

	Object::postRender();
}

//------------------------------------------------------------------------------

//! This function draws the line from camera in direction rot. It then finds the intersection
//! with the ground: the corresponding grid coordinates are returned.
//! If there is no intersection within the bounds of the grounds
//! a gridPoint(-1,-1) is returned
GridPoint Terrain::getGridCoordinates(Pd camera, Qd rot)
{
    Vd dir = -rot * Vd(0, 1, 0);
    if(dir.z >= 0){//We are looking in the sky
        return GridPoint(-1,-1);
    }
    double lambda = -camera.z / dir.z;
    double intersecx = camera.x + dir.x * lambda;
    double intersecy = camera.y + dir.y * lambda;

    int noX = (int) width / GRID_SIZE;
    int noY = (int) height / GRID_SIZE;
    int x = (int) (floor(intersecx / (double) GRID_SIZE) + width/(2*GRID_SIZE));
    int y = (int) (floor(intersecy / (double) GRID_SIZE) + height/(2*GRID_SIZE));
    if(x < 0 || y < 0 || x >= noX || y >= noY){
        return GridPoint(-1,-1);
    }
    return GridPoint(x, y);
}

//------------------------------------------------------------------------------

void Terrain::setSelected(GridPoint p){
	if(selected.x != p.x || selected.y != p.y){
		selected = p;
		if(canPlaceStructure(p)){
			ghost = pair<GridPoint, ObjectHandle>(p, ObjectHandle(Objects::DefenseTower(0)));
		}else{
			ghost = pair<GridPoint, ObjectHandle>(GridPoint(-1, -1), ObjectHandle());
		}
	}else if(p.x == -1, p.y == -1){
		ghost = pair<GridPoint, ObjectHandle>(GridPoint(-1, -1), ObjectHandle());
	}
}

//------------------------------------------------------------------------------

bool Terrain::canPlaceStructure(GridPoint p){
	map<GridPoint, ObjectHandle>::iterator it;
	it = structures.find(p);
	if(it != structures.end()){
		return false;
	}else{
		return true;
	}
}

//------------------------------------------------------------------------------

bool Terrain::placeStructure(GridPoint p, ObjectHandle s){
	map<GridPoint, ObjectHandle>::iterator it;
	it = structures.find(p);
	if(it != structures.end()){
		return false;
	}
	Building *b = TO(Building, s);
	if(b){
		b->loc = p;
	}
	const GridPoint ip = GridPoint(p);
	structures.insert(make_pair(ip, s));
	return true;
}

//------------------------------------------------------------------------------

GridPoint Terrain::ToGrid(Pd point){
	int x = ((point.x+(width/2))/GRID_SIZE);
	int y = ((point.y+(height/2))/GRID_SIZE) + 1;
	return GridPoint(x, y);
}

//------------------------------------------------------------------------------

Pd Terrain::ToPointD(GridPoint point){
	int x = (point.x*GRID_SIZE)-(width/2);
	int y = (point.y*GRID_SIZE)-(height/2);
	return Pd(x, y, 0);
}

//------------------------------------------------------------------------------

void Building::preRender(){
	Object::preRender();

	if(built) return;

	int now = Video::ElapsedTime();
	if((now-buildTime) > buildDuration){       
		built = true; 
		return;
	}

	float animationHeight = ((float)height/(float)buildDuration)*(float)now
		 - ((float)buildTime*((float)height/(float)buildDuration));
	float randX = (((float)rand()/RAND_MAX)-0.5);
	float randY = (((float)rand()/RAND_MAX)-0.5);
	glTranslatef(randX, randY, -height + animationHeight);
}

//------------------------------------------------------------------------------

Mine::Mine(Pd P, Qd R, BoundingBox B, Resource _maxIncome)
		: Structure(B), maxIncome(_maxIncome)
{
	model.rock = ModelObjectContainer();
	model.rock->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.rock->children.insert(Assets::Model::RockObj);
	children.insert(model.rock);
	model.rock->material = Assets::Model::RockTex;
	material = Assets::Grass;
}

//------------------------------------------------------------------------------

void Mine::draw() {
	drawFoundation(1);
}

//------------------------------------------------------------------------------

DefenseTower::DefenseTower(Player::Id _owner)
		: Building(4, BoundingBox(),
			100, 0,
			Video::ElapsedTime(), 10000,
			20, _owner) 
{
	model.turret = ModelObjectContainer();
	model.turret->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.turret->children.insert(Assets::Model::TurretObj);
	children.insert(model.turret);
	int i = 2;
	if (Game::game.players.count(owner))
		i = TO(Player,Game::game.players[owner])->team-'a';
	model.turret->material = Assets::Model::TurretTex[i];
	material = Assets::Grass;
	lastshot = Video::ElapsedTime();
}

//------------------------------------------------------------------------------

DefenseTower::DefenseTower(int buildTime)
		: Building(4, BoundingBox(),
			0, 0,
			Video::ElapsedTime(), buildTime,
			0, -1)
{
	model.turret = ModelObjectContainer();
	model.turret->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.turret->children.insert(Assets::Model::TurretObj);
	children.insert(model.turret);
	model.turret->material = Assets::Model::GhostTurretTex;
	material = Assets::Model::GhostTurretTex;
}

//------------------------------------------------------------------------------


void DefenseTower::frame()
{
	#define RANGE 40.0
	#define ROF 1000

	float movemulti = Video::CurrentFPS()/60;
	float movespeed = movemulti*0.2;

	World *w = TO(World, Game::game.world);
	if(!w) return;

	Pd worldcoord = w->terrain->ToPointD(loc);
	worldcoord.x += (int)GRID_SIZE/2;
	worldcoord.y += (int)GRID_SIZE/2;
	worldcoord.z = 0.0;

	Player *own = NULL;
	if(Game::game.players.count(owner))
		own = TO(Player, Game::game.players[owner]); 

	//Find nearest player
	if(own && built){
		ObjectHandle closest;
		double distance = RANGE;
		set<ObjectHandle>::iterator it;
		for(it = w->children.begin(); it != w->children.end(); it++){
			Player *p = TO(Player, *it);
			DefenseTower *t = TO(DefenseTower, *it);
			if(p){
				if(p->team != own->team){
					//Enemy player found
					double curr_dist = !(Vd(p->origin) + -Vd(worldcoord));
					if(curr_dist < distance){
						distance = curr_dist;
						closest = *it;
						continue;
					}
				}
			}else if(t){
				Player *t_own = NULL;
				if (Game::game.players.count(t->owner))
					TO(Player, Game::game.players[t->owner]);
				if(t_own && t_own->team != own->team){
					//Enemy tower found
					double curr_dist =!(Vd(t->origin) + -Vd(worldcoord));
					if(curr_dist < distance){
						distance = curr_dist;
						closest = *it;
						continue;
					}
				}
			}
		}
		if(closest){
			Qd target = worldcoord.lookAt(closest->origin);
			
			Rd angleRot = (~model.turret->rotation) * -(~target);

			double angle = fmod(angleRot.a, 2*Pi);

			if(angle < 0.07){
				//Locked
				int now = Video::ElapsedTime();
				if(now - lastshot > ROF){
					//Shoot
					lastshot = now;
					w->addLaserBeam(LaserBeam(worldcoord, target));
				}
			}else{
				if(angleRot.v.z == 1) movespeed *= -1;
				model.turret->rotation = model.turret->rotation * Rd(movespeed, Vd(0, 0, 1));
			}
		}
	}
}


//------------------------------------------------------------------------------

void DefenseTower::draw()
{
	drawFoundation(1);
}

//------------------------------------------------------------------------------

void DefenseTower::render()
{
	frame();
	Object::render();
}

//------------------------------------------------------------------------------

ResourceMine::ResourceMine(Player::Id _owner)
		: Building(15, BoundingBox(), 300, 30,
			Video::ElapsedTime(), 20000,
			0, _owner)
{
	model.rock = ModelObjectContainer();
	model.rig = ModelObjectContainer();
	model.drill = ModelObjectContainer();
	model.rock->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.rig->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.drill->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.rock->children.insert(Assets::Model::RockObj);
	model.rig->children.insert(Assets::Model::MineObj);
	model.drill->children.insert(Assets::Model::DrillObj);
	children.insert(model.rock);
	children.insert(model.rig);
	children.insert(model.drill);

	model.rock->material = Assets::Model::RockTex;

	int i = 1;
	if (Game::game.players.count(owner))
		i = TO(Player,Game::game.players[owner])->team-'a';
	model.rig->material = Assets::Model::MineTex[i];
	model.drill->material = Assets::Model::DrillTex[i];

	material = Assets::Grass;
}

//------------------------------------------------------------------------------

//Ghost constructor
ResourceMine::ResourceMine(int buildTime)
		: Building(15, BoundingBox(), 0, 0,
			Video::ElapsedTime(), buildTime,
			0, -1)
{
	model.rig = ModelObjectContainer();
	model.drill = ModelObjectContainer();
	model.rig->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.drill->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.rig->children.insert(Assets::Model::MineObj);
	model.drill->children.insert(Assets::Model::DrillObj);
	children.insert(model.rig);
	children.insert(model.drill);
	
	model.rig->material = Assets::Model::GhostTurretTex;
	model.drill->material = Assets::Model::GhostTurretTex;
}


//------------------------------------------------------------------------------

void ResourceMine::draw() 
{
	model.drill->rotation = model.drill->rotation * Rd(0.1,Vd(0,0,1));
	if(owner != -1)
		drawFoundation(1);
}

//------------------------------------------------------------------------------

void drawFoundation(int h) {
	glBegin(GL_QUADS);
		//Front side
		glNormal3i(0, -1, 0);
		glTexCoord2i(0, 0);	glVertex3i(0, 0, 0);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, 0);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, h);
		glTexCoord2i(0, 0);	glVertex3i(0, 0, h);

		//Right side
		glNormal3i(1, 0, 0);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, 0);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, 0);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, h);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, h);

		//Back side
		glNormal3i(0, 1, 0);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, 0);
		glTexCoord2i(1, 0);	glVertex3i(0, GRID_SIZE, 0);
		glTexCoord2i(1, 0);	glVertex3i(0, GRID_SIZE, h);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, h);

		//Left side
		glNormal3i(-1, 0, 0);
		glTexCoord2i(1, 0); glVertex3i(0, GRID_SIZE, 0);
		glTexCoord2i(0, 0);	glVertex3i(0, 0, 0);
		glTexCoord2i(0, 0);	glVertex3i(0, 0, h);
		glTexCoord2i(1, 0);	glVertex3i(0, GRID_SIZE, h);

		//Top side
		glNormal3i(0, 0, 1);
		glTexCoord2i(0, 0); glVertex3i(0, 0, h);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, h);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, h);
		glTexCoord2i(1, 0);	glVertex3i(0, GRID_SIZE, h);
	glEnd();
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------

