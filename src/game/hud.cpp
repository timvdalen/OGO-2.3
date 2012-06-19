#include <stdio.h>

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif
#include <sstream>
#include "core.h"
#include "materials.h"
#include "hud.h"
#include "assets.h"
#include "game.h"

namespace HUD_objects {

using namespace std;
using namespace Materials;
using namespace Objects;

//------------------------------------------------------------------------------

int modulo(int a, int b)
{
    int r = a % b;
    if (r < 0) r += b;
    return r;
}

//------------------------------------------------------------------------------

HUD::HUD(int _width, int _height){
	resize(_width, _height);
	ObjectHandle mdHandle;
	mdHandle = MessageDisplayer(100, 100, -1, -1); //Don't limit size for now
	messageDisplayer = TO(MessageDisplayer,mdHandle);
	children.insert(mdHandle);	
    
    ObjectHandle sdHandle;
    sdHandle = StatusDisplayer(50,50, _width, _height);
    statusDisplayer = TO(StatusDisplayer,sdHandle);
	children.insert(sdHandle);	
    
	#ifndef AUTOBUILD
    ObjectHandle bsHandle;
    bsHandle = BuildingSelection(0,100, _width, _height);
    buildselector = TO(BuildingSelection,bsHandle);
	children.insert(bsHandle);	
    #endif
	
	ObjectHandle chHandle;
	chHandle = CrossHair(0, 0, _width, _height);
	crossHair = TO(CrossHair, chHandle);
	children.insert(chHandle);
    
    ObjectHandle mmHandle;
    mmHandle = MiniMap(40,40, _width, _height);
    children.insert(mmHandle);
}

//------------------------------------------------------------------------------

void HUD::resize(int _width, int _height){
	width = _width;
	height = _height;

	set<ObjectHandle>::iterator it;
	for(it = children.begin(); it != children.end(); ++it){
		Widget* child = dynamic_cast<Widget *>(&**it);
		if(child) child->resize(_width, _height);
	}	
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void HUD::postRender(){
	Object::postRender();

	glEnable(GL_LIGHTING);

	glDepthFunc(GL_LESS);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
}

//------------------------------------------------------------------------------

Widget::Widget(int _x, int _y, int _width, int _height, MaterialHandle M = Assets::Widget)
	: Object(Pd(), Qd(), M)
{
	replace(_x, _y);
	resize(_width, _height);
}

//------------------------------------------------------------------------------

void Widget::replace(int _x, int _y){
	xOffset = _x;
	yOffset = _y;
}

//------------------------------------------------------------------------------

void Widget::resize(int _width, int _height){
	width = _width;
	height = _height;
}

//------------------------------------------------------------------------------

void Widget::render(){
	glRasterPos2i(xOffset, yOffset);

	if(material) material->select();

	draw();

	if(material) material->unselect();

	glRasterPos2i(0, 0);
}

//------------------------------------------------------------------------------

ChatMessage::ChatMessage(const Player &_player, string _message)
	: player(_player)
{
	message = _message;
}

//------------------------------------------------------------------------------

string ChatMessage::toString(){
	//This could be so much more efficient
	return string("<") + player.name + string(">: ") + message;
}

//------------------------------------------------------------------------------

PlayerFragMessage::PlayerFragMessage(const Player &_killer, const Player &_victim)
	: killer(_killer), victim(_victim)
{}

//------------------------------------------------------------------------------

string PlayerFragMessage::toString(){
	return string("<") + killer.name + string("> fragged <") + victim.name + string(">");
}

//------------------------------------------------------------------------------

TowerFragMessage::TowerFragMessage(const Player &_player)
	: player(_player)
{}

//------------------------------------------------------------------------------

string TowerFragMessage::toString(){
	return string("<") + player.name + string("> was fragged by a tower");
}

//------------------------------------------------------------------------------

SystemMessage::SystemMessage(string _message)
	: message(_message)
{}

//------------------------------------------------------------------------------

string SystemMessage::toString(){
	return string("** ") + message + string(" **");
}

//------------------------------------------------------------------------------

MessageDisplayer::MessageDisplayer(int _x, int _y, int _width, int _height)
	: Widget(_x, _y, _width, _height)
{
	curr = 0;
	full = 0;
	showAlways = false;
	lastMessage = ElapsedTime();
}

//------------------------------------------------------------------------------

void MessageDisplayer::addMessage(Handle<DisplayMessage> m){
	messages[curr] = m;
	//Move head to next slot
	curr = ((curr+1)%10);
	if(full < 10) full++;
	lastMessage = ElapsedTime();
}

//------------------------------------------------------------------------------

void MessageDisplayer::setShowAlways(bool show){
	if(!show){
		lastMessage = ElapsedTime() - 1000;
	}
	showAlways = show;
}

//------------------------------------------------------------------------------

bool MessageDisplayer::getShowAlways(){
	return showAlways;
}

//------------------------------------------------------------------------------

void MessageDisplayer::draw(){
	glRasterPos2i(xOffset, yOffset);
	
	//Loop through all messages, starting with the first added one
	//up to the last added one
	for(int i=0; i < 10; i++){
		int ai = (curr+i)%10;
		Handle<DisplayMessage> mh = messages[ai];
		if(!mh)	continue;
		DisplayMessage *m = &*mh;
		if(!m) continue;
		string message = m->toString();
		glRasterPos2i(xOffset, yOffset + (18*i));
		for(int count=0; count < message.length(); count++){
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, message[count]);
		}
	}
}

//------------------------------------------------------------------------------

void MessageDisplayer::render(){
	MaterialHandle font;

	if(showAlways){
		font = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
	}else{
		int now = ElapsedTime();
		if((now - lastMessage) > 2000){
			return;
		}else if((now - lastMessage) > 1000){
			float alpha = -(0.001)*((now-lastMessage)-2000);
			font = ColorMaterial(1.0f, 1.0f, 1.0f, alpha);
		}else{
			font = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
	
	//Select material
	font->select();

	draw(); //Other render functions are not needed for this class

	//Unselect material
	font->unselect();
	
}

//------------------------------------------------------------------------------

CrossHair::CrossHair(int _x, int _y, int _width, int _height)
	: Widget(_x, _y, _width, _height, Assets::CrossHair)
{}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
    
    
StatusDisplayer::StatusDisplayer(int _x, int _y, int _width, int _height) : Widget(_x, _y, _width, _height)
    {
    }
    
//To scale the line thickness with the size of the satus displayer
inline int scale(int linewidth, float scale){
	int result = (int)(scale*linewidth + 0.5);
	return result < 1 ? 1 : result;
}
    
    //------------------------------------------------------------------------------
    //Should be splitted in subroutintes, to lazy atm
    //Using the x and y coordinates can be done (and is not done)
    //by using render and translating..., width and height can be scaled...
    //Maybe also not use select, but that is a lot of work to revamp
    //and only results in more beautifull code
    void StatusDisplayer::draw(){
        float scalar = min(width, height);
        scalar = scalar/800.;
		scalar = scalar > 1? 1 : scalar;
        glRasterPos2i(0,0);
        //Cache them somewere?
       // MaterialHandle bg = ColorMaterial(254.0/255.0, 251.0/255, 225.0/255.0,0.8f);
		MaterialHandle bg = ColorMaterial(20.0/255.0, 20.0/255, 20.0/255.0,0.8f);
        MaterialHandle white = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
        MaterialHandle black = ColorMaterial(1.0f,1.0f,1.0f,1.0f);
        MaterialHandle darkred = ColorMaterial(138.0/255.0,24.0/255.0,18.0/255.0,1.0);
        MaterialHandle red = ColorMaterial(229.0/255.0,37.0/255.0,32.0/255.0,1.0);
        MaterialHandle darkgreen = ColorMaterial(19.0/255.0,51.0/255.0,36.0/255.0, 1.0);
        MaterialHandle green = ColorMaterial(156.0/255.0,202.0/255.0,135.0/255.0, 1.0);
        bg->select();
        //Draw background
        glBegin(GL_QUADS);
        glVertex2i(5,5);
        glVertex2i(5,105);
        glVertex2i(205,105);
        glVertex2i(205,5);
        glEnd();
        bg->unselect();
        //draw table lines
        black->select();
        glBegin(GL_LINE_STRIP);
        glVertex2i(5,5);
        glVertex2i(5,105);
        glVertex2i(205,105);
        glVertex2i(205,5);
        glVertex2i(5,5);
        glEnd();
        glBegin(GL_LINES);
        glVertex2i(60, 5);
        glVertex2i(60,105);
        glVertex2i(5, 55);
        glVertex2i(205,55);
        glEnd();
        black->unselect();
        //Draw money icon
        Assets::Icon::Money->select();
        glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex2i(10,10);
        glTexCoord2f(0,0);
        glVertex2i(10,50);
        glTexCoord2f(1,0);
        glVertex2i(50,50);
        glTexCoord2f(1,1);
        glVertex2i(50,10);
        glEnd();
        Assets::Icon::Money->unselect();
        //Draw Health icon
        Assets::Icon::Health->select();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex2i(10,60);
        glTexCoord2f(0,1);
        glVertex2i(10,100);
        glTexCoord2f(1,1);
        glVertex2i(50,100);
        glTexCoord2f(1,0);
        glVertex2i(50,60);
        glEnd();
        Assets::Icon::Health->unselect();
        //Draw amount of money
        black->select();
        stringstream ss;
		int resources;
		map<unsigned char,Team>::iterator it = Game::game.teams.find(Game::game.player->team);
		if(it != Game::game.teams.end()){
			resources = it->second.resources;
		}else{
			resources = 0;
		}
        ss << resources << "f";
		string money = ss.str();
        glPushMatrix();
        glTranslatef(65,45,0);
        glScalef(0.25,-0.25,1);
		glRasterPos2i(10, 15);
        glLineWidth(scale(2, scalar));
		for(int count=0; count < money.length(); count++){
			glutStrokeCharacter(GLUT_STROKE_ROMAN, money[count]);
		}
        glPopMatrix();
        black->unselect();
        //Draw Health-bar
        black->select();
        glLineWidth(scale(5, scalar));
        glBegin(GL_LINE_STRIP);
        glVertex2i(70,65);
        glVertex2i(70,95);
        glVertex2i(170,95);
        glVertex2i(170,65);
        glVertex2i(70,65);
        glEnd();
        //round corners
        glLineWidth(scale(2, scalar));
        glBegin(GL_LINE_STRIP);
        glVertex2i(69,64);
        glVertex2i(69,96);
        glVertex2i(171,96);
        glVertex2i(171,64);
        glVertex2i(69,64);
        glEnd();
        black->unselect();
        //red-background healthbar
        red->select();
        glBegin(GL_QUADS);
        glVertex2i(72,67);
        glVertex2i(72,93);
        glVertex2i(168,93);
        glVertex2i(168,67);
        glEnd();
        red->unselect();
        darkred->select();
        glBegin(GL_QUADS);
        glVertex2i(75,70);
        glVertex2i(75,90);
        glVertex2i(165,90);
        glVertex2i(165,70);
        glEnd();
        darkred->unselect();
        //draw-live health bar
        double factor = ((double)Game::game.player->health)/((double)Game::game.player->maxHealth);
        green->select();
        glBegin(GL_QUADS);
        glVertex2i(72,67);
        glVertex2i(72,93);
        glVertex2i((int) (72 + 96*factor), 93);
        glVertex2i((int) (72 + 96*factor), 67);
        glEnd();
        green->unselect();
        if(96*factor > 6){//The rectangle has not been degenerated
            darkgreen->select();
            glBegin(GL_QUADS);
            glVertex2i(75,70);
            glVertex2i(75,90);
            glVertex2i((int) (69 + 96*factor),90);
            glVertex2i((int) (69 + 96*factor),70);
            glEnd();
            darkgreen->unselect();
        }
        //Draw line to distinct red part from green part
        if(3 < 96*factor && 96*factor < 93){//Dont overwrite the green part with a line
            glLineWidth(scale(2, scalar));
            black->select();
            glBegin(GL_LINES);
            glVertex2i((int) (72 + 96*factor), 93);
            glVertex2i((int) (72 + 96*factor), 67);
            glEnd();
            black->unselect();
        }
        glLineWidth(1);
    }
    
    void StatusDisplayer::render(){
        //Set to the right coordinates
        float scale = min(width, height);
        scale = scale/800.;
        glPushMatrix();
		glScalef(scale, scale,1);
        //Set to the right coordinates
        //-10 because the draw() method has as upperright corner (10,10)
        //If one wants to subtract minus 10 from all the vertices
        //the statusbar would be as it should be..
        glTranslatef(xOffset-10,yOffset-10,0);
        //TODO scaling with width and height (is this actually needed)?
        draw();
        glPopMatrix();
        
    }
    //------------------------------------------------------------------------------
    
    
    BuildingSelection::BuildingSelection(int _x, int _y, int _width, int _height) : Widget(_x, _y, _width, _height)
    {
        selected = 1;
        show = false;
    }
    
    
    //------------------------------------------------------------------------------
    void BuildingSelection::draw(){
        //TODO link this with the interface
        MaterialHandle icons[2] = {
            Assets::Icon::Pickaxe_normal,Assets::Icon::Tower_normal
        };
        string buildingname[2] = {"Mine", "Tower"};
        int n = 2;
        //Init ended, real code started:
        glTranslatef(-185,-n*70,0);
        //MaterialHandle bgselected = ColorMaterial(255.0/255.0, 255.0/255.0, 190.0/255.0,1.0f);
		MaterialHandle bgselected = ColorMaterial(20.0/255.0, 20.0/255.0, 20.0/255.0,1.0f);
		MaterialHandle bgunselected = ColorMaterial(50.0/255.0, 50.0/255.0, 50.0/255.0, 0.8f);
//        MaterialHandle bgunselected = ColorMaterial(254.0/255.0, 251.0/255.0, 225.0/255.0, 0.8f);
        MaterialHandle black = ColorMaterial(1.0f,1.0f,1.0f,1.0f);
        int i;
        for(i = 0; i < n; i++){
            //The background
            if(selected == i + 1){
                bgselected->select();
            }else{
                bgunselected->select();
            }
            glBegin(GL_QUADS);
            glVertex2i(0,0);
            glVertex2i(0, 70);
            glVertex2i(370, 70);
            glVertex2i(370,0);
            glEnd();
            if(selected == i + 1){
                bgselected->unselect();
            }else{
                bgunselected->unselect();
            }
            //The table lines
            black->select();
            glBegin(GL_LINES);
            //Top-line
            glVertex2i(0,0);
            glVertex2i(370,0);
            //left-edge
            glVertex2i(0,0);
            glVertex2i(0,70);
            //icon-text separator
            glVertex2i(70,0);
            glVertex2i(70,70);
            //right-edge        
            glVertex2i(370,0);
            glVertex2i(370,70);
            glEnd();
            black->unselect();
            //The icon
            icons[i]->select();
            glBegin(GL_QUADS);
            glTexCoord2f(0,1);
            glVertex2i(10,10);
            glTexCoord2f(0,0);
            glVertex2i(10,60);
            glTexCoord2f(1,0);
            glVertex2i(60,60);
            glTexCoord2f(1,1);
            glVertex2i(60,10);
            glEnd();
            icons[i]->unselect();
            //The text
            black->select();
            glPushMatrix();
                glTranslatef(80,40,0);
                glScalef(0.15,-0.15,1);
                glLineWidth(1);
                stringstream ss;
                ss << "Press '" << (i + 1) << "' to build a " << buildingname[i]
                << ".";
                string msg = ss.str();
                for(int count=0; count < msg.length(); count++){
                    glutStrokeCharacter(GLUT_STROKE_ROMAN, msg[count]);
                }
            glPopMatrix();
            glTranslatef(0,70,0);
        }
        //The last table lines
        black->select();
        glBegin(GL_LINES);
        glVertex2i(0,0);
        glVertex2i(370,0);
        glEnd();
        black->unselect();
}
    
    void BuildingSelection::render(){
        if(show){
        glPushMatrix();
        //Set to the right coordinates
        glTranslatef(width/2+xOffset,height-yOffset,0);
        draw();
        glPopMatrix();
        }
  
}
    //------------------------------------------------------------------------------
    
    
    MiniMap::MiniMap(int _x, int _y, int _width, int _height) : Widget(_x, _y, _width, _height)
    {
    }
	

void drawStructure(GridPoint p, ObjectHandle s, int xspacing, int yspacing){
	Building *b = TO(Building, s);
	Player* player = 0;
	if(b && Game::game.players.count(b->owner)){
		player = TO(Player, Game::game.players[b->owner]);
	}
	double enlargedown = 0;
	double enlargeup = 0;
	MaterialHandle mat;
	float progress;
	//TODO team recognition.
	if(TO(DefenseTower, s)){
		progress = (float)(Video::ElapsedTime()-b->buildTime)/b->buildDuration;
		if(player){
			if(player->team == 'a'){
				mat = Assets::Icon::Tower_red;
			}else{
				mat = Assets::Icon::Tower_blue;
			}
		}else{
			mat = Assets::Icon::Tower_normal;
		}
    }else if(TO(Mine, s)){
		enlargeup = 0;
		enlargedown = 0;
		progress = 1;
		mat = Assets::Icon::Mine;
	}else if(TO(ResourceMine, s)){
		enlargeup = 0;
		enlargedown = 0;
		progress = (float)(Video::ElapsedTime()-b->buildTime)/b->buildDuration;
		if(player){
			if(player->team == 'a'){
				mat = Assets::Icon::Pickaxe_red;
			}else{
				mat = Assets::Icon::Pickaxe_blue;
			}
		}else{
			mat = Assets::Icon::Pickaxe_normal;
		}
	}else if(TO(HeadQuarters, s)){
		enlargeup = 0;
		enlargedown = 1;
		progress = (float)(Video::ElapsedTime()-b->buildTime)/b->buildDuration;
		if(player){
			if(player->team == 'a'){
				mat = Assets::Icon::HQ_red;
			}else{
				mat = Assets::Icon::HQ_blue;
			}
		}else{
			mat = Assets::Icon::HQ_normal;
		}
	}
	if(mat){
		mat->select();
		glColor4f(1.0f,1.0f,1.0f,0.7f*progress+0.3f);
		glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2i((p.x-enlargedown)*xspacing,(p.y-enlargedown)*yspacing);
		glTexCoord2f(0,0);
		glVertex2i((p.x-enlargedown)*xspacing,(p.y+1+enlargeup)*yspacing);
		glTexCoord2f(1,0);
		glVertex2i((p.x+1+enlargeup)*xspacing, (p.y+1+enlargeup)*yspacing);
		glTexCoord2f(1,1);
		glVertex2i((p.x+1+enlargeup)*xspacing, (p.y-enlargedown)*yspacing);
		glEnd();
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		mat->unselect();
	}
}
    //------------------------------------------------------------------------------
    void drawPlayer(Player *p, int xspacing, int yspacing, World* w, bool marked){
        float relx = ((w->width/GRID_SIZE)*(p->origin.x + w->width/2.0)/(w->width));
        float rely = ((w->height/GRID_SIZE)*(p->origin.y + w->height/2.0)/(w->height));
        if(marked){
            Assets::Icon::Robot_normal->select();
        }else if(p->team =='a'){
            Assets::Icon::Robot_red->select();
        }else{
            Assets::Icon::Robot_blue->select();
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex2i((int)(relx*xspacing),(int)(rely*yspacing));
        glTexCoord2f(0,0);
        glVertex2i((int)(relx*xspacing),(int)((rely+1)*yspacing));
        glTexCoord2f(1,0);
        glVertex2i((int)((relx+1)*xspacing), (int)((rely+1)*yspacing));
        glTexCoord2f(1,1);
        glVertex2i((int)((relx+1)*xspacing), (int)((rely)*yspacing));
        glEnd();
        if(marked){
            Assets::Icon::Robot_normal->unselect();
        }else if(p->team == 'a'){
            Assets::Icon::Robot_red->unselect();
        }else{
            Assets::Icon::Robot_blue->unselect();
        }
    }
    
    
//------------------------------------------------------------------------------
void MiniMap::draw()
{
	World *w = TO(World,Game::game.world);
	if (!w) return;
	
	//MaterialHandle bg = ColorMaterial(156.0/255.0, 202.0/255.0, 135.0/255.0, 0.8f);
	MaterialHandle bg = ColorMaterial(20.0/255.0, 20.0/255.0, 20.0/255.0, 0.8f);
    MaterialHandle black = ColorMaterial(1.0f,1.0f,1.0f,1.0f);
    if(w->width > w->height){
		glScalef(1.0f, w->height/w->width,1.0f);
	}else{
		glScalef(w->width/w->height,1.0f,1.0f);
	}
	bg->select();
	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(0,320);
	glVertex2i(320,320);
	glVertex2i(320,0);
	glEnd();
	bg->unselect();
	black->select();
	glBegin(GL_LINE_STRIP);
	glVertex2i(0,0);
	glVertex2i(0,320);
	glVertex2i(320,320);
	glVertex2i(320,0);
	glVertex2i(0,0);
	glEnd();
	black->unselect();
	//The buildings
	map<GridPoint, ObjectHandle> &structs = w->terrain->structures;
	map<GridPoint, ObjectHandle>::iterator itt;
	int xspacing = (int) (320.0 / (w->width / GRID_SIZE));
	int yspacing = (int) (320.0 / (w->height / GRID_SIZE));
	for(itt = structs.begin(); itt != structs.end(); itt++)
	{
		if (!itt->second) continue;
		GridPoint p = itt->first;
		ObjectHandle s = itt->second;
		drawStructure(p, s, xspacing, yspacing);
	}
	xspacing = (int) (320.0 / (w->width / GRID_SIZE + 1));
	yspacing = (int) (320.0 / (w->height / GRID_SIZE + 1));
	//The robots
	map<Player::Id,ObjectHandle>::iterator it;
	for (it = Game::game.players.begin(); it != Game::game.players.end(); ++it)
	{
		Player *p = TO(Player,it->second);
		if(!p){
			return;
		}
		drawPlayer(p,xspacing,yspacing,w,false);
	}        
    drawPlayer(Game::game.player, xspacing, yspacing, w, true);
}
    
    
    void MiniMap::render(){
        //scale width
            glPushMatrix();
            //Set to the right coordinates
            float scale = min(width, height);
            scale = scale/1000.0;
            glTranslatef(width,0,0);
            glScalef(scale,scale,0);
            glTranslatef(-320 - xOffset,yOffset,0);
            draw();
            glPopMatrix();
    }
    
    
//------------------------------------------------------------------------------

TextInput::TextInput(Input *_input, int _x, int _y, int _width, int _height)
	: Widget(_x, _y, _width, _height, Assets::WidgetBG), input(_input)
{
	done = false;
	input->grabText();
}

//------------------------------------------------------------------------------

void TextInput::draw(){

	string buffer = input->text;
	//TODO: This should be implemented in Widget::preRender() and Widget::postRender()
	glPushMatrix();
		int maxchars = (int) ((0.36 * width) / 9);
		int noLines = buffer.size()/maxchars;
		glTranslatef(xOffset, yOffset, 0);
		glBegin(GL_QUADS);
			glVertex2f(0.30*width, 0.45*height);
			glVertex2f(0.30*width, 0.55*height + noLines*15);
			glVertex2f(0.70*width, 0.55*height + noLines*15);
			glVertex2f(0.70*width, 0.45*height);
		glEnd();
		Assets::Font->select();
		glRasterPos2i((int) (xOffset + 0.32 * width), (int) (yOffset + 0.50 * height));
		
		int curchars = 0;
		int line = 0;
		for(int count=0; count < buffer.length(); count++){
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[count]);
			curchars++;
			if(curchars > maxchars){
				line++;
				curchars = 0;
				glRasterPos2i((int) (xOffset + 0.32 * width), (int) (yOffset + 0.50 * height + 15 * line));
			}
		}
		Assets::Font->unselect();
	glPopMatrix();
}
    


//------------------------------------------------------------------------------

} // namespace HUD_objects

//------------------------------------------------------------------------------
