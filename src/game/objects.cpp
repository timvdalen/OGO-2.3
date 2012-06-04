/*
 * Objects -- see header
 */

#include <stdio.h>
#include "objects.h"
#include "video.h"

namespace Objects {

//------------------------------------------------------------------------------

ObjectHandle BoundedObject::checkCollision(Point<double> origin, Vector<double> direction)
{
	return (Object());
}

//------------------------------------------------------------------------------

void World::draw(){
	#define HIGH 500

	double halfWidth = width/2;
	double halfHeight = height/2;


	glBegin(GL_TRIANGLES);
		glTexCoord2d(0, 0);
		glVertex3f(-1 * halfWidth, -1 * halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(-1 * halfWidth, halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(halfWidth, halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(halfWidth, -1* halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(-1 * halfWidth, halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(halfWidth, halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(halfWidth, -1 * halfHeight, 0);
		glTexCoord2d(1, 0);
		glVertex3f(-1 * halfWidth, -1 * halfHeight, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);
		
	glEnd();

	#undef HIGH
}

//------------------------------------------------------------------------------

void World::postRender(){
	terrain.render();

	Object::postRender();
}

//------------------------------------------------------------------------------

void Terrain::draw(){
	int noX = (int) width/GRID_SIZE;
	int noY = (int) height/GRID_SIZE;

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
				Material *gridMat;
				if(selected.x == i && selected.y == j){
					gridMat = new Materials::SelectedGridMaterial();
				}else{
					gridMat = new Materials::GridMaterial();
				}
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
				material->select();
			}
		}
	}
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
