/*
 * Materials -- see header file for more info
 */

#include "video.h"
#include "materials.h"

namespace Materials {

//------------------------------------------------------------------------------

void ShadedMaterial::select()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

//------------------------------------------------------------------------------

} // namespace Materials

//------------------------------------------------------------------------------
