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

	glBegin(GL_TRIANGLES);
		glTexCoord2d(0, 0);
		glVertex3f(-1 * width, -1 * height, 0);
		glTexCoord2d(1, 0);
		glVertex3f(-1 * width, height, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(width, height, 0);
		glTexCoord2d(1, 0);
		glVertex3f(width, -1* height, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(-1 * width, height, 0);
		glTexCoord2d(1, 0);
		glVertex3f(width, height, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);

		glTexCoord2d(0, 0);
		glVertex3f(width, -1 * height, 0);
		glTexCoord2d(1, 0);
		glVertex3f(-1 * width, -1 * height, 0);
		glTexCoord2d(0.5, 1);
		glVertex3f(0, 0, HIGH);
		
	glEnd();

	#undef HIGH
}


//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
