#include "loader3ds.h"

//#include "core.h"
//#include "base.h"

#if defined _WIN32
	#define WIN32_LEAN_AND_MEAN 1
	#include <windows.h>
	#include <gl\gl.h>
	#include <gl\glu.h>
	#include <gl\glext.h>
#elif defined __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <3ds/lib3ds.h>

//--------------------------------------------

namespace Loader {

using namespace Base;
using namespace Core;

//--------------------------------------------

typedef float Vertex[3];

void ModelObject::load(const char * path) {
	file = lib3ds_file_open(path);
	
	//Vertex * vertices = new Vertex[file->meshes_size * 3];
	//Vertex * normals = new Vertex[file->meshes_size * 3];

	
}

//--------------------------------------------

typedef struct {
    GLuint tex_id;
    GLint w;
    GLint h;
    unsigned char *pixels;
} Texture;

void ModelObject::displaylist(Lib3dsMesh * mesh) {
            {
                int p;
                float (*normalL)[3] = (float(*)[3])malloc(3 * 3 * sizeof(float) * mesh->nfaces);
                Lib3dsMaterial *oldmat = (Lib3dsMaterial *) - 1;
                {
                    float M[4][4];
                    lib3ds_matrix_copy(M, mesh->matrix);
                    lib3ds_matrix_inv(M);
                    glMultMatrixf(&M[0][0]);
                }
                lib3ds_mesh_calculate_vertex_normals(mesh, normalL);

                for (p = 0; p < mesh->nfaces; ++p) {
                    Lib3dsMaterial *mat = 0;

                    if (mesh->faces[p].material >= 0) {
                        mat = file->materials[mesh->faces[p].material];
                    }

                    if (mat != oldmat) {
                        if (mat) {
                            if (mat->texture1_map.user_ptr) {
                                Texture* pt = (Texture*)mat->texture1_map.user_ptr;
                                glEnable(GL_TEXTURE_2D);
                                glBindTexture(GL_TEXTURE_2D, pt->tex_id);
                            } else {
                                glDisable(GL_TEXTURE_2D);
                            }

                            {
                                float a[4], d[4], s[4];
                                int i;
                                for (i=0; i<3; ++i) {
                                    a[i] = mat->ambient[i];
                                    d[i] = mat->diffuse[i];
                                    s[i] = mat->specular[i];
                                }
                                a[3] = d[3] = s[3] = 1.0f;
                                
                                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                                glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                            }
                            float shininess = pow(2, 10.0*mat->shininess);
                            glMaterialf(GL_FRONT, GL_SHININESS, shininess <= 128? shininess : 128);
                        } else {
                            static const float a[4] = {0.7, 0.7, 0.7, 1.0};
                            static const float d[4] = {0.7, 0.7, 0.7, 1.0};
                            static const float s[4] = {1.0, 1.0, 1.0, 1.0};
                            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                            glMaterialf(GL_FRONT, GL_SHININESS, pow(2, 10.0*0.5));
                        }
                        oldmat = mat;
                    }

                    {
                        /*{
                            float v1[3], n[3], v2[3];
                            glBegin(GL_LINES);
                            for (i = 0; i < 3; ++i) {
                                lib3ds_vector_copy(v1, mesh->vertices[f->points[i]]);
                                glVertex3fv(v1);
                                lib3ds_vector_copy(n, normalL[3*p+i]);
                                lib3ds_vector_scalar(n, 10.f);
                                lib3ds_vector_add(v2, v1, n);
                                glVertex3fv(v2);
                            }
                            glEnd();
                        }*/

                        glBegin(GL_TRIANGLES);
                        for (int i = 0; i < 3; ++i) {
                            glNormal3fv(normalL[3*p+i]);
							
                            if (mat->texture1_map.user_ptr) {
                                glTexCoord2f(
                                    mesh->texcos[mesh->faces[p].index[i]][0],
                                    1-mesh->texcos[mesh->faces[p].index[i]][1] );
                            }

                            glVertex3fv(mesh->vertices[mesh->faces[p].index[i]]);
                        }
                        glEnd();
                    }
                }

                free(normalL);
            }

            glDisable(GL_TEXTURE_2D);
}

//--------------------------------------------

void ModelObject::render(Lib3dsNode * n) {
    for (Lib3dsNode * nc = n->childs; !nc; nc = nc->next) {
        render(nc);
    }

	if (n->type == LIB3DS_NODE_MESH_INSTANCE) {
		Lib3dsMeshInstanceNode * node = (Lib3dsMeshInstanceNode*) n;
		Lib3dsMesh * mesh;
		for (int i = 0; i < file->meshes_size; i++) {
			mesh = file->meshes[i];

			if (!mesh->user_id) {
				mesh->user_id = glGenLists(1);
				glNewList(mesh->user_id, GL_COMPILE);

				displaylist(mesh);

				glEndList();
			} else {
				glPushMatrix();
				glMultMatrixf(&n->matrix[0][0]);
				glTranslatef(-node->pivot[0], -node->pivot[1], -node->pivot[2]);
				glCallList(mesh->user_id);
				glPopMatrix();
			}
		}
	}
}

//--------------------------------------------

ModelObject::ModelObject(Point<double> P = Point<double>(), Quaternion<double> R = Quaternion<double>()) : Object(P, R) {}

//--------------------------------------------

ModelObject::~ModelObject() {}

//--------------------------------------------

void ModelObject::render() {
	for (Lib3dsNode * n = file->nodes; n; n = n->next) {
		render(n);
	}
}

//--------------------------------------------

} // namespace Loader