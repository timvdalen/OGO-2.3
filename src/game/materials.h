
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

#include <string>

#include "base.h"
#include "core.h"
#include "image.h"
#include "video.h"

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
	
	//! Cleans up the material from the OpenGL pipeline
	virtual void unselect();
};

//------------------------------------------------------------------------------

//! Textured material base class
class TexturedMaterial : public Material
{
	public:
	//! Constructs a material with supplied texture file
	//! \note Currently only supports PNG files
	TexturedMaterial(std::string filename);
	
	//! Constructs a material with supplied texture image
	//! \note The texture data is stored in video memory, so it is save to
	//!       destroy the image after construction the material
	TexturedMaterial(const Image &);
	
	//! Cleans up the texture data
	virtual ~TexturedMaterial();
	
	//! Applies this material to the OpenGL pipeline
	virtual void select();
	
	//! Cleans up the texture binding
	virtual void unselect();
	uword id() { return texture->id; }
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
	
	//! Cleans up the material from the OpenGL pipeline
	virtual void unselect() { second->unselect(); first->unselect(); }
};

//------------------------------------------------------------------------------

//! A material that is used to clear the previous material settings.

//! If GL_BLEND is enabled and glBlendFunc GL_SRC_ALPHA is set to 
//! GL_ONE_MINUS_SRC_ALPHA this will cause objects to be transparent.
class EmptyMaterial : public Material {
	public:
		EmptyMaterial() {}

		virtual void select();
};

//------------------------------------------------------------------------------

//! A material for the grid.
class GridMaterial : public Material
{
	uword size;
	
	public:
	GridMaterial(uword S = 10) : size(S) {}
	
	virtual void select();
	virtual void unselect();
};

//------------------------------------------------------------------------------

class ColorMaterial: public Material{
	public:
	//! Red part of the color
	GLfloat r;
		
	//! Green part of the color
	GLfloat g;

	//! Blue part of the color
	GLfloat b;

	//! Alpha part of the color
	GLfloat a;

	//! Constructs a new ColorMaterial
	ColorMaterial(GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a);

	//! Selects this material
	virtual void select();

	//! Unselects this material
	virtual void unselect();
};

//------------------------------------------------------------------------------

} // namespace Material

#endif // _MATERIAL_H

//------------------------------------------------------------------------------

