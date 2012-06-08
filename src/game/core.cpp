/*
 * Core module -- see header
 */

#include <string>
#include "video.h"
#include "core.h"

namespace Core {

using namespace std;
using namespace Base::Alias;

//------------------------------------------------------------------------------

void Object::preRender()
{
	// Stack has only at least 32 levels! Maybe just translate twice...
	glPushMatrix();
	//glTranslated(origin.x, origin.y, origin.z);
	//Rotation<double> rot = rotation;
	//glRotated(rot.a, rot.v.x, rot.v.y, rot.v.z);
	
	// Camera orientation (quaternion to rotation matrix)
	const Quaternion<double> &q = rotation;
	double aa, ab, ac, ad, bb, bc, bd, cc, cd, dd;
	aa = q.a*q.a; ab = q.a*q.b; ac = q.a*q.c; ad = q.a*q.d;
	              bb = q.b*q.b; bc = q.b*q.c; bd = q.b*q.d;
	                            cc = q.c*q.c; cd = q.c*q.d;
	                                          dd = q.d*q.d;

	double m[16] =
		{aa+bb-cc-dd, 2.0*(bc-ad), 2.0*(bd+ac),           0,
		 2.0*(bc+ad), aa-bb+cc-dd, 2.0*(cd-ab),           0,
		 2.0*(bd-ac), 2.0*(cd+ab), aa-bb-cc+dd,           0,
	        origin.x,    origin.y,    origin.z, aa+bb+cc+dd};

	glMultMatrixd(m);
}

//------------------------------------------------------------------------------

void Object::render()
{
	preRender();
	
	if (material) material->select();
	
	draw();
	
	if (material) material->unselect();
	
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

} // namespace Core

//------------------------------------------------------------------------------
