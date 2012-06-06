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

#include <lib3ds.h>
#include <string>
#include <iostream>

#define PROBE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

//--------------------------------------------

namespace Loader {

using namespace Base;
using namespace Core;

//--------------------------------------------

typedef float Vertex[3];

//--------------------------------------------

void ModelObject::load(const char * path) {
	file = lib3ds_file_open(path);
}

//--------------------------------------------

void ModelObject::displaylist(Lib3dsMesh * mesh) {
    /*
            {
                int p;

                //float (*normalL)[3] = (float(*)[3])malloc(3 * 3 * sizeof(float) * mesh->nfaces);
				
				//Replacement code "malloc"
				float * normalL0 = new float[3 * mesh->nfaces];
				float * normalL1 = new float[3 * mesh->nfaces];
				float * normalL2 = new float[3 * mesh->nfaces];	
				float n[3] = {*normalL0, *normalL1, *normalL2};
				float (*normalL)[3] = &n;

                Lib3dsMaterial *oldmat = (Lib3dsMaterial *) - 1;
                {
                    float M[4][4];
                    lib3ds_matrix_copy(M, mesh->matrix);
                    lib3ds_matrix_inv(M);
                    glMultMatrixf(&M[0][0]);
                }
                lib3ds_mesh_calculate_vertex_normals(mesh, normalL);

                for (p = 0; p < mesh->nfaces; ++p) {
                    {
                        glBegin(GL_TRIANGLES);
                        for (int i = 0; i < 3; ++i) {
                            glNormal3fv(normalL[3*p+i]);
                            glTexCoord2f(
                                mesh->texcos[mesh->faces[p].index[i]][0],
                                1-mesh->texcos[mesh->faces[p].index[i]][1] );
                            glVertex3fv(mesh->vertices[mesh->faces[p].index[i]]);
                        }
                        glEnd();
                    }
                }

                free(normalL);
            }

            glDisable(GL_TEXTURE_2D);
    */
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

			if (!mesh->user_id) {
				mesh->user_id = glGenLists(1);
				//cout << "displaylist id: " << mesh->user_id;
				glNewList(mesh->user_id, GL_COMPILE);

				glPushMatrix();
				//glMultMatrixf(&n->matrix[0][0]);
				//glTranslatef(-node->pivot[0], -node->pivot[1], -node->pivot[2]);
				glMultMatrixf(&mesh->matrix[0][0]);
				glTranslatef(-node->pivot[0], -node->pivot[1], -node->pivot[2]);
				//glTranslatef(node->pos[0], node->pos[1], node->pos[2]);
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
	cout << "Opening " << path << "\n";
}

//--------------------------------------------

ModelObject::~ModelObject() {}

//--------------------------------------------

void ModelObject::render() {
	if (file) {
		Lib3dsNode * n;
		for (n = file->nodes; n; n = n->next) {
			render(n);
		}
		cout << "rendered. \n";
	}
	else {
		//cout << "file not found..." << "\n"; -_- ;;
	}
}

//--------------------------------------------

} // namespace Loader
