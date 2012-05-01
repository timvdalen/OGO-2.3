#include <wx/wx.h>

/* Globals */
//TODO: This is very ugly, need a way to pass messages between frames.
wxString selected;
bool joining;

class LobbyGUI: public wxApp{
	    virtual bool OnInit();
};

class mainFrame: public wxFrame{
		wxListBox *listbox;

	public:
	    	mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

		void OnJoinClick(wxCommandEvent& event);
		void OnCreateClick(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()	
};

class gameLobby: public wxFrame{
	public:
		gameLobby(const wxString& title, const wxPoint& pos, const wxSize& size);	
};

enum{
	ID_LISTBOX,
	ID_JOIN,
	ID_CREATE,
};

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
END_EVENT_TABLE()

IMPLEMENT_APP(LobbyGUI)

bool LobbyGUI::OnInit(){
	mainFrame *frame = new mainFrame( _("name T.B.D. Lobby"), wxPoint(49, 50), wxSize(320, 200) );
	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)	: wxFrame(NULL, -1, title, pos, size){
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	wxStaticText *st = new wxStaticText(panel, wxID_ANY, _("Possible games"), wxPoint(10, 10), wxDefaultSize, wxALIGN_LEFT);
	listbox = new wxListBox(panel, ID_LISTBOX, wxPoint(10, 30), wxSize(300, 100)); 
	listbox->Append(_("Game 1"));
	listbox->Append(_("Andere game"));
	listbox->Append(_("Weer andere game"));

	wxButton *btnJoin = new wxButton(panel, ID_JOIN, _("Join"), wxPoint(10, 135));
	Connect(ID_JOIN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mainFrame::OnJoinClick));
	wxButton *btnCreate = new wxButton(panel, ID_CREATE, _("Create"), wxPoint(100, 135));
	Connect(ID_CREATE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mainFrame::OnCreateClick));

	CreateStatusBar();
	SetStatusText(_("Waiting for broadcast...."));
}

void mainFrame::OnJoinClick(wxCommandEvent& WXUNUSED(event)){
	SetStatusText(_("You clicked join"));

	int sel = listbox->GetSelection();
	if(sel < 0){
		wxMessageBox(_("Please select a game"), _("No selection"), wxOK | wxICON_EXCLAMATION);
	}else{
		selected = listbox->GetString(sel);
		joining = true;
		this->Close();
		gameLobby *lobby = new gameLobby( _("Joining: ") + selected, wxPoint(49, 50), wxSize(300, 150));
		lobby->Show(true);
	}
}

void mainFrame::OnCreateClick(wxCommandEvent& WXUNUSED(event)){
	SetStatusText(_("Creating game"));

	wxString seltemp = wxGetTextFromUser(_("Please enter a game name"), _("Game name"));
	if(seltemp != _("")){
		selected = seltemp;
		joining = false;
		this->Close();
		gameLobby *lobby = new gameLobby( _("Hosting: ") + selected, wxPoint(49, 50), wxSize(300, 150));
		lobby->Show(true);
	}else{
		wxMessageBox(_("Please enter a game name"), _("No text"), wxOK | wxICON_EXCLAMATION);
	}
}


gameLobby::gameLobby(const wxString& title, const wxPoint& pos, const wxSize& size)	: wxFrame(NULL, -1, title, pos, size){
	wxPanel *panel = new wxPanel(this, wxID_ANY);	
	wxStaticText *st = new wxStaticText(panel, wxID_ANY, _("Game: ") + selected, wxPoint(10, 10), wxDefaultSize, wxALIGN_LEFT);
}
