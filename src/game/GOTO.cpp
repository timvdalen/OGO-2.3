#include <wx/wx.h>
//#include <wx/sizer.h>
#include <wx/glcanvas.h>

#include "video.h"

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