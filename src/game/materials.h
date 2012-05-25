
#include "core.h"
#include "video.h"

//! Contains material classes
namespace Material{

//------------------------------------------------------------------------------

class ShadedMaterial : Material{
	public:
		//! The shininess of this material
		GLfloat shininess;

		//! The ambient light reflection
		GLfloat *ambient[4];

		//! The diffuse light reflection
		GLfloat *diffuse[4];

		//! The specular light reflection
		GLfloat *specular[4];

		//! The emissive light reflection
		GLfloat *emissive[4]; 

		//! Constructs an empty Material
		ShadedMaterial() {}

		//! Constructs a Material based on M
		ShadedMaterial(Material M) {}

		//! Applies this material to the OpenGL pipeline
		virtual void select() {}
};

}
