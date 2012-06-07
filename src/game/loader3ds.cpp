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
#include <cstdlib>

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
	{
		int p;

		float (*normalL)[3] = (float(*)[3])malloc(3 * 3 * sizeof(float) * mesh->nfaces);
		/* Replacement code "malloc"
		const int size = 3 * mesh->nfaces;
		float * normalL0 = new float[size];
		float * normalL1 = new float[size];
		float * normalL2 = new float[size];	
		float n[3] = {*normalL0, *normalL1, *normalL2};
		float (*normalL)[3] = &n;
		//*/

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
						mesh->texcos[mesh->faces[p].index[i]][1] );
					glVertex3fv(mesh->vertices[mesh->faces[p].index[i]]);
				}
				glEnd();
			}
		}

		free(normalL);
	}
}

//--------------------------------------------

void ModelObject::render(Lib3dsNode * n) {
    for (Lib3dsNode * nc = n->childs; nc; nc = nc->next) {
        render(nc);
    }

	if (n->type == LIB3DS_NODE_MESH_INSTANCE) {
		Lib3dsMeshInstanceNode * node = (Lib3dsMeshInstanceNode*) n;

		Lib3dsMesh * mesh;
		for (int i = 0; i < file->nmeshes; i++) {
			mesh = file->meshes[i];
			
			if (!mesh) continue;

			if (!mesh->user_id) {
				mesh->user_id = glGenLists(1);
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
}

//--------------------------------------------

ModelObject::~ModelObject() {}

//--------------------------------------------

void ModelObject::draw() {
	if (file) {
		Lib3dsNode * n;
		for (n = file->nodes; n; n = n->next) {
			render(n);
		}
	}
}

//--------------------------------------------

} // namespace Loader
