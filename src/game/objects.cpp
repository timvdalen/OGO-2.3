/*
 * Objects -- see header
 */
#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include <stack>
#include <limits>
#include <stdio.h>

#include "objects.h"
#include "structures.h"
#include "video.h"
#include "assets.h"

namespace Objects {

REGISTER(BoundedObject,)

//------------------------------------------------------------------------------

void applyBillboarding(){
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX , m);
        //set look and up-vector to identity vertices
    float billBoardedMatrix[16] = { 1   ,     0,     0,   m[3],
                                    m[4]   ,  m[5], m[6],   m[7],
                                       0,     0,     1,  m[11],
                                   m[12], m[13], m[14],  m[15]};
    glLoadMatrixf(billBoardedMatrix);     
}

//------------------------------------------------------------------------------

bool Destroyable::isDestroyed(){
	return !(maxHealth > 0 && health > 0);
}

//------------------------------------------------------------------------------

void Destroyable::damage(double dmg){
	health -= dmg;
}

//------------------------------------------------------------------------------

void Destroyable::heal(double _health){
	health += health;
}

//------------------------------------------------------------------------------

void Destroyable::fullHeal(){
	health = maxHealth;
}

//------------------------------------------------------------------------------

pair<ObjectHandle,double> BoundedObject::checkCollision(Pd origin, Vd direction)
{
    double collision = numeric_limits<double>::infinity();
    //--- We only check lbl rbh, could be improved-----
    Point<double> a = bb.lbl;
    Point<double> b = bb.rth;
    //--- Origin does not have to be rotated
    Point<double> p = (-rotation)*(origin - this->origin);
    //--- Vector only needs to be rotated
    //--- We might need the inverse of rotation here
    Vector<double> v = (-rotation)*direction;
    //--- Now check if we have a collision in the x direction
    double lambda1, lambda2;
    if(v.x != 0){
        lambda1 = (a.x - p.x)/(v.x);
        lambda2 = (b.x - p.x)/(v.x);
        if(insideBox(p + v*lambda1, a, b) && 0 < lambda1 && lambda1 < collision){
            collision = lambda1;
        }
        if (insideBox(p + v*lambda2, a, b) && 0 < lambda2 && lambda2 < collision){
            collision = lambda2;
        }
    }
    //---- y direction
    if(v.y != 0){
        lambda1 = (a.y - p.y)/(v.y);
        lambda2 = (b.y - p.y)/(v.y);
        if(insideBox(p + v*lambda1, a, b) && 0 < lambda1 && lambda1 < collision){
            collision = lambda1;
        }
        if(insideBox(p + v*lambda2,a,b) && 0 < lambda2 && lambda2 < collision){
            collision = lambda2;
        }
    }
    //--- z direction
    if(v.z != 0){
        lambda1 = (a.z - p.z)/(v.z);//intersection with axis in lbl.z
        lambda2 = (b.z - p.z)/(v.z);
        if(insideBox(p + v*lambda1, a, b) && 0 < lambda1 && lambda1 < collision){
            collision = lambda1;
        }
        if(insideBox(p + v*lambda2, a, b) && 0 < lambda2 && lambda2 < collision){
            collision = lambda2;
        }
    }
    return make_pair(ObjectHandle(),collision);
}

//------------------------------------------------------------------------------

ObjectHandle BoundedObject::checkCollision2(Pd origin, Vd direction)
{
    return checkCollision(origin,direction).first;
}

//------------------------------------------------------------------------------

bool BoundedObject::checkCollision(const ObjectHandle &target)
{
	BoundedObject *bo = TO(BoundedObject, target);
	if (bo)
	{
		// Todo: implement
	}
	else return insideBox(target->origin, bb.lbl, bb.rth);
}

//------------------------------------------------------------------------------

bool BoundedObject::insideBox(Point<double> p, Point<double> a, Point<double> b){
        return a.x - 0.01 <= p.x && p.x <= b.x + 0.01//Inside x-interval
            && a.y - 0.01 <= p.y && p.y <= b.y + 0.01//Inside y-interval
            && a.z - 0.01 <= p.z && p.z <= b.z + 0.01;//Inside z-interval
}

//------------------------------------------------------------------------------

void ModelObjectContainer::render()
{
	preRender();

	if (material) material->select();

	draw();

	postRender();

	if (material) material->unselect();
}

//------------------------------------------------------------------------------

LaserBeam::LaserBeam(Pd _origin, Qd _direction, double _collision, int _fireTime, int _ttl)
	: Object(_origin), direction(_direction), collision(_collision)
{
	fireTime = _fireTime;
	ttl = _ttl;
	done = false;
}

//------------------------------------------------------------------------------

void LaserBeam::preRender(){
	int timelived = Video::ElapsedTime() - fireTime;
	if(timelived >= ttl){
		done = true;
		return;
	}
	MaterialHandle lm;
	if(timelived < 0.33*ttl){
		//Fade in
		float alpha = ((1.0/ttl)/0.33)*timelived;
		lm = TwinMaterial(LineMaterial(1),
				ShadedMaterial(Cf(0.2,0.8,0.2,alpha), //Ambient
							   Cf(0.2,0.8,0.2,alpha), //Diffuse
							   Cf(0.2,0.8,0.2,alpha), //Specular
							   Cf(0.8,1,0,alpha),     //Emissive
							   100.0));           //Shininess
	}else if(timelived < 0.66*ttl){
		lm = TwinMaterial(LineMaterial(2),
				ShadedMaterial(Cf(0.2,0.8,0.2,1), //Ambient
							   Cf(0.2,0.8,0.2,1), //Diffuse
							   Cf(0.2,0.8,0.2,1), //Specular
							   Cf(0.8,1,0,1),     //Emissive
							   100.0));           //Shininess
	}else{
		//Fade out
		float alpha = ((-1*(1.0/ttl)/0.33)*timelived + 3);
		lm = TwinMaterial(LineMaterial(1),
				ShadedMaterial(Cf(0.2,0.8,0.2,alpha), //Ambient
							   Cf(0.2,0.8,0.2,alpha), //Diffuse
							   Cf(0.2,0.8,0.2,alpha), //Specular
							   Cf(0.8,1,0,alpha),     //Emissive
							   100.0));           //Shininess
	}

	material = lm;

	Object::preRender();
}

//------------------------------------------------------------------------------

void LaserBeam::draw(){
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		Vd dir = -direction*Vd(0, 1, 0);
		Vd endpoint;
		if (collision < 38.0)
		{
			endpoint = ~dir*collision;
		}
		else
		{
			endpoint = ~dir*38;
		}
		glVertex3f(endpoint.x, endpoint.y, endpoint.z);
	glEnd();

	if (collision < 38.0)
	{
		glPushMatrix();
		glTranslatef(endpoint.x, endpoint.y, endpoint.z);
		glutSolidSphere(0.1, 15, 15);
		glPopMatrix();
	}
}

//------------------------------------------------------------------------------

Cuboid::Cuboid(Pd origin, double S)
	: BoundedObject(origin, Qd(), BoundingBox(), Assets::Test),
	  u(Vd(S,0,0)),
	  v(Vd(0,S,0)),
	  w(Vd(0,0,S))
{
	bb.lbl = origin         ;
	bb.rbl = origin +u      ;
	bb.ltl = origin    +v   ;
	bb.rtl = origin +u +v   ;
	bb.lbh = origin       +w;
	bb.rbh = origin +u    +w;
	bb.lth = origin    +v +w;
	bb.rth = origin +u +v +w;
}

//------------------------------------------------------------------------------

void Cuboid::draw()
{
	#define Vert(v) { Pd vt = (v); glVertex3d(vt.x,vt.y,vt.z); }
	#define Norm(u,v) { Vd n = ~((u)*(v)); glNormal3d(n.x,n.y,n.z); }
	#define A { glTexCoord2d(0.0,0.0); }
	#define B { glTexCoord2d(1.0,0.0); }
	#define C { glTexCoord2d(1.0,1.0); }
	#define D { glTexCoord2d(0.0,1.0); }

	Pd o = Vd(0,0,0);
	Pd a = o + u + v + w;

	glBegin(GL_QUADS);
		Norm(u,w); A Vert(o); B Vert(o+u); C Vert(o+u+w); D Vert(o+w);
		Norm(v,u); A Vert(o); B Vert(o+v); C Vert(o+v+u); D Vert(o+u);
		Norm(w,v); A Vert(o); B Vert(o+w); C Vert(o+w+v); D Vert(o+v);
		Norm(u,v); A Vert(a); B Vert(a-u); C Vert(a-u-v); D Vert(a-v);
		Norm(v,w); A Vert(a); B Vert(a-v); C Vert(a-v-w); D Vert(a-w);
		Norm(w,u); A Vert(a); B Vert(a-w); C Vert(a-w-u); D Vert(a-u);
	glEnd();

	#undef Vert
	#undef Norm
	#undef A
	#undef B
	#undef C
	#undef D
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
