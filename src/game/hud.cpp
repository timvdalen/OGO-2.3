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

namespace HUD_objects{

using namespace std;
using namespace Materials;

HUD::HUD(int _width, int _height){
	resize(_width, _height);
	children.insert(MessageDisplayer(100, 100));
	messageDisplayer = dynamic_cast<MessageDisplayer *>(&**children.begin());
}

void HUD::resize(int _width, int _height){
	width = _width;
	height = _height;
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
}

void HUD::postRender(){
	Object::postRender();

	glDepthFunc(GL_LESS);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
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

MessageDisplayer::MessageDisplayer(int _x, int _y){
	x = _x;
	y = _y;
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
	glRasterPos2i(x, y);


	ShadedMaterial *textColor;


	int now = glutGet(GLUT_ELAPSED_TIME);
	if((now - lastMessage) > 2000){
		return;
	}else if((now - lastMessage) > 1000){
		textColor = new ShadedMaterial(Cf(1, 1, 1, -(1/1000)*((now-lastMessage)-2000)));
	}else{
		textColor = new ShadedMaterial(Cf(1, 1, 1, 1));
	}
	textColor = new ShadedMaterial(Cf(1, 1, 1, 1));
	textColor->select();

	

	string helpstring = "q: Gear down";
	for(int count=0; count < helpstring.length(); count++){
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, helpstring[count]);
	}
}

void MessageDisplayer::render(){
	draw(); //Other render functions are not needed for this class
}

}
