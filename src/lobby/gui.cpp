#include <wx/wx.h>
#include <wx/statline.h>
#include <string>
#include <map>
#include <queue>
#include <pthread.h>
#include "ConfigFile.h"
#include "games.h"
#include "lobby.h"
#include "common.h"
#include "CrossPlatform.h"

#define CALL(x, ...) { if (x) (x)(__VA_ARGS__); }

using namespace Lobby;
using namespace Net;
using namespace std;

struct GameNo{
	Game game;
	int id;
};

enum gamelist_function_t {
	glJOIN,
	glCHANGE,
	glPART
};

enum gamelobby_function_t{
	CONNECT,
	PLAYER,
	JOIN,
	PART,
	TEAM,
	STATE,
	CHAT,
	CLOSE,
	START,
	SERVERSTART
};

/* Globals */
//TODO: This is very ugly, need a way to pass messages between frames.
wxString selected;
bool joining;
wxString playerName;
int playnum;
map<int, Player> playerList;
wxCheckBox *cbList[6];
wxStaticText *stList[6];
map<Address, GameNo> gameMap;
GameLobby *lobby;
Game activeGame;
Address server;
pthread_mutex_t mainFrameLock;
pthread_mutex_t lobbyFrameLock;

GameList *games;

//Gamelist listeners
static void onJoinGameCall(Address _server, Game _game);
static void onChangeGameCall(Address _server, Game _game);
static void onPartGameCall(Address _server);
static void onJoinGame(Address _server, Game _game);
static void onChangeGame(Address _server, Game _game);
static void onPartGame(Address _server);

//Lobby listeners
static void lobbyOnConnectCall(Player::Id pid, Game game);
static void lobbyOnPlayerCall(Player player);
static void lobbyOnJoinCall(Player::Id pid, string playerName);
static void lobbyOnPartCall(Player::Id pid);
static void lobbyOnChatCall(Player::Id pid, string line);
static void lobbyOnCloseCall();
static void lobbyOnStateCall(Player::Id pid, Player::State state);
static void lobbyOnStartCall();
static void serverLobbyOnStartCall();
static void lobbyOnConnect(Player::Id pid, Game game);
static void lobbyOnPlayer(Player player);
static void lobbyOnJoin(Player::Id pid, string playerName);
static void lobbyOnPart(Player::Id pid);
static void lobbyOnChat(Player::Id pid, string line);
static void lobbyOnClose();
static void lobbyOnState(Player::Id pid, Player::State state);
static void lobbyOnStart();
static void serverLobbyOnStart();

static void lobbyAddPlayer(Player player);

class LobbyGUI: public wxApp{
	    virtual bool OnInit();
};

class mainFrame: public wxFrame{
		wxListBox *listbox;	
	public:
		queue<gamelist_function_t> functions;
		//Casting from void became a problem
		queue<Address> addrArgs;
		queue<Game> gameArgs;

		mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

		void ProcessCallbacks(wxIdleEvent &event);

		void OnJoinClick(wxCommandEvent& event);
		void OnCreateClick(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()	
};

mainFrame *frame;

class gameLobbyFrame: public wxFrame{
	private:
		static bool GetState(Player::State);

	public:
		queue<gamelobby_function_t> functions;
		queue<Game> gameArgs;
		//Next three could be merged but this might make things harder to understand
		queue<Player> playArgs;
		queue<Player::Id> pidArgs;
		queue<Player::State> stateArgs;
		queue<string> strArgs;
		queue<unsigned char> ucharArgs;

		gameLobbyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
		void OnSendClick(wxCommandEvent& event);
		void OnReadyClick(wxCommandEvent& event);

		void ProcessCallbacks(wxIdleEvent &event);

		void SetupPlayerList();
		void AddToPlayerList(Player player, int position);
		void UpdatePlayerList(Player player);
		void AddChatLine(Player player, string line);
		void RemoveCheckboxes();
		void CloseLobby();


		wxPanel *panelRightTop;
		wxTextCtrl *txtChat;

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
	EVT_IDLE(mainFrame::ProcessCallbacks)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(gameLobbyFrame, wxFrame)
	//EVT_TEXT_ENTER(ID_SEND, gameLobbyFrame::OnSendClick)
	EVT_IDLE(gameLobbyFrame::ProcessCallbacks)
END_EVENT_TABLE()

IMPLEMENT_APP(LobbyGUI)

bool LobbyGUI::OnInit(){
    CrossPlatform::init();
	
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

	if(!(pthread_mutex_init(&mainFrameLock, NULL) == 0)) exit(EXIT_FAILURE);

	games = new GameList(LOBBY_PORT);

	games->onJoin = onJoinGameCall;
	games->onChange = onChangeGameCall;
	games->onPart = onPartGameCall;
}

void mainFrame::ProcessCallbacks(wxIdleEvent &event){
	if(functions.size() != 0){
		//Achieve lock on functions and arguments queues
		pthread_mutex_lock(&mainFrameLock);
		//Handle event on the queue
		gamelist_function_t function = functions.front();
		functions.pop();
		switch(function){
			case glJOIN:
				{
					Address server = addrArgs.front(); addrArgs.pop();
					Game game = gameArgs.front(); gameArgs.pop();
					onJoinGame(server, game);
				}
				break;
			case glCHANGE:
				{
					Address server = addrArgs.front(); addrArgs.pop();
					Game game = gameArgs.front(); gameArgs.pop();
					onChangeGame(server, game);
				}
				break;
			case glPART:
				{
					Address server = addrArgs.front(); addrArgs.pop();
					onPartGame(server);
				}
				break;
		}
		//Release lock
		pthread_mutex_unlock(&mainFrameLock);
	}
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

		server = addr;

		delete games;

		activeGame = game;

		frame->Close();
		wxString gamename(game.name.c_str(), wxConvUTF8);
		lobbyFrame = new gameLobbyFrame( _("Joining: ") + gamename, wxPoint(49, 50), wxSize(900, 300));
		lobbyFrame->Show(true);
	}
}

void mainFrame::OnCreateClick(wxCommandEvent& WXUNUSED(event)){
	SetStatusText(_("Creating game"));

	wxString seltemp = wxGetTextFromUser(_("Please enter a game name"), _("Game name"));
	if(seltemp != _("")){
		joining = false;
		
		delete games;

		selected = seltemp;
		joining = false;
		frame->Close();
		lobbyFrame = new gameLobbyFrame( _("Hosting: ") + selected, wxPoint(49, 50), wxSize(900, 300));
		lobbyFrame->Show(true);
	}else{
		wxMessageBox(_("Please enter a game name"), _("No text"), wxOK | wxICON_EXCLAMATION);
	}
}

/* Game list listeners */
static void onJoinGameCall(Address _server, Game _game){
	//Achieve lock
	pthread_mutex_lock(&mainFrameLock);

	frame->functions.push(glJOIN);
	frame->addrArgs.push(_server);
	frame->gameArgs.push(_game);

	//Release lock
	pthread_mutex_unlock(&mainFrameLock);
}

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

static void onPartGameCall(Address _server){
	//Achieve lock
	pthread_mutex_lock(&mainFrameLock);

	frame->functions.push(glPART);
	frame->addrArgs.push(_server);

	//Release lock
	pthread_mutex_unlock(&mainFrameLock);
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

static void onChangeGameCall(Address _server, Game _game){
	//Achieve lock
	pthread_mutex_lock(&mainFrameLock);

	frame->functions.push(glCHANGE);
	frame->addrArgs.push(_server);
	frame->gameArgs.push(_game);

	//Release lock
	pthread_mutex_unlock(&mainFrameLock);
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


	wxString gameName(activeGame.name.c_str(), wxConvUTF8);
	
	wxStaticText *st = new wxStaticText(panelLeft, wxID_ANY, _("Game: ") + gameName, wxPoint(10, 5), wxDefaultSize, wxALIGN_LEFT);	

	txtChat = new wxTextCtrl(panelLeft, ID_CHAT, _("Server: Welcome to the game! Enjoy your stay. [MOTD hier]\n"), wxPoint(10, 20), wxSize(579, 240), wxTE_MULTILINE | wxTE_READONLY);
	wxStaticText *txtPlayerName = new wxStaticText(panelLeft, wxID_ANY, playerName + _(":"), wxPoint(10, 270), wxDefaultSize, wxALIGN_LEFT);
	wxTextCtrl *textInput = new wxTextCtrl(panelLeft, ID_TXTSEND, _(""), wxPoint(65, 263), wxSize(420, 25), wxTE_PROCESS_ENTER);
	wxButton *btnSend = new wxButton(panelLeft, ID_SEND, _("Send"), wxPoint(490, 260));
	Connect(ID_SEND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gameLobbyFrame::OnSendClick));
	Connect(ID_TXTSEND, wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(gameLobbyFrame::OnSendClick));


	wxStaticLine *slMain = new wxStaticLine(mainPanel, wxID_ANY, wxPoint(599, 10), wxSize(1, 280), wxLI_VERTICAL);
	
	wxPanel *panelRight = new wxPanel(mainPanel, wxID_ANY, wxPoint(601, 0), wxSize(299, 300));

	panelRightTop = new wxPanel(panelRight, wxID_ANY, wxPoint(0, 0), wxSize(299, 129));

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
		//btnReady->Enable(false);
	}

	Connect(ID_READY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gameLobbyFrame::OnReadyClick));

	if(!(pthread_mutex_init(&lobbyFrameLock, NULL) == 0)) exit(EXIT_FAILURE);
	
	if(joining){
		//Create new ClientLobby
		//Wait for lobby to connect
		lobby = new ClientLobby(string(playerName.mb_str()), server);
		lobby->onStart = lobbyOnStartCall;
	}else{
		//Create new ServerLobby
		lobby = new ServerLobby(string(selected.mb_str()), string(playerName.mb_str()), LOBBY_PORT);
		lobby->onStart = serverLobbyOnStartCall;
	}
	lobby->onConnect = lobbyOnConnectCall;
	lobby->onPlayer = lobbyOnPlayerCall;
	lobby->onJoin = lobbyOnJoinCall;
	lobby->onPart = lobbyOnPartCall;
	lobby->onChat = lobbyOnChatCall;
	lobby->onClose = lobbyOnCloseCall;
	lobby->onState = lobbyOnStateCall;
}

void gameLobbyFrame::ProcessCallbacks(wxIdleEvent &event){
	if(functions.size() != 0){
		//Achieve lock on functions and arguments queues
		pthread_mutex_lock(&lobbyFrameLock);
		//Handle event on the queue
		gamelobby_function_t function = functions.front();
		functions.pop();

		switch(function){
			case CONNECT:
				{
					Player::Id pid = pidArgs.front(); pidArgs.pop();
					Game game = gameArgs.front(); gameArgs.pop();
					lobbyOnConnect(pid, game);
				}
				break;
			case PLAYER:
				{
					Player player = playArgs.front(); playArgs.pop();
					lobbyOnPlayer(player);
				}
				break;
			case JOIN:
				{
					Player::Id pid = pidArgs.front(); pidArgs.pop();
					string playerName = strArgs.front(); strArgs.pop();
					lobbyOnJoin(pid, playerName);
				}
				break;
			case PART:
				{
					Player::Id pid = pidArgs.front(); pidArgs.pop();
					lobbyOnPart(pid);
				}
				break;
			/* Not implemented yet
			case TEAM:
				{
					Player::Id pid = pidArgs.front(); pidArgs.pop();
					unsigned char team = ucharArgs.front(); ucharArgs.pop();
					lobbyOnTeam(pid, team);
				}
				break;
			*/
			case STATE:
				{
					Player::Id pid = pidArgs.front(); pidArgs.pop();
					Player::State state = stateArgs.front(); stateArgs.pop();
					lobbyOnState(pid, state);
				}
				break;
			case CHAT:
				{
					Player::Id pid = pidArgs.front(); pidArgs.pop();
					string line = strArgs.front(); strArgs.pop();
					lobbyOnChat(pid, line);
				}
				break;
			case CLOSE:
				lobbyOnClose();
				break;
			case START:
				lobbyOnStart();
				break;
			case SERVERSTART:
				serverLobbyOnStart();
				break;
		}

		pthread_mutex_unlock(&lobbyFrameLock);
	}
}

void gameLobbyFrame::OnSendClick(wxCommandEvent& WXUNUSED(event)){
	wxTextCtrl *textInput = (wxTextCtrl*) this->FindWindowById(ID_TXTSEND);
	wxString message = textInput->GetValue();
	textInput->ChangeValue(_(""));
	if(joining){
		lobby->chat(string(message.mb_str()));
	}else{
		ServerLobby *server = reinterpret_cast<ServerLobby *> (lobby);
		server->chat(string(message.mb_str()));
	}
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

		ClientLobby *client = reinterpret_cast<ClientLobby *> (lobby);
		client->state(state);

		chk->SetValue(state);

		//TODO: Again, do I need to do this?
		playerList[playnum] = me;
	}else{
		ServerLobby *server = reinterpret_cast<ServerLobby *> (lobby);
		server->start();

		//Netcode should call this, but for now call it from here:
		serverLobbyOnStart();
	}
}

void gameLobbyFrame::SetupPlayerList(){
	map<int, Player>::iterator it;
	puts("Setting up players");
	int i=0;
	for(it = playerList.begin(); it != playerList.end(); it++){		
		Player player = it->second;
		this->AddToPlayerList(player, i);
		i++;
	}

}

void gameLobbyFrame::AddToPlayerList(Player player, int i = playerList.size()){
	panelRightTop->SetFocus();
	wxCheckBox *chk = new wxCheckBox(panelRightTop, wxID_ANY, _("")	, wxPoint(10, (15*i)));
	chk->Enable(false);
	chk->SetValue(gameLobbyFrame::GetState(player.state));
	cbList[(int) player.id] = chk;

	wxString name(player.name.c_str(), wxConvUTF8);

	wxStaticText *pn = new wxStaticText(panelRightTop, wxID_ANY, name, wxPoint(30, 5+(15*i)));
	stList[(int) player.id] = pn;
}

void gameLobbyFrame::UpdatePlayerList(Player player){
	wxCheckBox *chk = cbList[(int) player.id];
	chk->SetValue(gameLobbyFrame::GetState(player.state));
}

bool gameLobbyFrame::GetState(Player::State state){
	bool ret;
	switch(state){
		case Player::stReady:
		case Player::stHost:
			ret = true; break;
			case Player::stBusy:
		default:
			ret = false; break;
	}

	return ret;
}

void gameLobbyFrame::AddChatLine(Player player, string line){
	wxString playername(player.name.c_str(), wxConvUTF8);
	wxString wxline(line.c_str(), wxConvUTF8);
	txtChat->AppendText(playername + _(": ") + wxline + _("\n"));	
}

void gameLobbyFrame::RemoveCheckboxes(){
	map<int, Player>::iterator it;
	for(it = playerList.begin(); it != playerList.end(); it++){		
		int id = it->first;
		
		char * str;
		sprintf(str, "id: %i, name: %s", id, it->second.name.c_str());
		puts(str);
		cbList[id]->Destroy();
		stList[id]->Destroy();
	}

}

void gameLobbyFrame::CloseLobby(){
	this->Close();
}

/* Game lobby listeners */
static void lobbyOnConnectCall(Player::Id pid, Game game){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(CONNECT);
	lobbyFrame->pidArgs.push(pid);
	lobbyFrame->gameArgs.push(game);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnConnect(Player::Id pid, Game game){
	playnum = pid;
	Player me;
	me.id = pid;
	if(joining){
		me.state = Player::stBusy;
	}else{
		me.state = Player::stHost;
	}
	me.name = string(playerName.mb_str());	
	
	lobbyAddPlayer(me);
}

static void lobbyOnJoinCall(Player::Id pid, string _playername){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(JOIN);
	lobbyFrame->pidArgs.push(pid);
	lobbyFrame->strArgs.push(_playername);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnJoin(Player::Id pid, string _playername){
	//I'm assuming this is called for new players in the lobby
	//TODO: Confirm this
	Player newplayer;
	newplayer.id = pid;
	newplayer.state = Player::stBusy;
	newplayer.name = _playername;

	lobbyAddPlayer(newplayer);
}

static void lobbyOnPlayerCall(Player player){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(PLAYER);
	lobbyFrame->playArgs.push(player);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnPlayer(Player player){
	//I'm assuming this is called for existing players in the lobby you just joined
	//TODO: Confirm this
	lobbyAddPlayer(player);
}

static void lobbyAddPlayer(Player player){
	lobbyFrame->AddToPlayerList(player);

	playerList.insert(pair<int, Player>((int) player.id, player));
}

static void lobbyOnPartCall(Player::Id pid){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(PART);
	lobbyFrame->pidArgs.push(pid);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnPart(Player::Id pid){
	lobbyFrame->RemoveCheckboxes();
	playerList.erase((int) pid);
	lobbyFrame->SetupPlayerList();
}

static void lobbyOnChatCall(Player::Id pid, string line){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(CHAT);
	lobbyFrame->pidArgs.push(pid);
	lobbyFrame->strArgs.push(line);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnChat(Player::Id pid, string line){
	Player player = playerList[(int) pid];
	lobbyFrame->AddChatLine(player, line);
}

static void lobbyOnCloseCall(){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(CLOSE);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnClose(){
	lobbyFrame->CloseLobby();
}

static void lobbyOnStateCall(Player::Id pid, Player::State state){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(STATE);
	lobbyFrame->pidArgs.push(pid);
	lobbyFrame->stateArgs.push(state);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnState(Player::Id pid, Player::State state){
	Player player = playerList[(int) pid];
	player.state = state;

	lobbyFrame->UpdatePlayerList(player);

	//TODO: Yet again
	playerList[(int) pid] = player;	
}

static void lobbyOnStartCall(){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(START);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void lobbyOnStart(){
	char *serverAddr;
	server.string(serverAddr);
	//I would use exec_*, but Windows does not support that
	#if (defined WIN32 || defined _MSC_VER)
		char command[28];
		strcat(command, "start Game ");
		strcat(command, serverAddr);
	#else
		char command[27];
		strcat(command, "./Game ");
		strcat(command, serverAddr);
		strcat(command, " &");
	#endif
	system(command);
	exit(EXIT_SUCCESS);
}

static void serverLobbyOnStartCall(){
	//Achieve lock
	pthread_mutex_lock(&lobbyFrameLock);

	lobbyFrame->functions.push(SERVERSTART);

	//Release lock
	pthread_mutex_unlock(&lobbyFrameLock);
}

static void serverLobbyOnStart(){
	#if (defined WIN32 || defined _MSC_VER)
		system("start Game");
	#else
		system("./Game &");
	#endif
	exit(EXIT_SUCCESS);
}
