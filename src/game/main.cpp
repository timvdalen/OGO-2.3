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
#include "broadcaster.h"

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
	//Broadcaster::Initialize();	
	Game::Initialize(argc, argv);
	Game::Exec("inputs.exec");
	
	Net::Initialize();
	NetCode::Initialize(argc, argv);
	
	Video::OnFrame = Frame;
	Video::StartEventLoop();
	
	NetCode::Terminate();
	Game::Terminate();
	Net::Terminate();
	//Broadcaster::Terminate();
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
		vector<ObjectHandle>::iterator it;
		for(it = game.world->temporary.begin(); it != game.world->temporary.end(); it++){
			Droppable *d = TO(Droppable, *it);
			if(d){
				if (!Vd(game.player->origin - d->origin) < 1.5)
				{
					if(game.teams.count(game.player->team))
					{
						game.teams[game.player->team].resources += d->worth;
						NetCode::Take(d->id);
						d->done = true;
					}
				}
			}
		}
		
		if (game.firing) Fire();
		
		map<Player::Id,ObjectHandle>::iterator pit;
		for (pit = game.players.begin(); pit != game.players.end(); ++pit)
			TO(Player,pit->second)->frame();
		
		game.world->terrain->frame();
		
		map<GridPoint,ObjectHandle>::iterator bit;
		Terrain *t = game.world->terrain;
		for (bit = t->structures.begin(); bit != t->structures.end(); ++bit)
		{
			Building *b = TO(Building, bit->second);
			if (b) b->frame();
		}
		
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
		if(!p || p == Game::game.player) continue;
		p->interpolate();
	}
}

//------------------------------------------------------------------------------
