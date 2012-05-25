namespace Materials{

//------------------------------------------------------------------------------

class ShadedMaterial : Material{
	public:
		ShadedMaterial(Material M){
			this.shininess = M.shininess;
			this.ambient = M.ambient;
			this.diffuse = M.diffuse;
			this.specular = M.specular;
			this.emissive = M.emissive;
		}

		virtual void select(){
			glMaterialf(GL_FRONT, GL_SHININESS, this.shininess);
			glMaterialfv(GL_FRONT, GL_AMBIENT, this.ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, this.diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, this.specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, this.emissive);
		}
};

}
