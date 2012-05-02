#include <wx/wx.h>
#include <wx/statline.h>

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
		void OnSendClick(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

enum{
	ID_LISTBOX,
	ID_JOIN,
	ID_CREATE,
	ID_SEND,
	ID_TXTSEND,
};

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(gameLobby, wxFrame)
	//EVT_TEXT_ENTER(ID_SEND, gameLobby::OnSendClick)
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
		gameLobby *lobby = new gameLobby( _("Joining: ") + selected, wxPoint(49, 50), wxSize(900, 300));
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
		gameLobby *lobby = new gameLobby( _("Hosting: ") + selected, wxPoint(49, 50), wxSize(900, 300));
		lobby->Show(true);
	}else{
		wxMessageBox(_("Please enter a game name"), _("No text"), wxOK | wxICON_EXCLAMATION);
	}
}


gameLobby::gameLobby(const wxString& title, const wxPoint& pos, const wxSize& size)	: wxFrame(NULL, -1, title, pos, size){
	wxPanel *mainPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 300));

	wxPanel *panelLeft = new wxPanel(mainPanel, wxID_ANY, wxPoint(0, 0), wxSize(599, 300));
	
	wxStaticText *st = new wxStaticText(panelLeft, wxID_ANY, _("Game: ") + selected, wxPoint(10, 5), wxDefaultSize, wxALIGN_LEFT);	
	wxTextCtrl *textChat = new wxTextCtrl(panelLeft, -1, _("Player 1: Bla bla bla\nPlayer3: I agree bla bla\nPlayer 4: Nog een bla bla"), wxPoint(10, 20), wxSize(579, 240), wxTE_MULTILINE | wxTE_READONLY);
	wxStaticText *playerName = new wxStaticText(panelLeft, wxID_ANY, _("Player 2:"), wxPoint(10, 270), wxDefaultSize, wxALIGN_LEFT);
	wxTextCtrl *textInput = new wxTextCtrl(panelLeft, ID_TXTSEND, _(""), wxPoint(65, 263), wxSize(420, 25), wxTE_PROCESS_ENTER); //TODO: generate and catch ENTER
	wxButton *btnSend = new wxButton(panelLeft, ID_SEND, _("Send"), wxPoint(490, 260));
	Connect(ID_SEND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gameLobby::OnSendClick));
	Connect(ID_TXTSEND, wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(gameLobby::OnSendClick));

	wxStaticLine *slMain = new wxStaticLine(mainPanel, wxID_ANY, wxPoint(599, 10), wxSize(1, 280), wxLI_VERTICAL);
	
	wxPanel *panelRight = new wxPanel(mainPanel, wxID_ANY, wxPoint(601, 0), wxSize(299, 300));

	wxPanel *panelRightTop = new wxPanel(panelRight, wxID_ANY, wxPoint(0, 0), wxSize(299, 99));
	wxCheckBox *chkOne = new wxCheckBox(panelRightTop, wxID_ANY, _("Player 1"), wxPoint(10, 5));
	wxCheckBox *chkTwo = new wxCheckBox(panelRightTop, wxID_ANY, _("Player 2"), wxPoint(10, 20));
	wxCheckBox *chkThree = new wxCheckBox(panelRightTop, wxID_ANY, _("Player 3"), wxPoint(10, 35));
	wxCheckBox *chkFour = new wxCheckBox(panelRightTop, wxID_ANY, _("Player 4"), wxPoint(10, 50));
	wxCheckBox *chkFive = new wxCheckBox(panelRightTop, wxID_ANY, _("Player 5"), wxPoint(10, 65));
	wxCheckBox *chkSix = new wxCheckBox(panelRightTop, wxID_ANY, _("Player 6"), wxPoint(10, 80));
	
	chkOne->SetValue(true);
	chkThree->SetValue(true);
	chkFive->SetValue(true);

	/* Disable the checkboxes
	 * This also causes the text to get disabled, so I'm leaving this out for now
	 * A solution would be to draw the player names and checkboxes separate
	chkOne->Enable(false);
	chkTwo->Enable(false);
	chkThree->Enable(false);
	chkFour->Enable(false);
	chkFive->Enable(false);
	chkSix->Enable(false);
	*/

	wxStaticLine *slRight = new wxStaticLine(panelRight, wxID_ANY, wxPoint(5, 100), wxSize(290, 1), wxLI_HORIZONTAL); 
	wxPanel *panelRightBottom = new wxPanel(panelRight, wxID_ANY, wxPoint(0, 101), wxSize(299, 299));
}

void gameLobby::OnSendClick(wxCommandEvent& WXUNUSED(event)){
	wxMessageBox(_("Sending: "), _("A"), wxOK);
}
