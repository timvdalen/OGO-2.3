#ifndef _MATERIALS_H
#define _MATERIALS_H

#include "core.h"
#include "video.h"

//! Contains material classes
namespace Materials{

//------------------------------------------------------------------------------

//! A material that is used in a lighted environment
class ShadedMaterial : public Material {
	private:
		//! Initialize the material
		void init(const GLfloat _shininess,
			       	const GLfloat _ambient[4],
			       	const GLfloat _diffuse[4],
			       	const GLfloat _specular[4],
			       	const GLfloat _emissive[4]);

	public:
		//! The shininess of this material
		GLfloat shininess;

		//! The ambient light reflection
		GLfloat ambient[4];

		//! The diffuse light reflection
		GLfloat diffuse[4];

		//! The specular light reflection
		GLfloat specular[4];

		//! The emissive light reflection
		GLfloat emissive[4]; 

		//! Constructs a ShadedMaterial based on GLfloats
		ShadedMaterial(const GLfloat _shininess,
			       	const GLfloat _ambient1,
			       	const GLfloat _ambient2,
			       	const GLfloat _ambient3,
			       	const GLfloat _ambient4,
			     	const GLfloat _diffuse1,
			       	const GLfloat _diffuse2,
			       	const GLfloat _diffuse3,
			       	const GLfloat _diffuse4,
			     	const GLfloat _specular1,
				const GLfloat _specular2,
				const GLfloat _specular3,
				const GLfloat _specular4,
			     	const GLfloat _emissive1,
				const GLfloat _emissive2,
				const GLfloat _emissive3,
				const GLfloat _emissive4);

		//! Constructs a ShadedMaterial based on GLfloat arrays
		ShadedMaterial(const GLfloat _shininess,
			       	const GLfloat _ambient[4],
			       	const GLfloat _diffuse[4],
			       	const GLfloat _specular[4],
			       	const GLfloat _emissive[4]);

		//! Constructs a ShadedMaterial based on M
		ShadedMaterial(const ShadedMaterial &M);

		//! Applies this material to the OpenGL pipeline
		virtual void select();
};

//------------------------------------------------------------------------------

//! A material that is used to clear the previous material settings
//! If GL_BLEND is enabled and glBlendFunc GL_SRC_ALPHA is set to 
//! GL_ONE_MINUS_SRC_ALPHA this will cause objects to be transparent.
class EmptyMaterial : public ShadedMaterial {
	public:
		EmptyMaterial() : ShadedMaterial(0.0f, //Shininess
				0.0f, 0.0f, 0.0f, 0.0f, //Ambient
				0.0f, 0.0f, 0.0f, 0.0f, //Diffuse
				0.0f, 0.0f, 0.0f, 0.0f, //Specular
				0.0f, 0.0f, 0.0f, 0.0f) //Emissive
       		{}
};

}

#endif  /* _MATERIALS_H */
