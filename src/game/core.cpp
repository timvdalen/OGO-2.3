/*
 * Core module -- see header
 */
#include <stdio.h>

#include <string>

#include "core.h"
#include "video.h"
#include "objects.h"
#include "player.h"
#include "structures.h"
#include "world.h"

namespace Core {

using namespace std;
using namespace Base::Alias;
using namespace Objects;

Object::Constructor::List Object::Constructor::list;

REGISTER(Object)
REGISTER(BoundedObject)
REGISTER(Player)
REGISTER(Structure)
REGISTER(Mine)
REGISTER(Building)
REGISTER(HeadQuarters)
REGISTER(DefenseTower)
REGISTER(ResourceMine)
REGISTER(Droppable)

//------------------------------------------------------------------------------

string convert(const Point<double> &p) { char buffer[1024];
	sprintf(buffer, "P%f,%f,%f", p.x, p.y, p.z); return string(buffer); }

string convert(const Vector<double> &v) { char buffer[1024];
	sprintf(buffer, "V%f,%f,%f", v.x, v.y, v.z); return string(buffer); }

string convert(const Quaternion<double> &q) { char buffer[1024];
	sprintf(buffer, "Q%f,%f,%f,%f", q.a, q.b, q.c, q.d); return string(buffer); }

string convert(const Rotation<double> &r) { char buffer[1024];
	sprintf(buffer, "R%f,%f,%f,%f", r.a, r.v.x, r.v.y, r.v.z); return string(buffer); }

string convert(const Color<double> &c) { char buffer[1024];
	sprintf(buffer, "C%f,%f,%f,%f", c.r, c.g, c.b, c.a); return string(buffer); }

string convert(const double &d) { char buffer[64];
	sprintf(buffer, "F%f", d); return string(buffer); }

string convert(const long &l) { char buffer[32];
	sprintf(buffer, "I%d", l); return string(buffer); }

//------------------------------------------------------------------------------

Point<double> ToPoint(const string &str) { Point<double> p;
	sscanf(str.c_str(), "P%lf,%lf,%lf", &p.x, &p.y, &p.z); return p; }

Vector<double> ToVector(const string &str) { Vector<double> v;
	sscanf(str.c_str(), "V%lf,%lf,%lf", &v.x, &v.y, &v.z); return v; }

Quaternion<double> ToQuaternion(const string &str) { Quaternion<double> q;
	sscanf(str.c_str(), "Q%lf,%lf,%lf,%lf", &q.a, &q.b, &q.c, &q.d); return q; }

Rotation<double> ToRotation(const string &str) { Rotation<double> r = Rd(0, Vd(0,0,1));
	sscanf(str.c_str(), "R%lf,%lf,%lf,%lf", &r.a, &r.v.x, &r.v.y, &r.v.z); return r; }

Color<float> ToColor(const string &str) { Color<float> c;
	sscanf(str.c_str(), "C%lf,%lf,%lf,%lf", &c.r, &c.g, &c.b, &c.a); return c; }

double ToFloat(const string &str) { double d = 0;
	sscanf(str.c_str(), "F%lf", &d); return d; }

long ToInteger(const string &str) { long l = 0;
	sscanf(str.c_str(), "I%ld", &l); return l; }

//------------------------------------------------------------------------------

void Object::updateAbsolute()
{
	if (children.size() < 1) return;
	
	Pd pos = absoluteOrigin();
	Qd rot = absoluteRotation();
	set<ObjectHandle>::iterator it;
	for (it = children.begin(); it != children.end(); ++it)
	{
		(*it)->parentOrigin = pos;
		(*it)->parentRotation = rot;
	}
}

//------------------------------------------------------------------------------

void Object::preRender()
{
	// Camera orientation and translation (quaternion to rotation matrix)
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
	
	glPushMatrix();
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

bool Object::unserialize(const string &str)
{
	vector<string> arg;
	size_t left, right;
	for (left = 0, right = str.find("|", 0); right != string::npos;
	right = str.find("|", right))
	{
		arg.push_back(str.substr(left, right - left));
		left = ++right;
	}
	arg.push_back(str.substr(left));
	return unserialize(arg);
}

//------------------------------------------------------------------------------

ObjectHandle Object::construct(string str)
{
	size_t right = str.find("|", right);
	if (right != string::npos) str.resize(right);
	if (!Constructor::list.count(str)) return Object();
	return Constructor::list[str]();
}

//------------------------------------------------------------------------------

} // namespace Core

//------------------------------------------------------------------------------
