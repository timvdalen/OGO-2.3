#include <stdio.h>

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include "core.h"
#include "materials.h"
#include "hud.h"
#include "assets.h"

namespace HUD_objects{

using namespace std;
using namespace Materials;

HUD::HUD(int _width, int _height){
	resize(_width, _height);
	
	ObjectHandle mdHandle;
	mdHandle = MessageDisplayer(100, 100, -1, -1); //Don't limit size for now
	messageDisplayer = dynamic_cast<MessageDisplayer *>(&*mdHandle);
	children.insert(mdHandle);	

	ObjectHandle chHandle;
	chHandle = CrossHair(0, 0, _width, _height);
	crossHair = dynamic_cast<CrossHair *>(&*chHandle);
	children.insert(chHandle);
}

void HUD::resize(int _width, int _height){
	width = _width;
	height = _height;

	set<ObjectHandle>::iterator it;
	for(it = children.begin(); it != children.end(); ++it){
		Widget* child = dynamic_cast<Widget *>(&**it);
		if(child) child->resize(_width, _height);
	}	
}

void HUD::preRender(){
	glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        gluOrtho2D(0,viewport[2], viewport[3], 0);

        glDepthFunc(GL_ALWAYS);

	glDisable(GL_LIGHTING);
}

void HUD::postRender(){
	Object::postRender();

	glEnable(GL_LIGHTING);

	glDepthFunc(GL_LESS);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
}

Widget::Widget(int _x, int _y, int _width, int _height, MaterialHandle M = Assets::Widget)
	: Object(Pd(), Qd(), M)
{
	replace(_x, _y);
	resize(_width, _height);
}

void Widget::replace(int _x, int _y){
	xOffset = _x;
	yOffset = _y;
}

void Widget::resize(int _width, int _height){
	width = _width;
	height = _height;
}

void Widget::render(){
	glRasterPos2i(xOffset, yOffset);

	if(material) material->select();

	draw();

	if(material) material->unselect();

	glRasterPos2i(0, 0);
}

ChatMessage::ChatMessage(Core::Player _player, string _message)
	: player(_player)
{
	message = _message;
}

string ChatMessage::toString(){
	//This could be so much more efficient
	return string("<") + player.name + string(">: ") + message;
}

PlayerFragMessage::PlayerFragMessage(Core::Player _killer, Core::Player _victim)
	: killer(_killer), victim(_victim)
{}

string PlayerFragMessage::toString(){
	return string("<") + killer.name + string("> fragged <") + victim.name + string(">");
}

TowerFragMessage::TowerFragMessage(Core::Player _player)
	: player(_player)
{}

string TowerFragMessage::toString(){
	return string("<") + player.name + string("> was fragged by a tower");
}

MessageDisplayer::MessageDisplayer(int _x, int _y, int _width, int _height)
	: Widget(_x, _y, _width, _height)
{
	curr = 0;
	full = 0;
	lastMessage = glutGet(GLUT_ELAPSED_TIME);
}

void MessageDisplayer::addMessage(Message m){
	//Move head to next slot
	curr = ((curr+1)%10);
	messages[curr] = m;
	if(full < 10) full++;
	lastMessage = glutGet(GLUT_ELAPSED_TIME);
}

void MessageDisplayer::draw(){
	glRasterPos2i(xOffset, yOffset);

	
	string teststring = "Welcome to the game!";
	for(int count=0; count < teststring.length(); count++){
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, teststring[count]);
	}
}

void MessageDisplayer::render(){
	MaterialHandle font;

	int now = glutGet(GLUT_ELAPSED_TIME);
	if((now - lastMessage) > 2000){
		return;
	}else if((now - lastMessage) > 1000){
		float alpha = -(0.001)*((now-lastMessage)-2000);
		font = ColorMaterial(1.0f, 1.0f, 1.0f, alpha);
	}else{
		font = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
	}

	//Select material
	font->select();

	draw(); //Other render functions are not needed for this class

	//Unselect material
	font->unselect();
	
}

CrossHair::CrossHair(int _x, int _y, int _width, int _height)
	: Widget(_x, _y, _width, _height, Assets::CrossHair)
{}

void CrossHair::draw(){
	int midX = width/2;
	int midY = height/2;

	glBegin(GL_LINES);
		glVertex2i(midX - 15, midY);
		glVertex2i(midX - 5, midY);

		glVertex2i(midX + 5, midY);
		glVertex2i(midX + 15, midY);

		glVertex2i(midX, midY - 15);
		glVertex2i(midX, midY - 5);

		glVertex2i(midX, midY + 5);
		glVertex2i(midX, midY + 15);
	glEnd();
}

}

