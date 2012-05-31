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
#include <string>
#include <iostream>

#define PROBE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

//--------------------------------------------

namespace Loader {

using namespace Base;
using namespace Core;

//--------------------------------------------

typedef float Vertex[3];


typedef struct {
    GLuint tex_id;
    GLint w;
    GLint h;
    unsigned char *pixels;
} Texture;

//--------------------------------------------

void ModelObject::load(const char * path) {
	file = lib3ds_file_open(path);
	
	//Vertex * vertices = new Vertex[file->meshes_size * 3];
	//Vertex * normals = new Vertex[file->meshes_size * 3];

    for (int i = 0; i < file->nmaterials; ++i) {
        Lib3dsMaterial *mat = file->materials[i];
        if (mat->texture1_map.name[0]) {  // texture map?
            Lib3dsTextureMap *tex = &mat->texture1_map;

            char texname[1024];
            Texture *pt = (Texture*)calloc(sizeof(*pt),1);
            tex->user_ptr = pt;
            strcpy(texname, path);
            strcat(texname, "/");
            strcat(texname, tex->name);

            printf("Loading %s\n", texname);
            //if (tga_load(texname, &pt->pixels, &pt->w, &pt->h)) {
			if (true) {
                glGenTextures(1, &pt->tex_id);

                glBindTexture(GL_TEXTURE_2D, pt->tex_id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pt->w, pt->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pt->pixels);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            } else {
                fprintf(stderr, "Loading '%s' failed!\n", texname);
            }
        }
    }//*/
}

//--------------------------------------------

void ModelObject::displaylist(Lib3dsMesh * mesh) {
	// cout << "begin displaylist";
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
                    if (mesh->faces[p].material >= 0) mat = file->materials[mesh->faces[p].material];

                    if (mat != oldmat) {
					// cout << "\n OMFG WTF BBQ WHY THE FUCK DOES THIS FOLLOWING LINE CAUSE AN ACCESS VIOLATION \n";
					//system("PAUSE");
                        if (mat) {
					puts("test");
					fflush(stdout);

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
							// cout << "\n else started";
                            static const float a[4] = {0.7, 0.7, 0.7, 1.0};
                            static const float d[4] = {0.7, 0.7, 0.7, 1.0};
                            static const float s[4] = {1.0, 1.0, 1.0, 1.0};
                            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                            glMaterialf(GL_FRONT, GL_SHININESS, pow(2, 10.0*0.5));
							// cout << "\n else executed";
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

						// cout << "\n begin triangles";
                        glBegin(GL_TRIANGLES);
                        for (int i = 0; i < 3; ++i) {
                            glNormal3fv(normalL[3*p+i]);
							
							// cout << "\n begin texture mapping";
							//if (!mat) // cout << "NULL!!!";
							if (mat && mat->texture1_map.user_ptr) {
								// cout << "\n texture mapping";
                                glTexCoord2f(
                                    mesh->texcos[mesh->faces[p].index[i]][0],
                                    1-mesh->texcos[mesh->faces[p].index[i]][1] );
                            }
							// cout << "\n end texture mapping";
							//float * a;
							//a = mesh->vertices[mesh->faces[p].index[i]];
							//cout << a[0] << "," << a[1] << "," << a[2] << "\n";
                            glVertex3fv(mesh->vertices[mesh->faces[p].index[i]]);
                        }
						cout << "\n";
                        glEnd();
						// cout << "\n end triangles";
                    }
                }

                free(normalL);
            }

            glDisable(GL_TEXTURE_2D);

	// cout << "end displaylist";
}

//--------------------------------------------

void ModelObject::render(Lib3dsNode * n) {
	// cout << "render " << n->node_id;

    for (Lib3dsNode * nc = n->childs; nc; nc = nc->next) {
        render(nc);
    }

	if (n->type == LIB3DS_NODE_MESH_INSTANCE) {
		//cout << "mesh found";

		Lib3dsMeshInstanceNode * node = (Lib3dsMeshInstanceNode*) n;
		Lib3dsMesh * mesh;

		// cout << "\n mesh size: " << file->meshes_size <<"\n";

		for (int i = 0; i < file->meshes_size; i++) {
			mesh = file->meshes[i];

			//cout << "start mesh rendering id: " << mesh->user_id;

			if (!mesh->user_id) {
				mesh->user_id = glGenLists(1);
				//cout << "displaylist id: " << mesh->user_id;
				glNewList(mesh->user_id, GL_COMPILE);

				glPushMatrix();
				//glMultMatrixf(&n->matrix[0][0]);
				glTranslatef(-node->pivot[0], -node->pivot[1], -node->pivot[2]);
				glMultMatrixf(&mesh->matrix[0][0]);
				glTranslatef(node->pos[0], node->pos[1], node->pos[2]);
				//glRotatef(node->rot[0], node->rot[1], node->rot[2], node->rot[3]);

				displaylist(mesh);

				glPopMatrix();

				glEndList();
			} else {
				glCallList(mesh->user_id);
				glFlush();
			}
		}
	}
}

//--------------------------------------------

ModelObject::ModelObject(Point<double> P, Quaternion<double> R, string path) : Object(P, R) {
	load(path.c_str());
	// // cout << path;
}

//--------------------------------------------

ModelObject::~ModelObject() {}

//--------------------------------------------

void ModelObject::render() {
	if (file) {
		// cout << file->nodes->name << " <- nodename, followed by -> " << file->nodes->next->name;
		Lib3dsNode * n;
		for (n = file->nodes; n; n = n->next) {
			render(n);
			PROBE
		}
		cout << "rendered. \n";
	}
	else {
		// // cout << "file not found...";
	}
}

//--------------------------------------------

} // namespace Loader