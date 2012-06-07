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

MaterialHandle Test;
MaterialHandle Cloud;
MaterialHandle Grass;

MaterialHandle Head;
MaterialHandle Body;
MaterialHandle Gun;

MaterialHandle Widget;
MaterialHandle Font;
MaterialHandle CrossHair;

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
		if (!strcmp(argv[i], "-p"))
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

	Head = TexturedMaterial(path + "assets/textures/models/headtext.png");
	Body = TexturedMaterial(path + "assets/textures/models/robottex.png");
	Gun = TexturedMaterial(path + "assets/textures/models/guntextu.png");

	Widget =  ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
	Font = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);
	CrossHair = ColorMaterial(1.0f, 1.0f, 1.0f, 1.0f);


	HeadObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/head.3DS");
	BodyObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/body.3DS");
	GunObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/gun.3DS");
	WrenchObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/wrench.3DS");
	WheelObj = Loader::ModelObject(Pd(), Qd(), path + "assets/models/wheel.3DS");
}

//------------------------------------------------------------------------------

void Terminate()
{
	Grid.clear();
	SelectedGrid.clear();
	Test.clear();
	Cloud.clear();
	Grass.clear();

	Head.clear();
	Body.clear();
	Gun.clear();

	CrossHair.clear();

	HeadObj.clear();
	BodyObj.clear();
	GunObj.clear();
	WrenchObj.clear();
	WheelObj.clear();
}

//------------------------------------------------------------------------------

} // namespace Assets

//------------------------------------------------------------------------------
