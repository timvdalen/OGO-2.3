/*
 * Core module -- see header
 */

#include <string>
#include "video.h"
#include "core.h"

namespace Core {

using namespace std;

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
	
	material->select();
	
	draw();
	
	material->unselect();
	
	postRender();
}

//------------------------------------------------------------------------------

void Object::draw()
{
	// Nothing to render, but will render its children next
}

//------------------------------------------------------------------------------

void Object::postRender()
{
	set<ObjectHandle>::iterator it;
	for (it = children.begin(); it != children.end(); ++it)
		(*it)->render();
	
	glPopMatrix();
}

//------------------------------------------------------------------------------

Player::Player(Id _id, unsigned char _team, string _name){
	id = id;
	team = _team;
	name = _name;
}

//------------------------------------------------------------------------------

} // namespace Core

//------------------------------------------------------------------------------
