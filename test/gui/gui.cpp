#include <wx/wx.h>

class LobbyGUI: public wxApp{
	    virtual bool OnInit();
};


class mainFrame: public wxFrame{
	public:
	    	mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

		void OnJoinClick(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

enum{
	ID_LISTBOX,
	ID_JOIN,
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
	wxListBox *listbox = new wxListBox(panel, ID_LISTBOX, wxPoint(10, 30), wxSize(300, 100)); 
	listbox->Append(_("Game 1"));
	listbox->Append(_("Andere game"));
	listbox->Append(_("Weer andere game"));

	wxButton *button = new wxButton(panel, ID_JOIN, wxT("Join"), wxPoint(10, 135));
	Connect(ID_JOIN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mainFrame::OnJoinClick));

	CreateStatusBar();
	SetStatusText(_("Waiting for broadcast...."));
}

void mainFrame::OnJoinClick(wxCommandEvent& WXUNUSED(event)){
	SetStatusText(_("You clicked join"));
}
