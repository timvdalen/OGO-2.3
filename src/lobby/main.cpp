/*
 * Lobby main file
 *
 * Authors: Ogo 2.3 Group 3 2012
 *
 * Date: 26-04-12 12:11
 *
 * Description: Player accumulation and social interaction prior to a game.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <windows.h>

#include <sstream>
#include <string>
#include <map>

#include "net.h"
#include "protocol.h"
#include "games.h"
#include "lobby.h"

using namespace std;
using namespace Lobby;

string playerName, gameName;
string queue = "";
bool queueing = false;
bool hosting = false, connecting = false;
Net::Address server;

//------------------------------------------------------------------------------

map<Net::Address,Game> gamelist; // Should be guarded by a mutex

void gamelist_update();
void gamelist_join(Net::Address server, Game game);
void gamelist_change(Net::Address server, Game game);
void gamelist_part(Net::Address server);

//------------------------------------------------------------------------------

map<Lobby::Player::Id, string> playerlist; // Should be guarded by a mutex

void lobby_connect(Player::Id pid, Game game);
void lobby_player(Player player);
void lobby_join(Player::Id pid, string playerName);
void lobby_part(Player::Id pid);
void lobby_team(Player::Id pid, unsigned char team);
void lobby_state(Player::Id pid, Player::State state);
void lobby_chat(Player::Id pid, string line);
void lobby_close();
void lobby_start();

char *gets2(char *data, size_t len);
void printf2(const char *fmt, ...);

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	{
	Net::Initialize();

	char buffer[1024];
	string line, cmd;
	Protocol::Message msg;
	GameLobby *lobby;

	printf("Player name> ");
	playerName = gets2(buffer, sizeof (buffer));
	printf("Welcome %s!\nCommands:\n\t!list\t\tget active server list\n"
	       "\t!host [name]\tstart a server\n\t!connect <addr>\tjoin a lobby\n> ");

	{
		GameList games(LOBBY_PORT);
		games.onJoin = gamelist_join;
		games.onChange = gamelist_change;
		games.onPart = gamelist_part;

		while (games.valid())
		{
			line = gets2(buffer, sizeof (buffer));
			if (line[0] == '!')
			{
				msg = line;
				cmd = (string) msg[0];
				if (cmd == "!list")
				{
					map<Net::Address,Game>::iterator it;
					for (it = gamelist.begin(); it != gamelist.end(); ++it)
					{
						it->first.string(buffer);
						printf("%s) %s (%d)\n", buffer, it->second.name.c_str(),
						                                it->second.numPlayers);
					}
				}
				else if (cmd == "!connect")
				{
					if (msg.size() < 2) continue;
					connecting = true;
					break;
				}
				else if (cmd == "!host")
				{
					hosting = true;
					break;
				}
			}
			printf("> ");
		}

		if (!games.valid())
		{
			puts("Game polling failed!");
			return (EXIT_FAILURE);
		}
	}

	puts("Stopped polling for servers.");

	if (connecting)
	{
		string line = msg[1];
		Net::Address addr((const char *) line.c_str(), LOBBY_PORT);
		if (!addr.valid())
		{
			printf("Address '%s' is invalid!\n", line.c_str());
			return (EXIT_FAILURE);
		}

		addr.string(buffer);
		printf("Connecting to %s...\n", buffer);

		server = addr;

		lobby = new ClientLobby(playerName, addr);
		if (!lobby || !lobby->valid())
		{
			puts("Could not connect to the lobby.");
			return (EXIT_FAILURE);
		}
	}
	else if (hosting)
	{
		string line = (msg.size() < 2) ? "" : msg[1];
		if (line.empty())
			line = playerName + "'s game";

		lobby = new ServerLobby(line, playerName, LOBBY_PORT);
		if (!lobby || !lobby->valid())
		{
			puts("Could not create a new lobby.");
			return (EXIT_FAILURE);
		}
	}

	lobby->onConnect = lobby_connect;
	lobby->onPlayer = lobby_player;
	lobby->onJoin = lobby_join;
	lobby->onPart = lobby_part;
	lobby->onTeam = lobby_team;
	lobby->onState = lobby_state;
	lobby->onChat = lobby_chat;
	lobby->onClose = lobby_close;
	lobby->onStart = lobby_start;

	while (lobby->valid())
	{
		ServerLobby *server = reinterpret_cast<ServerLobby *> (lobby);
		ClientLobby *client = reinterpret_cast<ClientLobby *> (lobby);
		line = gets2(buffer, sizeof (buffer));

		if (!queue.empty())
		{
			printf(queue.c_str());
			queue.clear();
		}

		if (line.empty())
			queueing = !queueing;
		else if (line[0] == '!')
		{
			msg = line;
			cmd = (string) msg[0];
			if ((cmd == "!close") || (cmd == "!exit"))
				lobby->close();

			else if ((msg.size() > 1) && (cmd == "!team"))
			{
				     if (hosting)    server->team((int) msg[1]);
				else if (connecting) client->team((int) msg[1]);
			}

			else if (hosting)
			{
				if (cmd == "!start") server->start();
			}

			else if (connecting)
			{
				     if (cmd == "!ready") client->state(true);
				else if (cmd == "!busy")  client->state(false);
			}
		}
		else if (hosting)    server->chat(line);
		else if (connecting) client->chat(line);

		if (queueing)
			printf("> ");
	}

	delete lobby;

	Net::Terminate();
	}

	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

void gamelist_join(Net::Address server, Game game)
{
	gamelist[server] = game;
}

//------------------------------------------------------------------------------

void gamelist_change(Net::Address server, Game game)
{
	gamelist[server] = game;
}

//------------------------------------------------------------------------------

void gamelist_part(Net::Address server)
{
	gamelist.erase(server);
}

//------------------------------------------------------------------------------

void lobby_connect(Player::Id pid, Game game)
{
	gameName = game.name;
	printf2("--- Connected to server: %s (%d)\n", game.name.c_str(), game.numPlayers);
	playerlist[pid] = playerName;
}

//------------------------------------------------------------------------------

void lobby_player(Player player)
{
	char state;
	switch (player.state)
	{
		case Player::stBusy: state = 'B'; break;
		case Player::stReady: state = 'R'; break;
		case Player::stHost: state = 'H'; break;
	}
	printf("   [%d]\t%s\t%c\n", player.team, player.name.c_str(), state);
	playerlist[player.id] = player.name;
}

//------------------------------------------------------------------------------

void lobby_join(Player::Id pid, string playerName)
{
	printf2("*** %s joined.\n", playerName.c_str());
	playerlist[pid] = playerName;
}

//------------------------------------------------------------------------------

void lobby_part(Player::Id pid)
{
	printf2("*** %s parted.\n", playerlist[pid].c_str());
	playerlist.erase(pid);
}

//------------------------------------------------------------------------------

void lobby_team(Player::Id pid, unsigned char team)
{
	printf2("*** %s changed to team %d.\n", playerlist[pid].c_str(), team);
}

//------------------------------------------------------------------------------

void lobby_state(Player::Id pid, Player::State state)
{

	printf2("*** %s is %s.\n", playerlist[pid].c_str(), state ? "ready" : "busy");
}

//------------------------------------------------------------------------------

void lobby_chat(Player::Id pid, string line)
{
	printf2("<%s> %s\n", playerlist[pid].c_str(), line.c_str());
}

//------------------------------------------------------------------------------

void lobby_close()
{
	printf2("--- Connection lost.\n");
}

//------------------------------------------------------------------------------

void lobby_start()
{
	char serverAddr[180];
	server.string(serverAddr);
#if (defined WIN32 || defined _MSC_VER)
	stringstream ss;
	ss << "start Game --connect " << serverAddr;
	const char* command = ss.str().c_str();
#else
#ifdef __APPLE__
	stringstream ss;
	ss << "osascript -e \"tell application \\\"Terminal\\\" to do script \\\"'`pwd`/Game' -p '`pwd`/ --connect ' " << serverAddr;
	const char *command = ss.str().c_str();
#else
	stringstream ss;
	ss << "./Game --connect " << serverAddr << " &";
	const char *command = ss.str().c_str();
#endif
#endif
	Sleep(2000);
	system(command);
	exit(EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

char *gets2(char *data, size_t len)
{
	data[0] = 0;
	fgets(data, len, stdin);
	char *ptr = data + strlen(data) - 1;
	if (*ptr == '\n') *ptr-- = 0;
	if (*ptr == '\r') *ptr = 0;
	return data;
}

//------------------------------------------------------------------------------

void printf2(const char *fmt, ...)
{
	char buffer[1024];
	va_list args;

	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	if (queueing)
		queue += string(buffer);
	else
		printf(buffer);
}

//------------------------------------------------------------------------------
