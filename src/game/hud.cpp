#include "hud.h"

namespace HUD_objects{

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

}
