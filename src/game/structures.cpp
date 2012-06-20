/*
 * Structure objects -- see header file for more info
 */


#include <stdlib.h>
#include <stdio.h>
#include <limits>

#include "video.h"
#include "objects.h"
#include "structures.h"
#include "materials.h"
#include "player.h"
#include "world.h"
#include "game.h"

namespace Objects {
#define HIGH 100

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

string convert(const GridPoint &g) { char buffer[1024];
	sprintf(buffer, "G%d,%d", g.x, g.y); return string(buffer); }

//------------------------------------------------------------------------------

GridPoint ToGridPoint(const string &str) { GridPoint g;
	sscanf(str.c_str(), "G%ld,%ld", &g.x, &g.y); return g; }

//------------------------------------------------------------------------------

Terrain::Terrain(double _width, double _height)
	: BoundedObject(Pd(), Qd(),
	  BoundingBox(Pd(-_width/2, -_height/2, 0), Pd(_width,0,0), Pd(0,_height,0), Pd(_width,_height,0), Pd(), Pd(), Pd(), Pd(_width/2, _height/2, HIGH)),
	  Assets::Grass)
{
	width = _width;
	height = _height;
	showGrid = false;
	selected.x = -1;
	selected.y = -1;
	gridCached = false;
}

//------------------------------------------------------------------------------
pair<ObjectHandle,double> Terrain::checkCollision(Pd origin, Vd direction)
{
    map<GridPoint, ObjectHandle>::iterator it;
	ObjectHandle colobject;
    double collision = numeric_limits<double>::infinity();
    for (it = structures.begin(); it != structures.end(); ++it){
		Structure* child = TO(Structure, it->second);
        if(child){
			Point<double> newp = (origin - ToPointD(child->loc));
            pair<ObjectHandle, double> childcollision = child->checkCollision(newp, direction);
            if(childcollision.second < collision){ //We have a collision with a child
            	collision = childcollision.second;
                colobject = it->second;
			}
    	}
	}
	return make_pair(colobject,collision);
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
			int structure = canPlaceStructure(selected);
			switch(structure){
			case 1:{
				gridMat = Assets::SelectedGrid;
				height = 0;
				}
				break;
			case 11:{
				gridMat = Assets::ErrorGrid;
				height = 0;
				}
				break;
			case 2:{
				gridMat = Assets::SelectedGrid;
				height = 1;//For foundation
				}
				break;
			case 22:{
				gridMat = Assets::ErrorGrid;
				height = 0;
				}
				break;
			default:{
				gridMat = Assets::ErrorGrid;
				height = 1;//For foundation
				}
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
	for(it = structures.begin(); it != structures.end();){
		GridPoint p = it->first;
		ObjectHandle s = it->second;
		glPushMatrix();
			glTranslated((-(width/2)) + (p.x*GRID_SIZE), (-(height/2)) + (p.y*GRID_SIZE), 0);
			Building *b = TO(Building, s);
			if(b && b->isDestroyed()){
				//if(b->owner == Game::game.player->id){
					//Drop some coins
					Resource toDrop = b->cost/2;
					int noCoins = toDrop/20;
					for(int i=0; i < noCoins; i++){
						Pd droppoint = Game::game.world->terrain->ToPointD(b->loc);
						droppoint.x += (rand()%10);
						droppoint.y += (rand()%10);
						Game::game.world->temporary.push_back(Droppable(droppoint, 20));
					}
					//TODO: Send this over the network
				//}
				structures.erase(it++);
				glPopMatrix();
				continue;
			}
			s->render();
		glPopMatrix();
		it++;
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
		int structure = canPlaceStructure(p);
		switch(structure){
		case 1: ghost = pair<GridPoint, ObjectHandle>(p, ObjectHandle(Objects::DefenseTower(0))); break;
		case 11: ghost = pair<GridPoint, ObjectHandle>(p, ObjectHandle(Objects::DefenseTower(0, true))); break;
		case 2: ghost = pair<GridPoint, ObjectHandle>(p, ObjectHandle(Objects::ResourceMine(0))); break;
		case 12: ghost = pair<GridPoint, ObjectHandle>(p, ObjectHandle(Objects::ResourceMine(0, true))); break;
		default:
			ghost = ghost = pair<GridPoint, ObjectHandle>(GridPoint(-1, -1), ObjectHandle()); break;
		}
	}else if(p.x == -1, p.y == -1){
		ghost = pair<GridPoint, ObjectHandle>(GridPoint(-1, -1), ObjectHandle());
	}
}

//------------------------------------------------------------------------------

int Terrain::canPlaceStructure(GridPoint p){
	map<GridPoint, ObjectHandle>::iterator it;
	it = structures.find(p);
	Resource resources = 0;
	map<unsigned char,Team>::iterator team_it = Game::game.teams.find(Game::game.player->team);
	if(team_it != Game::game.teams.end()) resources = team_it->second.resources;
	if(it != structures.end()){
		Structure *s = TO(Structure, it->second);
		if(!s) return 0;
		if(s->type() == "Mine"){
			if(resources >= ResourceMine(Game::game.player->id).cost){
				return 2;
			}else{
				return 12;
			}
		}else{
			return 0;
		}
	}else{
		if(resources >= DefenseTower(Game::game.player->id).cost){
			return 1;
		}else{
			return 11;
		}
	}
}

//------------------------------------------------------------------------------

bool Terrain::placeStructure(GridPoint p, ObjectHandle s){
	if(!s) return false;
	Structure *struc = TO(Structure, s);

	int structure = canPlaceStructure(p);
	if(!struc) return false;
	if(structure == 0 || (structure == 2 && struc->type() != "ResourceMine") || (struc->type() == "HeadQuarters" && !(canPlaceStructure(GridPoint(p.x-1, p.y)) && canPlaceStructure(GridPoint(p.x-1, p.y-1)) && canPlaceStructure(GridPoint(p.x, p.y-1))))){
		return false;
	}
	struc-> loc = p;
	Building *b = TO(Building, s);
	const GridPoint ip = GridPoint(p);
	if(structure == 2){
		structures.erase(p);
	}
	structures.insert(make_pair(ip, s));
	if(struc->type() == "HeadQuarters"){
		ObjectHandle block = Structure();
		structures.insert(make_pair(GridPoint(p.x-1, p.y), block));
		structures.insert(make_pair(GridPoint(p.x-1, p.y-1), block));
		structures.insert(make_pair(GridPoint(p.x, p.y-1), block));
	}
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

void Building::drawHealthbar(){
	if(Game::game.players.count(owner)){
		glDisable(GL_LIGHTING);//It might not be the best idea to switch lighting off and on so much
		glPushMatrix();
			//Draw health bar
			if(type() == "HeadQuarters"){
				glTranslated(0, 0, (double)height+1);
			}else if(type() == "ResourceMine"){
				glTranslated(GRID_SIZE/2, GRID_SIZE/2, (double)height+3);
			}else{
				glTranslated(GRID_SIZE/2, GRID_SIZE/2, (double)height+1);
			}
			glRotated(90,1,0,0);//Revert custom axis
			applyBillboarding();
			glScalef(0.1, 0.1, 0.1);    //Scale down
			Assets::HealthBar::Border->select();
			glBegin(GL_LINES);
				glVertex3f(-16.01, -1.01, 0.0);
				glVertex3f(16.01, -1.01, 0.0);

				glVertex3f(16.01, -1.01, 0.0);
				glVertex3f(16.01, 1.01, 0.0);

				glVertex3f(16.01, 1.01, 0.0);
				glVertex3f(-16.01, 1.01, 0.0);

				glVertex3f(-16.01, 1.01, 0.0);
				glVertex3f(-16.01, -1.01, 0.0);
			glEnd();
			Assets::HealthBar::Border->unselect();
			double barwidth = ((health/maxHealth)*32) - 16.0;
			Assets::HealthBar::Green->select();
			glBegin(GL_QUADS);
				glVertex3f(-16.0, -1.0, 0.0);
				glVertex3f(barwidth, -1.0, 0.0);
				glVertex3f(barwidth, 1.0, 0.0);
				glVertex3f(-16.0, 1.0, 0.0);
			glEnd();
			Assets::HealthBar::Green->unselect();
			Assets::HealthBar::Red->select();
			glBegin(GL_QUADS);
				glVertex3f(barwidth, -1.0, 0.0);
				glVertex3f(16.0, -1.0, 0.0);
				glVertex3f(16.0, 1.0, 0.0);
				glVertex3f(barwidth, 1.0, 0.0);
			glEnd();
			Assets::HealthBar::Red->unselect();
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}

void Building::preRender(){
	Object::preRender();

	glPushMatrix();

	if(built) return;

	int now = Video::ElapsedTime();
	if((now-buildTime) > buildDuration){
		built = true;
		return;
	}

	float animationHeight = ((float)height/(float)buildDuration)*(float)now
		 - ((float)buildTime*((float)height/(float)buildDuration));
	float percdone = 1-(animationHeight/height);
	float randX = ((((float)rand()/RAND_MAX)-0.5)*percdone);
	float randY = ((((float)rand()/RAND_MAX)-0.5)*percdone);
	glTranslatef(randX, randY, -height + animationHeight);
}

//------------------------------------------------------------------------------

void Building::postRender(){
	Object::postRender();
	glPopMatrix();//This is the matrix that was pushed in Object::preRender()
	drawHealthbar();
}

//------------------------------------------------------------------------------

void Building::frame()
{
	if(owner && built && income > 0 && owner == Game::game.player->id){
		int now = Video::ElapsedTime();
		if(now-lastGenerated > 5000){
			map<unsigned char,Team>::iterator it = Game::game.teams.find(Game::game.player->team);
			if(it != Game::game.teams.end()){
				//TODO: Send this over the network
				it->second.resources += income;
				lastGenerated = Video::ElapsedTime();
			}
		}
	}
}

//------------------------------------------------------------------------------

void Building::render()
{
	frame();
	Object::render();
}


//------------------------------------------------------------------------------

Mine::Mine(Pd P, Qd R, Resource _maxIncome)
		: Structure(BoundingBox(Pd(0,0,0),Pd(10.0,10.0,3.419255))), maxIncome(_maxIncome)
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

Wall::Wall(Pd P)
	: Structure(BoundingBox(Pd(0.0, 0.0, 0.0), Pd(10.0, 10.0, 10.0)))
{
	material = Assets::Wall;
}

//------------------------------------------------------------------------------

void Wall::draw(){
	#define Vert(v) { Pd vt = (v); glVertex3d(vt.x,vt.y,vt.z); }
	#define Norm(u,v) { Vd n = ~((u)*(v)); glNormal3d(n.x,n.y,n.z); }
	#define A { glTexCoord2d(0.0,0.0); }
	#define B { glTexCoord2d(1.0,0.0); }
	#define C { glTexCoord2d(1.0,1.0); }
	#define D { glTexCoord2d(0.0,1.0); }

	Vd u = Vd(10, 0, 0);
	Vd v = Vd(0, 10, 0);
	Vd w = Vd(0, 0, 10);

	Pd o = Vd(0,0,0);
	Pd a = o + u + v + w;

	glBegin(GL_QUADS);
		Norm(u,w); A Vert(o); B Vert(o+u); C Vert(o+u+w); D Vert(o+w);
		Norm(v,u); A Vert(o); B Vert(o+v); C Vert(o+v+u); D Vert(o+u);
		Norm(w,v); A Vert(o); B Vert(o+w); C Vert(o+w+v); D Vert(o+v);
		Norm(u,v); A Vert(a); B Vert(a-u); C Vert(a-u-v); D Vert(a-v);
		Norm(v,w); A Vert(a); B Vert(a-v); C Vert(a-v-w); D Vert(a-w);
		Norm(w,u); A Vert(a); B Vert(a-w); C Vert(a-w-u); D Vert(a-u);
	glEnd();

	#undef Vert
	#undef Norm
	#undef A
	#undef B
	#undef C
	#undef D
}

//------------------------------------------------------------------------------

HeadQuarters::HeadQuarters(Player::Id _owner)
		: Building(10, BoundingBox(Pd(-10,-10,0),Pd(10,10,25.0)),
		  0, 10,
		  0, 0,
		  0, _owner, 600.0)
{
	model.base = ModelObjectContainer();
	model.socket = ModelObjectContainer();
	model.core = ModelObjectContainer();
	model.coreinv = ModelObjectContainer();
	model.base->children.insert(Assets::Model::HQBaseObj);
	model.socket->children.insert(Assets::Model::HQSocketObj);
	model.core->children.insert(Assets::Model::HQCoreObj);
	model.coreinv->children.insert(Assets::Model::HQCoreinvObj);
	children.insert(model.base);
	children.insert(model.socket);
	children.insert(model.core);
	children.insert(model.coreinv);
	int i = 2;
	if (Game::game.players.count(owner))
		i = TO(Player,Game::game.players[owner])->team-'a';

	model.base->material = Assets::Model::HQBaseTex[i];
	model.socket->material = Assets::Model::HQSocketTex;
	model.core->material = Assets::Model::HQCoreTex[i];
	model.coreinv->material = Assets::Model::HQSocketTex;
}

//------------------------------------------------------------------------------

DefenseTower::DefenseTower(Player::Id _owner)
		: Building(4, BoundingBox(Pd(0,0,0),Pd(10.0,10.0,4.147209)),
			100, 0,
			Video::ElapsedTime(), 10000,
			20, _owner, 300.0)
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

DefenseTower::DefenseTower(int buildTime, bool error)
		: Building(4, BoundingBox(Pd(0,0,0),Pd(10.0,10.0,4.147209)),
			0, 0,
			Video::ElapsedTime(), buildTime,
			0, -1, -1)
{
	model.turret = ModelObjectContainer();
	model.turret->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.turret->children.insert(Assets::Model::TurretObj);
	children.insert(model.turret);
	if(error){
		material = Assets::Model::GhostErrorTex;
		model.turret->material = Assets::Model::GhostErrorTex;
	}else{
		material = Assets::Model::GhostTex;
		model.turret->material = Assets::Model::GhostTex;
	}
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
	worldcoord.z = 2.68;

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
			Building *b = TO(Building, *it);
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
			}
		}
		map<GridPoint, ObjectHandle>::iterator mit;
		for(mit = w->terrain->structures.begin(); mit != w->terrain->structures.end(); mit++){
			Building *b = TO(Building, mit->second);
			if(b){
				Player *b_own = NULL;
				if (Game::game.players.count(b->owner))
					b_own = TO(Player, Game::game.players[b->owner]);
				if(b_own && b_own->team != own->team){
					//Enemy tower found
					double curr_dist =!(Vd(w->terrain->ToPointD(mit->first)) + -Vd(worldcoord));
					if(curr_dist < distance){
						distance = curr_dist;
						closest = mit->second;
						continue;
					}
				}
			}
		}

		if(closest){
			Player *p = TO(Player, closest);
			Building *b = TO(Building, closest);

			Pd targetPoint;
			if(b){
				if(TO(HeadQuarters, b)){
					targetPoint = w->terrain->ToPointD(b->loc);
				}else{
					targetPoint = w->terrain->ToPointD(b->loc);
					targetPoint.x += GRID_SIZE/2;
					targetPoint.y += GRID_SIZE/2;
				}
			}else{
				targetPoint = closest->origin;
			}

			Qd target = worldcoord.lookAt(targetPoint);

            Rd angleRot = Rd((~model.turret->rotation));
            double anglePlayer = atan2(targetPoint.x-worldcoord.x, targetPoint.y-worldcoord.y);

			double rot = angleRot.a;

			double z = angleRot.v.z;
			double angleDiff = rot*z - anglePlayer;

			double angle = fabs(fmod(angleDiff, 2*Pi));

            double angP = fmod(anglePlayer + 2*Pi, 2*Pi);
			double angR = fmod(rot*z + 2*Pi, 2*Pi);

			if(angle < 0.03){
				//Locked
				int now = Video::ElapsedTime();
				if(now - lastshot > ROF){
					//Shoot animation
					lastshot = now;
					Pd startpoint = worldcoord+Vd(5*sin(rot*z)+0.5*cos(rot*z), 5*cos(rot*z)-0.5*sin(rot*z), 0);
					Pd startpoint2 = worldcoord+Vd(5*sin(rot*z)-0.5*cos(rot*z), 5*cos(rot*z)+0.5*sin(rot*z), 0);
					//Qd beam = gunLoc.lookAt(target);
                    w->addLaserBeam(LaserBeam(startpoint, target, 100));
					w->addLaserBeam(LaserBeam(startpoint2, target, 100));

					//Actual damage
					if(own->id == Game::game.player->id){
						if(p){
							p->damage(attackPower);
						}else if(b){
							b->damage(attackPower);
						}
						//TODO: Send over the network
					}
				}
			}else{
			    float phi = min(fabs(angP-angR), fabs(angR-angP));
			    phi = !(phi >= 0 || phi <= 0) ? 100 : phi;
			    float sign = ((angP - angR > 0 && angP - angR < Pi) || (angP-angR > -2*Pi && angP-angR < -Pi)) ? 1.0 : -1.0;
			    float theta = min(phi, movespeed);
			    theta *= sign;
				model.turret->rotation = model.turret->rotation * Rd(theta, Vd(0, 0, 1));
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

ResourceMine::ResourceMine(Player::Id _owner)
		: Building(8, BoundingBox(Pd(0,0,0),Pd(10.0,10.0,10.6)), 220, 30,
			Video::ElapsedTime(), 20000,
			0, _owner, 200.0)
{
	model.rig = ModelObjectContainer();
	model.drill = ModelObjectContainer();
	model.rig->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.drill->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.rig->children.insert(Assets::Model::MineObj);
	model.drill->children.insert(Assets::Model::DrillObj);
	children.insert(model.rig);
	children.insert(model.drill);

	rock = ModelObjectContainer();
	rock->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	rock->children.insert(Assets::Model::RockObj);
	rock->material = Assets::Model::RockTex;

	int i = 1;
	if (Game::game.players.count(owner))
		i = TO(Player,Game::game.players[owner])->team-'a';
	model.rig->material = Assets::Model::MineTex[i];
	model.drill->material = Assets::Model::DrillTex[i];
}

//------------------------------------------------------------------------------

//Ghost constructor
ResourceMine::ResourceMine(int buildTime, bool error)
		: Building(8, BoundingBox(Pd(0,0,0), Pd(10.0,10.0,10.6)), 0, 0,
			Video::ElapsedTime(), buildTime,
			0, -1, -1)
{
	model.rig = ModelObjectContainer();
	model.drill = ModelObjectContainer();
	model.rig->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.drill->origin = Pd(GRID_SIZE/2,GRID_SIZE/2,1);
	model.rig->children.insert(Assets::Model::MineObj);
	model.drill->children.insert(Assets::Model::DrillObj);
	children.insert(model.rig);
	children.insert(model.drill);

	if(error){
		model.rig->material = Assets::Model::GhostErrorTex;
		model.drill->material = Assets::Model::GhostErrorTex;
	}else{
		model.rig->material = Assets::Model::GhostTex;
		model.drill->material = Assets::Model::GhostTex;
	}
}


//------------------------------------------------------------------------------

void ResourceMine::draw()
{
	if(built)
		model.drill->rotation = model.drill->rotation * Rd(0.1,Vd(0,0,1));
}

//------------------------------------------------------------------------------

void ResourceMine::postRender()
{
	Object::postRender();
	if(rock) rock->render();
	if(Game::game.players.count(owner)){
		Assets::Grass->select();
		drawFoundation(1);
		Assets::Grass->unselect();
	}
	glPopMatrix();//This is the matrix thas was pushed in Object::preRender()
	Building::drawHealthbar();
}

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

