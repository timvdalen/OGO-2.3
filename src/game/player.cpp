    /*
 * Player object -- see header file for more info
 */

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include "player.h"

namespace Objects {

using namespace std;

//------------------------------------------------------------------------------

Player::Player(Id _id, unsigned char _team, string _name, Pd P, Qd R, BoundingBox B)
	: BoundedObject(P, R, B)
{
	id = _id;
	team = _team;
	name = _name;

	model.head = ModelObjectContainer();
	model.body = ModelObjectContainer();
	model.weapon = ModelObjectContainer();
	model.tool = ModelObjectContainer();
	model.wheel = ModelObjectContainer();
	model.head->children.insert(Assets::Model::HeadObj);
	model.body->children.insert(Assets::Model::BodyObj);
	model.weapon->children.insert(Assets::Model::GunObj);
	model.tool->children.insert(Assets::Model::WrenchObj);
	model.wheel->children.insert(Assets::Model::WheelObj);
	children.insert(model.head);
	children.insert(model.body);
	children.insert(model.weapon);
	children.insert(model.tool);
	children.insert(model.wheel);

	//set textures
	if (team - 'a' == 0) material = ShadedMaterial(Cf(0.847,0,0,1));
	else material = ShadedMaterial(Cf(0,0,1,1));
	model.head->material = Assets::Model::HeadTex;
	model.body->material = Assets::Model::BodyTex[team-'a'];
	model.weapon->material = Assets::Model::GunTex;
	model.wheel->material = Assets::Model::WheelTex[team-'a'];
	model.tool->material = Assets::Model::WrenchTex;

	//set position of seperate elements
	velocity = Vd(0,0,0);
	update(R);

	translateModel();
}

//------------------------------------------------------------------------------

Player::~Player()
{
}

//------------------------------------------------------------------------------

inline void translate(ObjectHandle o, double x, double y, double z) {
	o->origin = o->origin + Vd(x,y,z);
}

void Player::translateModel() {
	translate(model.head,0,0,1.95);
	translate(model.body,0,0,0.3);
	translate(model.weapon,-0.499,-0.037,1.333);
	translate(model.tool,0.544,-0.037,1.333);
	translate(model.wheel,0,0,0.3);
}

//------------------------------------------------------------------------------

const Vd Player::maxVelocity = Vd(0,1,0);

void Player::update(const Qd &camobj) {
	//if (velocity.y < 1) velocity.y += 0.01;

	if (velocity.y != oldVelocity.y) {
		oldVelocity = velocity;

		rotation = Qd();
		double angle = velocity.y / maxVelocity.y * 0.15f * Pi;
		rotation = rotation * Rd(angle,Vd(1,0,0));
		
		model.weapon->rotation = -rotation;
		model.tool->rotation = -rotation;
		//model.head->rotation = -rotation;
	}
	//model.head->rotation = camobj / rotation;
	model.head->rotation = camobj * -rotation;
}

//------------------------------------------------------------------------------

void Player::draw() {
	const Pd h = model.head->origin, 
			 w = model.weapon->origin,
			 t = model.tool->origin;
	glBegin(GL_TRIANGLES);
	glNormal3f(0,0,1);
	glVertex3f(w.x, w.y+0.04, w.z);
	glVertex3f(w.x, w.y-0.04, w.z);
	glVertex3f((h.x+w.x)/2, (h.y+w.y)/2, (h.z+w.z)/2);
	glNormal3f(0,0,-1);
	glVertex3f(w.x, w.y, w.z-0.04);
	glVertex3f(w.x, w.y+0.04, w.z);
	glVertex3f((h.x+w.x)/2, (h.y+w.y)/2, (h.z+w.z)/2);
	glVertex3f(w.x, w.y-0.04, w.z);
	glVertex3f(w.x, w.y, w.z-0.04);
	glVertex3f((h.x+w.x)/2, (h.y+w.y)/2, (h.z+w.z)/2);

	glNormal3f(0,0,1);
	glVertex3f(t.x, t.y-0.04, t.z);
	glVertex3f(t.x, t.y+0.04, t.z);
	glVertex3f((h.x+t.x)/2, (h.y+t.y)/2, (h.z+t.z)/2);
	glNormal3f(0,0,-1);
	glVertex3f(t.x, t.y+0.04, t.z);
	glVertex3f(t.x, t.y, t.z-0.04);
	glVertex3f((h.x+t.x)/2, (h.y+t.y)/2, (h.z+t.z)/2);
	glVertex3f(t.x, t.y, t.z-0.04);
	glVertex3f(t.x, t.y-0.04, t.z);
	glVertex3f((h.x+t.x)/2, (h.y+t.y)/2, (h.z+t.z)/2);
	glEnd();
}

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
    
    //TODO: Color for teams, for ferry maybe use projection matrix
void Player::render(){
    glDisable(GL_LIGHTING);
	MaterialHandle teamcolor;
	if(team == 'a'){
		teamcolor = ColorMaterial(0.847f, 0.0f, 0.0f,1.0f);
	}else{
		teamcolor = ColorMaterial(0.0f, 0.0f, 1.0f,1.0f);
	}
    glPushMatrix();
		glTranslated(origin.x,origin.y,origin.z+2.2);
		glRotated(90,1,0,0);//Revert custom axis
		applyBillboarding();
		glScalef(0.003, 0.003, 0.003);    //Scale down
		double w = glutStrokeLength(GLUT_STROKE_ROMAN, 
									   reinterpret_cast<const unsigned char *> (name.c_str()));
			//glColor4d(0,1,0,1);
		teamcolor->select();
			//Translate to center
		glTranslated(-w/2,0,0);
		for(int count=0; count < name.length(); count++){
			 glutStrokeCharacter(GLUT_STROKE_ROMAN, name[count]);
		}
		teamcolor->unselect();
	glPopMatrix();
    glEnable(GL_LIGHTING);
    Object::render();
}

void Player::interpolate(){
	origin = origin + velocity*(Movement::MoveSpeed*(currentFPS()/60.0);
}

//------------------------------------------------------------------------------

} // namespace Objects

//------------------------------------------------------------------------------
