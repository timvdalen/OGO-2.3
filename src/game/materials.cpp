/*
 * Materials -- see header file for more info
 */

#include "video.h"
#include "materials.h"

namespace Materials {

//------------------------------------------------------------------------------

void ShadedMaterial::select()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

//------------------------------------------------------------------------------

void ShadedMaterial::unselect()
{
	Cf white(1,1,1,1);
	Cf empty(0,0,0,0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_EMISSION, empty);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
}

//==============================================================================

TexturedMaterial::TexturedMaterial(string filename)
{
	PngImage image(filename.c_str());
	texture = Texture();
	texture->load(image);
}

//------------------------------------------------------------------------------

TexturedMaterial::TexturedMaterial(const Image &image)
{
	texture = Texture();
	texture->load(image);
}

//------------------------------------------------------------------------------

TexturedMaterial::~TexturedMaterial()
{
	texture.clear();
}

//------------------------------------------------------------------------------

void TexturedMaterial::select()
{
	if (!texture->id) return;
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture->id);
}

//------------------------------------------------------------------------------

void TexturedMaterial::unselect()
{
	glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

TexturedMaterial::Texture::~Texture()
{
	if (id) glDeleteTextures(1, (const GLuint *) &id);
}

//------------------------------------------------------------------------------

void TexturedMaterial::Texture::load(const Image &image)
{
	const int IFORMAT[] = {0, GL_LUMINANCE8, GL_LUMINANCE8_ALPHA8, GL_RGB8, GL_RGBA8};
	const int FORMAT[] = {0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};
	id = 0;
	
	if (!image.data) return;
	
	glGenTextures(1, (GLuint *) &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Push alignment
	int pack;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &pack);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	// Create texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, IFORMAT[image.depth],
	                  image.width, image.height, FORMAT[image.depth],
					  GL_UNSIGNED_BYTE, image.data);
	
	// Pop alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, pack);
}

//==============================================================================

void EmptyMaterial::select()
{
	GLfloat empty[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, empty);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, empty);
	glMaterialfv(GL_FRONT, GL_SPECULAR, empty);
	glMaterialfv(GL_FRONT, GL_EMISSION, empty);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
}

//==============================================================================

void LineMaterial::select()
{
	glLineWidth(size);
}

//------------------------------------------------------------------------------

void LineMaterial::unselect()
{
	glLineWidth(1);
}

//------------------------------------------------------------------------------

ColorMaterial::ColorMaterial(GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a){
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

//------------------------------------------------------------------------------

void ColorMaterial::select(){
	glColor4f(r, g, b, a);
}

//------------------------------------------------------------------------------

void ColorMaterial::unselect(){
	glColor4f(1,1,1,1);
}

//------------------------------------------------------------------------------

} // namespace Materials

//------------------------------------------------------------------------------
