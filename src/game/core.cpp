/*
 * Core module -- see header
 */

#include "video.h"
#include "core.h"

namespace Core {

//------------------------------------------------------------------------------

void Object::preRender()
{
	// Stack has only at least 32 levels! Maybe just translate twice...
	glPushMatrix();
	glTranslated(origin.x, origin.y, origin.z);
}

//------------------------------------------------------------------------------

void Object::render()
{
	preRender();
	
	draw();
	
	postRender();
}

//------------------------------------------------------------------------------

void Object::draw(){
	// Nothing to render, but will render its children next
}


void Object::postRender()
{
	set<ObjectHandle>::iterator it;
	for (it = children.begin(); it != children.end(); ++it)
		(*it)->render();
	
	glPopMatrix();
}

//------------------------------------------------------------------------------

} // namespace Core

//------------------------------------------------------------------------------
