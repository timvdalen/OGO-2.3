#include <wx/wx.h>
#include <wx/statline.h>
#include <string>
#include <map>
#include "ConfigFile.h"
#include "games.h"
#include "lobby.h"
#include "common.h"

using namespace Lobby;
using namespace Net;
using namespace std;

struct GameNo{
	Game game;
	int id;
};

/* Globals */
//TODO: This is very ugly, need a way to pass messages between frames.
wxString selected;
bool joining;
wxString playerName;
int playnum;
map<int, Player> playerList;
wxCheckBox *cbList[6];
map<Address, GameNo> gameMap;
GameLobby *lobby;


GameList games(LOBBY_PORT);

//Gamelist listeners
static void onJoinGame(Address _server, Game _game);
static void onChangeGame(Address _server, Game _game);
static void onPartGame(Address _server);

//Lobby listeners
static void lobbyOnConnect(Player::Id pid, Game game);



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


mainFrame *frame;


class gameLobbyFrame: public wxFrame{
	public:
		gameLobbyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
		void OnSendClick(wxCommandEvent& event);
		void OnReadyClick(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

gameLobbyFrame *lobbyFrame;

enum{
	ID_LISTBOX,
	ID_JOIN,
	ID_CREATE,
	ID_SEND,
	ID_CHAT,
	ID_TXTSEND,
	ID_READY,
};

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(gameLobbyFrame, wxFrame)
	//EVT_TEXT_ENTER(ID_SEND, gameLobbyFrame::OnSendClick)
END_EVENT_TABLE()

IMPLEMENT_APP(LobbyGUI)

bool LobbyGUI::OnInit(){

	
	ConfigFile config("game.conf");
	string pname;
	config.readInto(pname, "playername");
	wxString pnamet(pname.c_str(), wxConvUTF8);
	playerName = pnamet;

	frame = new mainFrame( _("name T.B.D. Lobby"), wxPoint(49, 50), wxSize(320, 200) );
	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)	: wxFrame(NULL, -1, title, pos, size){
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	wxStaticText *st = new wxStaticText(panel, wxID_ANY, _("Possible games"), wxPoint(10, 10), wxDefaultSize, wxALIGN_LEFT);
	listbox = new wxListBox(panel, ID_LISTBOX, wxPoint(10, 30), wxSize(300, 100)); 

	wxButton *btnJoin = new wxButton(panel, ID_JOIN, _("Join"), wxPoint(10, 135));
	Connect(ID_JOIN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mainFrame::OnJoinClick));
	wxButton *btnCreate = new wxButton(panel, ID_CREATE, _("Create"), wxPoint(100, 135));
	Connect(ID_CREATE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mainFrame::OnCreateClick));

	CreateStatusBar();
	SetStatusText(_("Waiting for broadcast...."));

	games.onJoin = onJoinGame;
	games.onChange = onChangeGame;
	games.onPart = onPartGame;
}

void mainFrame::OnJoinClick(wxCommandEvent& WXUNUSED(event)){
	SetStatusText(_("You clicked join"));

	int sel = listbox->GetSelection();
	if(sel < 0){
		wxMessageBox(_("Please select a game"), _("No selection"), wxOK | wxICON_EXCLAMATION);
	}else{
		joining = true;
		Address addr;
		Game game;
		map<Address,GameNo>::iterator it;
		for(it = gameMap.begin(); it != gameMap.end(); it++){
			GameNo cur = it->second;
			if(cur.id == sel){
				addr = it->first;
				game = cur.game;
				break;
			}
		}
		lobby = new ClientLobby(string(playerName.mb_str()), addr);
	       	lobby->onConnect = lobbyOnConnect;
	}
}

void mainFrame::OnCreateClick(wxCommandEvent& WXUNUSED(event)){
	SetStatusText(_("Creating game"));

	wxString seltemp = wxGetTextFromUser(_("Please enter a game name"), _("Game name"));
	if(seltemp != _("")){
		selected = seltemp;
		joining = false;
		this->Close();
		gameLobbyFrame *lobby = new gameLobbyFrame( _("Hosting: ") + selected, wxPoint(49, 50), wxSize(900, 300));
		lobby->Show(true);
	}else{
		wxMessageBox(_("Please enter a game name"), _("No text"), wxOK | wxICON_EXCLAMATION);
	}
}

/* Game list listeners */
static void onJoinGame(Address _server, Game _game){
	wxListBox *gameList = (wxListBox*) frame->FindWindowById(ID_LISTBOX);
	int newid = gameList->GetCount(); 
	struct GameNo newgame;
	newgame.game = _game;
	newgame.id = newid;
	gameMap.insert(pair<Address, GameNo>(_server, newgame));
	wxString gamename(_game.name.c_str(), wxConvUTF8);
	frame->SetStatusText(_("New game found: ") + gamename);

	gameList->Append(gamename);
}

static void onPartGame(Address _server){
	wxListBox *gameList = (wxListBox*) frame->FindWindowById(ID_LISTBOX);
	GameNo gameno = gameMap[_server];
	gameList->Delete(gameno.id);
	map<Address,GameNo>::iterator it;
	for(it = gameMap.begin(); it != gameMap.end(); it++){
		GameNo cur = it->second;
		if(cur.id > gameno.id){
			cur.id--;
			gameMap.insert(pair<Address, GameNo>(it->first, cur));//TODO: Check is this is needed
		}
	}
	wxString gamename(gameno.game.name.c_str(), wxConvUTF8);
	frame->SetStatusText(_("Game left: " + gamename));
}

static void onChangeGame(Address _server, Game _game){
	//TODO: This could be better
	//A lot.
	onPartGame(_server);
	onJoinGame(_server, _game);
}


gameLobbyFrame::gameLobbyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)	: wxFrame(NULL, -1, title, pos, size){


	wxPanel *mainPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 300));

	wxPanel *panelLeft = new wxPanel(mainPanel, wxID_ANY, wxPoint(0, 0), wxSize(599, 300));
	
//	wxStaticText *st = new wxStaticText(panelLeft, wxID_ANY, _("Game: ") + selected, wxPoint(10, 5), wxDefaultSize, wxALIGN_LEFT);	
	/*
	wxTextCtrl *textChat = new wxTextCtrl(panelLeft, ID_CHAT, _("Player 1: Bla bla bla\nPlayer3: I agree bla bla\nPlayer 4: Nog een bla bla"), wxPoint(10, 20), wxSize(579, 240), wxTE_MULTILINE | wxTE_READONLY);
	wxStaticText *txtPlayerName = new wxStaticText(panelLeft, wxID_ANY, playerName + _(":"), wxPoint(10, 270), wxDefaultSize, wxALIGN_LEFT);
	wxTextCtrl *textInput = new wxTextCtrl(panelLeft, ID_TXTSEND, _(""), wxPoint(65, 263), wxSize(420, 25), wxTE_PROCESS_ENTER);
	wxButton *btnSend = new wxButton(panelLeft, ID_SEND, _("Send"), wxPoint(490, 260));
	Connect(ID_SEND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gameLobbyFrame::OnSendClick));
	Connect(ID_TXTSEND, wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(gameLobbyFrame::OnSendClick));

	/*
	wxStaticLine *slMain = new wxStaticLine(mainPanel, wxID_ANY, wxPoint(599, 10), wxSize(1, 280), wxLI_VERTICAL);
	
	wxPanel *panelRight = new wxPanel(mainPanel, wxID_ANY, wxPoint(601, 0), wxSize(299, 300));

	wxPanel *panelRightTop = new wxPanel(panelRight, wxID_ANY, wxPoint(0, 0), wxSize(299, 129));


/*	
	map<int, Player>::iterator it;
	int i=0;
	for(it = playerList.begin(); it != playerList.end(); it++){
		int id = it->first;
		Player player = it->second;

		wxCheckBox *chk = new wxCheckBox(panelRightTop, wxID_ANY, _(""), wxPoint(10, (15*i)));
		bool state;

		switch(player.state){
			case Player::stReady:
			case Player::stHost:
				state = true; break;

			case Player::stBusy:
			default:
				state = false; break;
		}
		chk->Enable(false);
		chk->SetValue(state);
		cbList[id] = chk;

		wxString name(player.name.c_str(), wxConvUTF8);

		wxStaticText *pn = new wxStaticText(panelRightTop, wxID_ANY, name, wxPoint(30, 5+(15*i)));
		i++;
	}
	


	wxStaticLine *slRight = new wxStaticLine(panelRight, wxID_ANY, wxPoint(5, 130), wxSize(290, 1), wxLI_HORIZONTAL); 
	
	wxPanel *panelRightBottom = new wxPanel(panelRight, wxID_ANY, wxPoint(0, 131), wxSize(299, 279));
	wxString readyTxt;
	if(joining){
		readyTxt = _("Ready");
	}else{
		readyTxt = _("Start");
	}
	wxButton *btnReady = new wxButton(panelRightBottom, ID_READY, readyTxt, wxPoint(5, 5), wxSize(289, 40));
	if(!joining){
		btnReady->Enable(false);
	}

	Connect(ID_READY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gameLobbyFrame::OnReadyClick));
	*/
}

void gameLobbyFrame::OnSendClick(wxCommandEvent& WXUNUSED(event)){
	wxTextCtrl *textInput = (wxTextCtrl*) this->FindWindowById(ID_TXTSEND);
	wxTextCtrl *textChat = (wxTextCtrl*) this->FindWindowById(ID_CHAT);
	textChat->AppendText(_("\n") + playerName + _(": ") + textInput->GetValue());
	textInput->ChangeValue(_(""));
}

void gameLobbyFrame::OnReadyClick(wxCommandEvent& WXUNUSED(event)){
	if(joining){
		Player me = playerList[playnum];
		wxCheckBox *chk = cbList[playnum];
		//At this point, state is either busy or ready
		if(me.state == Player::stBusy){
			me.state = Player::stReady;
		}else{
			me.state = Player::stBusy;
		}

		bool state;

		switch(me.state){
			case Player::stReady:
			case Player::stHost:
				state = true; break;

			case Player::stBusy:
			default:
				state = false; break;
		}

		chk->SetValue(state);

		//TODO: Again, do I need to do this?
		playerList[playnum] = me;
	}else{
		wxMessageBox(_("Starting game"), _("Starting game"), wxOK);
	}
}

/* Game lobby listeners */
static void lobbyOnConnect(Player::Id pid, Game game){
	GameList *glp = &games;	
	delete glp;
	
	playnum = pid;
	Player me;
	me.id = pid;
	me.state = Player::stBusy;
	me.name = string(playerName.mb_str());	
	playerList.insert(pair<int, Player>((int) pid, me));

	frame->Close();
	wxString gamename(game.name.c_str(), wxConvUTF8);
	lobbyFrame = new gameLobbyFrame( _("Joining: ") + gamename, wxPoint(49, 50), wxSize(900, 300));
	lobbyFrame->Show(true);
}

