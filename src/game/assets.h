/*
 * Assets definitions
 *
 * Date: 04-06-12 14:06
 *
 * Description: Definition of the artwork and assets used by the game.
 *
 */

#ifndef _ASSETS_H
#define _ASSETS_H

#include "core.h"

namespace Assets {

using namespace Core;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[]);
void Terminate();

//------------------------------------------------------------------------------

extern MaterialHandle Grid;
extern MaterialHandle SelectedGrid;
extern MaterialHandle ErrorGrid;

extern MaterialHandle WorldMaterial;
extern MaterialHandle Star;

extern MaterialHandle Test;

extern MaterialHandle Cloud;
extern MaterialHandle Grass;

extern MaterialHandle Widget;
extern MaterialHandle WidgetBG;
extern MaterialHandle Font;
extern MaterialHandle CrossHair;

namespace Icon{

extern MaterialHandle Money;
extern MaterialHandle Health;
extern MaterialHandle Mine;
extern MaterialHandle Robot_red;    
extern MaterialHandle Robot_blue;
extern MaterialHandle Robot_normal;
extern MaterialHandle Pickaxe_normal;
extern MaterialHandle Pickaxe_blue;    
extern MaterialHandle Pickaxe_red;
extern MaterialHandle Tower_normal;
extern MaterialHandle Tower_red;
extern MaterialHandle Tower_blue;
extern MaterialHandle HQ_normal;
extern MaterialHandle HQ_blue;
extern MaterialHandle HQ_red;

}

//------------------------------------------------------------------------------

namespace Model {

//player objects
extern ObjectHandle HeadObj, BodyObj, GunObj, WrenchObj, WheelObj;
extern MaterialHandle HeadTex, BodyTex[2], GunTex, WrenchTex, WheelTex[2];

//structure objects
extern ObjectHandle TurretObj, MineObj, DrillObj, RockObj;
extern MaterialHandle TurretTex[3], MineTex[2], DrillTex[2], RockTex;
extern MaterialHandle GhostTurretTex;

}

//------------------------------------------------------------------------------

} // namespace Assets

#endif // _ASSETS_H

//------------------------------------------------------------------------------
