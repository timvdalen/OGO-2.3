
/*
 * Materials
 *
 * Date: 24-5-2012
 *
 * Description:
 *
 */

#ifndef _MATERIALS_H
#define _MATERIALS_H

#include "base.h"
#include "core.h"
#include "image.h"

//! Contains material classes
namespace Materials {

using namespace Core;
using namespace Base::Alias;
using namespace Loader;

//------------------------------------------------------------------------------

//! Blinn shaded material base class
class ShadedMaterial : public Material {
	public:
	//! The shininess of this material
	float shininess;

	//! The ambient light reflection
	Color<float> ambient;

	//! The diffuse light reflection
	Color<float> diffuse;

	//! The specular light reflection
	Color<float> specular;

	//! The emissive light reflection
	Color<float> emissive; 

	//! Constructs an empty Material
	ShadedMaterial(const Cf &C = Cf(1,1,1,1))
		: shininess(50.0), ambient(C), diffuse(C), specular(C), emissive(Cf(0,0,0,0)) {}
	ShadedMaterial(const Cf &A, const Cf &D, const Cf &S, const Cf &E, float Y)
		: shininess(Y), ambient(A), diffuse(D), specular(S), emissive(E) {}

	//! Applies this material to the OpenGL pipeline
	virtual void select();
};

//------------------------------------------------------------------------------

//! Textured material base class
class TexturedMaterial : public Material
{
	public:
	//! Constructs a material with supplied texture file
	//! \note Currently only supports PNG files
	TexturedMaterial(const char *filename);
	
	//! Constructs a material with supplied texture image
	//! \note The texture data is stored in video memory, so it is save to
	//!       destroy the image after construction the material
	TexturedMaterial(const Image &);
	
	//! Cleans up the texture data
	virtual ~TexturedMaterial();
	
	//! Applies this material to the OpenGL pipeline
	virtual void select();
	
	private:
	struct Texture
	{
		udword id;
		Texture() : id(0) {}
		~Texture();
		void load(const Image &);
	};
	Handle<Texture> texture;
};

//------------------------------------------------------------------------------

//! Material that allows two materials to be combined
class TwinMaterial : public Material
{
	public:
	MaterialHandle first;  //!< First material that will be applied
	MaterialHandle second; //!< Second material that will be applied
	
	//! Constructs the material from two other materials
	TwinMaterial(MaterialHandle F, MaterialHandle S) : first(F), second(S) {}
	
	//! Applies the materials to the OpenGL pipeline
	virtual void select() { first->select(); second->select(); }
};

//------------------------------------------------------------------------------

//! A material that is used to clear the previous material settings.

//! If GL_BLEND is enabled and glBlendFunc GL_SRC_ALPHA is set to 
//! GL_ONE_MINUS_SRC_ALPHA this will cause objects to be transparent.
class EmptyMaterial : public ShadedMaterial {
	public:
		EmptyMaterial() : ShadedMaterial(C(0,0,0,0), 	//Ambient
						 C(0,0,0,0), 	//Diffuse
						 C(0,0,0,0), 	//Specular
						 C(0,0,0,0), 	//Emissive,
						 0.0)		//Shininess
       		{}
};

//------------------------------------------------------------------------------

} // namespace Material

#endif // _MATERIAL_H

//------------------------------------------------------------------------------

