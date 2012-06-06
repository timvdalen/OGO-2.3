/*
 * Assets definitions -- see header file for more info
 */

#include <string.h>

#include <string>

#include "materials.h"
#include "assets.h"

namespace Assets {

using namespace std;

//------------------------------------------------------------------------------

using namespace Materials;

MaterialHandle Grid;
MaterialHandle SelectedGrid;

MaterialHandle Test;
MaterialHandle Cloud;
MaterialHandle Grass;

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
		Test = TwinMaterial(shade, TexturedMaterial("test.png"));
	}
	
	Cloud = TexturedMaterial(path + "assets/textures/world/cloud.png");
	Grass = TexturedMaterial(path + "assets/textures/terrain/grass.png");
}

//------------------------------------------------------------------------------

void Terminate()
{
	Grid.clear();
	SelectedGrid.clear();
	Test.clear();
	Cloud.clear();
	Grass.clear();
}

//------------------------------------------------------------------------------

} // namespace Assets

//------------------------------------------------------------------------------
