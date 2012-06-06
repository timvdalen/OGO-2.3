#include "core.h"
#include "hud.h"

namespace HUD_objects{

using namespace std;

HUD::HUD(int _width, int _height){
	resize(_width, _height);
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
	: player(_player);
{}

string TowerFragMessage::toString(){
	return string("<") + player.name + string("> was fragged by a tower");
}

}
