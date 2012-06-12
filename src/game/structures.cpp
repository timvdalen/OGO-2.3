/*
 * Structure objects -- see header file for more info
 */


#include <stdlib.h>

#include "video.h" 
#include "objects.h"
#include "structures.h"
#include "materials.h"

namespace Objects {

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
}

//------------------------------------------------------------------------------

void Terrain::draw()
{
	int noX = (int) width / GRID_SIZE;
	int noY = (int) height / GRID_SIZE;

	//Set normal
	glNormal3i(0, 0, 1);

	//Draw all columns
	for(int i=0; i < noX; i++){
		//Draw all cells in this column
		for(int j=0; j < noY; j++){
			double halfWidth = width/2;
			double halfHeight = height/2;
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
			if(showGrid){
				MaterialHandle gridMat;
				if(selected.x == i && selected.y == j){
					gridMat = Assets::SelectedGrid;
				}else{
					gridMat = Assets::Grid;
				}
				material->unselect();
				gridMat->select();
				glBegin(GL_LINES);
					glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);

					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);

					glVertex3f(((i+1) * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
					glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);

					glVertex3f((i * GRID_SIZE) - halfWidth, ((j+1) * GRID_SIZE) - halfHeight, 0);
					glVertex3f((i * GRID_SIZE) - halfWidth, (j * GRID_SIZE) - halfHeight, 0);
				glEnd();
				gridMat->unselect();
				material->select();
			}
		}
	}
}

//------------------------------------------------------------------------------

void Terrain::postRender()
{
	multimap<GridPoint, ObjectHandle>::iterator it;
	for(it = structures.begin(); it != structures.end(); it++){
		GridPoint p = it->first;
		ObjectHandle s = it->second;
		glPushMatrix();
			glTranslated((-(width/2)) + (p.x*GRID_SIZE), (-(height/2)) + (p.y*GRID_SIZE), 0);
			s->render();
		glPopMatrix();
	}

	Object::postRender();
}

//------------------------------------------------------------------------------

//! This function draws the line from camera to pos. It then finds the intersection
//! with the ground: the corresponding grid coordinates are returned.
//! If there is no intersection within the bounds of the grounds
//! a gridPoint(-1,-1) is returned
GridPoint Terrain::getGridCoordinates(Vd camera, Vd pos)
{
    Vd dir = pos + -camera;
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

bool Terrain::placeStructure(GridPoint p, ObjectHandle s){
    multimap<GridPoint, ObjectHandle>::iterator it;
    it = structures.find(p);
	if(it != structures.end()){
		return false;
	}
	const GridPoint ip = GridPoint(p);
	structures.insert(make_pair(ip, s));
	return true;
}

//------------------------------------------------------------------------------

void Building::preRender(){
	Object::preRender();
	
	int now = Video::ElapsedTime();
	if((now-buildTime) > buildDuration) 
		return;
		
	float animationHeight = ((float)height/(float)buildDuration)*(float)now
		 - ((float)buildTime*((float)height/(float)buildDuration));
	float randX = (((float)rand()/RAND_MAX)-0.5);
	float randY = (((float)rand()/RAND_MAX)-0.5);
	glTranslatef(randX, randY, -height + animationHeight);
}

//------------------------------------------------------------------------------

DefenseTower::DefenseTower()
		: Building(10, BoundingBox(),
			100, 0,
			Video::ElapsedTime(), 10000,
			20) 
{
	height = 10;
}

//------------------------------------------------------------------------------

void DefenseTower::draw()
{
	glBegin(GL_QUADS);
		//Front side
		glNormal3i(0, -1, 0);
		glTexCoord2i(0, 0);	glVertex3i(0, 0, 0);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, 0);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, 0, height);
		glTexCoord2i(1, 0);	glVertex3i(0, 0, height);
		
		//Right side
		glNormal3i(1, 0, 0);
		glTexCoord2i(0, 0);	glVertex3i(GRID_SIZE, 0, 0);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, 0);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, height);
		glTexCoord2i(1, 0);	glVertex3i(GRID_SIZE, 0, height);
		
		//Back side
		glNormal3i(0, 1, 0);
		glTexCoord2i(0, 0);	glVertex3i(GRID_SIZE, GRID_SIZE, 0);
		glTexCoord2i(0, 1);	glVertex3i(0, GRID_SIZE, 0);
		glTexCoord2i(1, 1);	glVertex3i(0, GRID_SIZE, height);
		glTexCoord2i(1, 0);	glVertex3i(GRID_SIZE, GRID_SIZE, height);
		
		//Left side
		glNormal3i(-1, 0, 0);
		glTexCoord2i(0, 0); glVertex3i(0, GRID_SIZE, 0);
		glTexCoord2i(0, 1);	glVertex3i(0, 0, 0);
		glTexCoord2i(1, 1);	glVertex3i(0, 0, height);
		glTexCoord2i(1, 0);	glVertex3i(0, GRID_SIZE, height);
		
		//Top side
		glNormal3i(0, 0, 1);
		glTexCoord2i(0, 0); glVertex3i(0, 0, height);
		glTexCoord2i(0, 1);	glVertex3i(GRID_SIZE, 0, height);
		glTexCoord2i(1, 1);	glVertex3i(GRID_SIZE, GRID_SIZE, height);
		glTexCoord2i(1, 0);	glVertex3i(0, GRID_SIZE, height);
	glEnd();
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
