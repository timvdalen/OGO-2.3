/*
 * Assets definitions -- see header file for more info
 */

#include <string.h>

#include <string>

#include "materials.h"
#include "assets.h"
#include "loader3ds.h"

namespace Assets {

using namespace std;

//------------------------------------------------------------------------------

using namespace Materials;

MaterialHandle Grid;
MaterialHandle SelectedGrid;

MaterialHandle WorldMaterial;
MaterialHandle Star;

MaterialHandle Test;
MaterialHandle Cloud;
MaterialHandle Grass;

MaterialHandle Widget;
MaterialHandle WidgetBG;
MaterialHandle Font;
MaterialHandle CrossHair;
    
namespace Icon{ 
   
MaterialHandle Money;
MaterialHandle Health;
MaterialHandle Mine;
MaterialHandle Robot_red;    
MaterialHandle Robot_blue;
MaterialHandle Robot_normal;
MaterialHandle Pickaxe_normal;
MaterialHandle Pickaxe_blue;    
MaterialHandle Pickaxe_red;
MaterialHandle Tower_normal;
MaterialHandle Tower_red;
MaterialHandle Tower_blue;
MaterialHandle HQ_normal;
MaterialHandle HQ_blue;
MaterialHandle HQ_red;

}

//------------------------------------------------------------------------------

ObjectHandle HeadObj;
ObjectHandle BodyObj;
ObjectHandle GunObj;
ObjectHandle WrenchObj;
ObjectHandle WheelObj;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[])
{
	string path = "./";
	for (int i = 0; i < argc - 1; ++i)
		if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--path"))
			path = argv[++i];
	
	Grid = TwinMaterial(GridMaterial(5),
		ShadedMaterial(Cf(0.2,0.8,0.2,1), //Ambient
	                   Cf(0.2,0.8,0.2,1), //Diffuse
	                   Cf(0.2,0.8,0.2,1), //Specular
	                   Cf(0.8,1,0,1),     //Emissive
	                   100.0));           //Shininess
	
	SelectedGrid = TwinMaterial(GridMaterial(10),
		ShadedMaterial(Cf(0.01,0.31,0.58,1), //Ambient
	                   Cf(0.01,0.31,0.58,1), //Diffuse
	                   Cf(0.01,0.31,0.58,1), //Specular
	                   Cf(0.49,0.97,1,1),    //Emissive,
	                   100.0));              //Shininess

	{
		ShadedMaterial shade(Cf(1,0,0,1));
		shade.emissive = Cf(0,.5,0,1);
		Test = TwinMaterial(shade, TexturedMaterial(path + "test.png"));
	}
	
	Cloud = TexturedMaterial(path + "assets/textures/world/cloud.png");
	Grass = TexturedMaterial(path + "assets/textures/terrain/grass.png");

	WorldMaterial = TwinMaterial(Cloud, ShadedMaterial(Cf(0.5, 0.5, 0.5, 1.0)));
	
	{
		ShadedMaterial light(Cf(1, 1, 1, 1));
		light.emissive = Cf(1, 1, 1, 1);
		Star = light;	
	}

	Widget =  ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
	WidgetBG = ColorMaterial(0.184f, 0.31f, 0.31f, 0.7f);
	Font = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
	CrossHair = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
    
    Icon::Money = TexturedMaterial(path + "assets/textures/hud/money.png");
    Icon::Health = TexturedMaterial(path+ "assets/textures/hud/health.png");
    Icon::Mine = TexturedMaterial(path + "assets/textures/hud/mine.png");
    Icon::Robot_red = TexturedMaterial(path + "assets/textures/hud/robot_red.png");
    Icon::Robot_blue = TexturedMaterial(path + "assets/textures/hud/robot_blue.png");
    Icon::Robot_normal = TexturedMaterial(path + "assets/textures/hud/robot_normal.png");
    Icon::Pickaxe_normal = TexturedMaterial(path + "assets/textures/hud/pickaxe_normal.png");
    Icon::Pickaxe_blue = TexturedMaterial(path + "assets/textures/hud/pickaxe_blue.png");
    Icon::Pickaxe_red = TexturedMaterial(path + "assets/textures/hud/pickaxe_red.png");
    Icon::Tower_normal = TexturedMaterial(path + "assets/textures/hud/tower_normal.png");
    Icon::Tower_red = TexturedMaterial(path + "assets/textures/hud/tower_red.png");
    Icon::HQ_normal = TexturedMaterial(path + "assets/textures/hud/headquarter_normal.png");
    Icon::HQ_blue = TexturedMaterial(path + "assets/textures/hud/headquarter_blue.png");
    Icon::HQ_red = TexturedMaterial(path + "assets/textures/hud/headquarter_red.png");
    
	HeadObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/head.3DS");
	BodyObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/body.3DS");
	GunObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/gun.3DS");
	WrenchObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/wrench.3DS");
	WheelObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/wheel.3DS");
	
	HeadObj->material = TexturedMaterial(path + "assets/textures/models/headtext.png");
	BodyObj->material = TexturedMaterial(path + "assets/textures/models/robottex.png");
	GunObj->material = TexturedMaterial(path + "assets/textures/models/guntextu.png");
}

//------------------------------------------------------------------------------

void Terminate()
{
	Grid.clear();
	SelectedGrid.clear();
	Test.clear();
	Cloud.clear();
	Grass.clear();

	WorldMaterial.clear();

	Widget.clear();
	Font.clear();
	CrossHair.clear();
	
	Icon::Money.clear();
    Icon::Health.clear();
	Icon::Mine.clear();
    Icon::Robot_red.clear();
    Icon::Robot_blue.clear();
    Icon::Robot_normal.clear();
    Icon::Pickaxe_normal.clear();
    Icon::Pickaxe_blue.clear();
    Icon::Pickaxe_red.clear();
    Icon::Tower_normal.clear();
    Icon::Tower_red.clear();
    Icon::HQ_normal.clear();
    Icon::HQ_blue.clear();
    Icon::HQ_red.clear();

	HeadObj.clear();
	BodyObj.clear();
	GunObj.clear();
	WrenchObj.clear();
	WheelObj.clear();
}

//------------------------------------------------------------------------------

} // namespace Assets

//------------------------------------------------------------------------------
