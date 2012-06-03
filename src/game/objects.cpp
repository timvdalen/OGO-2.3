/*
 * Objects -- see header
 */

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

} // namespace Objects

//------------------------------------------------------------------------------
