namespace Materials{

//------------------------------------------------------------------------------

ShadedMaterial::ShadedMaterial(const GLfloat _shininess,
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
		const GLfloat _emissive4)
{
	GLfloat _ambient[4] = { _ambient1, _ambient2, _ambient3, _ambient4 };
	GLfloat _diffuse[4] = { _diffuse1, _diffuse2, _diffuse3, _diffuse4 };
	GLfloat _specular[4] = { _specular1, _specular2, _specular3, _specular4 };
	GLfloat _emissive[4] = { _emissive1, _emissive2, _emissive3, _emissive4 };
	init(_shininess, _ambient, _diffuse, _specular, _emissive);
}

//------------------------------------------------------------------------------

ShadedMaterial::ShadedMaterial(const GLfloat _shininess, 
		const GLfloat _ambient[4], 
		const GLfloat _diffuse[4], 
		const GLfloat _specular[4], 
		const GLfloat _emissive[4]) :
	shininess(), ambient(), diffuse(), specular(), emissive()
{
	init(_shininess, _ambient, _diffuse, _specular, _emissive);
}

//------------------------------------------------------------------------------

ShadedMaterial::ShadedMaterial(const ShadedMaterial &M) :
	shininess(), ambient(), diffuse(), specular(), emissive()
{
	init(M.shininess, M.ambient, M.diffuse, M.specular, M.emissive);
}

//------------------------------------------------------------------------------

void ShadedMaterial::init(const GLfloat _shininess, 
		const GLfloat _ambient[4], 
		const GLfloat _diffuse[4], 
		const GLfloat _specular[4], 
		const GLfloat _emissive[4])
{
	this->shininess = _shininess;
	for(int i=0; i < 4; i++){
		this->ambient[i] = _ambient[i];
		this->diffuse[i] = _diffuse[i];
		this->specular[i] = _specular[i];
		this->emissive[i] = _emissive[i];
	}
}

//------------------------------------------------------------------------------

void ShadedMaterial::select(){
	glMaterialf(GL_FRONT, GL_SHININESS, this->shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, this->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, this->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, this->specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, this->emissive);
}

}
