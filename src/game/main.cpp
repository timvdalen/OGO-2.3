/*
 * Main module
 *
 * Date: 01-05-12 15:01
 *
 * Description:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "game.h"
#include "net.h"
#include "netcode.h"

using namespace Core;
using namespace Base::Alias;
using namespace Game;
using namespace Objects;

void Frame();

void UpdatePlayers();

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	Video::Initialize(argc, argv);
	
	Game::Initialize(argc, argv);
	Game::Exec("inputs.exec");
	
	Net::Initialize();
	NetCode::Initialize(argc, argv);
	
	Video::OnFrame = Frame;
	Video::StartEventLoop();
	
	NetCode::Terminate();
	Game::Terminate();
	Net::Terminate();
	Video::Terminate();
	
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

int loop = 0;

void Frame()
{
	NetCode::Frame();
	if (NetCode::TryLock())
	{
		// Critical section

		NetCode::Unlock();
	}

	Camera &cam = game.window->viewports[0]->camera;
	
	Terrain *t = TO(Terrain,game.world->terrain);
	if (t && (game.player->weapon == weapWrench))
		t->setSelected(t->getGridCoordinates(cam.origin, cam.objective));

	if (game.window->resized)
	{
		uword width, height;
		game.window->size(width = 0, height = 0);
		game.world->hud->resize(width, height);
	}

	game.controller->frame();
	
	game.player->update(game.controller->camera.objective);
	Vd velocity;
	switch (++loop)
	{
		case 1:
		{
			velocity = ((-game.player->rotation) * Vd(0,1,0))
			           * game.controller->move[dirY];
			
			if (!velocity)
			{
				velocity.z = 0;
				velocity = ~velocity;
			}
			if (game.controller->blocked)
				velocity = Vd(0,0,0);
			
			NetCode::Move(game.player->origin, velocity);
		} break;
		
		case 2:
			NetCode::Look(game.player->rotation);
			break;
		
		case 3:
			NetCode::Team(game.player->team, game.teams[game.player->team].resources);
			break;
		
		case 5: loop = 0; break;
	}
	
	UpdatePlayers();
	game.window->render();
}

//------------------------------------------------------------------------------

void UpdatePlayers()
{
	map<Player::Id,ObjectHandle>::iterator it;
	for (it = Game::game.players.begin(); it != Game::game.players.end(); ++it)
	{
		Player *p = TO(Player,it->second);
		if(!p || p == Game::game.player) return;
		p->interpolate();
	}
}

//------------------------------------------------------------------------------
