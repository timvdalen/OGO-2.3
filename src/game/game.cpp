#include <wx/wx.h>
//#include <wx/sizer.h>
#include <wx/glcanvas.h>

// OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#include <GLUT/glut.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//----------------------------------------------

class Canvas : public wxGLCanvas {
	void Render();
public:
	Canvas(wxFrame * frame, int * args) : 
	wxGLCanvas(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _("GLCanvas"), args) {
		//empty
	}
	void Paint(wxPaintEvent& event);
	void View3D(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y);
protected:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------

BEGIN_EVENT_TABLE(Canvas, wxGLCanvas)
    EVT_PAINT    (Canvas::Paint)
END_EVENT_TABLE()

//----------------------------------------------

void Canvas::Paint(wxPaintEvent& WXUNUSED(event)){
    Render();
}

//----------------------------------------------

void Canvas::View3D(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
    glEnable(GL_COLOR_MATERIAL);
	
    glViewport(topleft_x, topleft_y, bottomright_x-topleft_x, bottomright_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    float ratio = (float)(bottomright_x-topleft_x)/(float)(bottomright_y-topleft_y);
    gluPerspective(45, ratio, 0.1, 200);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//----------------------------------------------*/

void Canvas::Render() {
	SetCurrent();
    wxPaintDC(this);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//View3D(-GetSize().x,0,GetSize().x, GetSize().y);
	View3D(0,0,GetSize().x, GetSize().y);
	glLoadIdentity();

// using a little of glut
	glColor4f(0,0,1,1);
	glTranslatef(3,0,-5);
	//glRotatef(90,0,0,1);
    glutWireTeapot(0.4);

	for (int i = 0; i < 8; i++) {
		glTranslatef(-0.5,0,0.5);
		glPushMatrix();
		glRotatef(i*45,0,1,0.5);
		glutWireTeapot(0.4);
		glPopMatrix();
	}
 
    glFlush();
    SwapBuffers();
}

//----------------------------------------------

class Game : public wxApp {
	virtual bool OnInit();

	wxFrame * frame;
	wxGLCanvas * glPane;
};

//----------------------------------------------

IMPLEMENT_APP(Game)
 
// This is executed upon startup, like 'main()' in non-wxWidgets programs.
bool Game::OnInit() {
	frame = new wxFrame((wxFrame*) NULL, -1, _T("GOTO - Client v0.1"), wxPoint(0,0), wxGetDisplaySize());
	frame->ShowFullScreen(true);
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

	glPane = new Canvas((wxFrame*) frame, args);
	//glPane = new wxGLCanvas((wxFrame*) frame, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0, _("GLCanvas"), args);

	sizer->Add(glPane, 1, wxEXPAND);
	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);

	frame->CreateStatusBar();
	frame->SetStatusText(_("Developing... - Grudge of the Oblivious"));
	
	frame->Show();
	SetTopWindow(frame);
	return true;
};

//----------------------------------------------