
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


//! Contains material classes
namespace Materials {

using namespace Core;
using namespace Base::Alias;

//------------------------------------------------------------------------------

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

} // namespace Material

#endif // _MATERIAL_H

//------------------------------------------------------------------------------

