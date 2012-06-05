/*
 * Assets definitions -- see header file for more info
 */

#include "materials.h"
#include "assets.h"

namespace Assets {

using namespace Materials;

//------------------------------------------------------------------------------

MaterialHandle Test;
MaterialHandle Cloud;
MaterialHandle Grass;

//------------------------------------------------------------------------------

void Initialize()
{
	ShadedMaterial shade(Cf(1,0,0,1));
	shade.emissive = Cf(0,.5,0,1);
	Test = TwinMaterial(shade, TexturedMaterial("test.png"));
	
	Cloud = TexturedMaterial("assets/textures/world/cloud.png");
	Grass = TwinMaterial(TexturedMaterial("assets/textures/terrain/grass.png"),
	                     ShadedMaterial(Cf(1, 1, 1, 1)));
}

//------------------------------------------------------------------------------

void Terminate()
{
	Test.clear();
	Cloud.clear();
	Grass.clear();
}

//------------------------------------------------------------------------------

} // namespace Assets

//------------------------------------------------------------------------------
