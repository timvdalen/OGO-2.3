/*
 * Structure objects -- see header file for more info
 */

#include "objects.h"
#include "structures.h"

namespace Objects {

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

//! This function draws the line from camera to pos. It then finds the intersection
//! with the ground: the corresponding grid coordinates are returned.
Terrain::GridPoint Terrain::getGridCoordinates(Vd camera, Vd pos)
{
    Vd dir = pos + -camera;
    if(dir.z >= 0){//We are looking in the sky
        return GridPoint(-1,-1);
    }
    double lambda = -camera.z / dir.z;
    double intersecx = camera.x + dir.x * lambda;
    double intersecy = camera.y + dir.y * lambda;

    int x = (int) floor(intersecx / (double) GRID_SIZE) + width/(2*GRID_SIZE);
    int y = (int) floor(intersecy / (double) GRID_SIZE) + height/(2*GRID_SIZE);
    return GridPoint(x, y);
}


//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
