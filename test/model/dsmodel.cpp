//// Object.h /////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include "lib3ds/file.h"
#include "lib3ds/mesh.h"

class Object
{
        public:
                Object(std:: string filename);
				virtual ~Object();
                virtual void Draw() const;
                virtual void CreateVBO();
               
        protected:
                void GetFaces();
                unsigned int m_TotalFaces;
                Lib3dsFile * m_model;
                GLuint m_VertexVBO, m_NormalVBO, m_TexCoordVBO;
};

#endif //OBJECT_H

/// Object.cpp////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////////

Object::Object(std:: string filename)
{
        m_TotalFaces = 0;
        
        m_model = lib3ds_file_load(filename.c_str());
        // If loading the model failed, we throw an exception
        if(!m_model)
        {
                throw strcat("Unable to load ", filename.c_str());
        }
}

Object::~Object()
{

}

void Object::GetFaces()
{
      m_TotalFaces = 0;
     Lib3dsMesh * mesh;
     
     // Loop through every mesh.
     
     for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
     {
          // Add the number of faces this mesh has to the total number of faces.
          
          m_TotalFaces += mesh->faces;
     }     

}

void Object::CreateVBO()
{
        // Calculate the number of faces we have in total.
        
     GetFaces();
     
     // Allocate memory for our vertices, normals and texture-coordinates.
     
     Lib3dsVector * vertices = new Lib3dsVector[m_TotalFaces * 3];
     Lib3dsVector * normals = new Lib3dsVector[m_TotalFaces * 3];
     Lib3dsVector * texCoords = new Lib3dsVector[m_TotalFaces * 2];
     
     Lib3dsMesh * mesh;
     unsigned int FinishedFaces = 0;  
       
     // Loop through all the meshes.
     
     for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
     {
          lib3ds_mesh_calculate_normals(mesh, &normals[FinishedFaces*3]);
          
          // Loop through every face.
          
		  for(unsigned int cur_face = 0; cur_face < mesh->faces;cur_face++)
          {             
			  Lib3dsFace * face = &mesh->faceL[cur_face];
               for(unsigned int i = 0;i < 3;i++)

               {                        
                    // If there are texture-coordinates.
                    
                    if(mesh->texels)
			        {
                         memcpy(&texCoords[FinishedFaces*2 + i], mesh->texelL[face->points[ i ]], sizeof(Lib3dsTexel)); 
 
                    }     
                    memcpy(&vertices[FinishedFaces*3 + i], mesh->pointL[face->points[ i ]].pos, sizeof(Lib3dsVector));
               }
               FinishedFaces++;
          }
     }
     
     // Generate a VBO and store it with our vertices.
     
     glGenBuffers(1, &m_VertexVBO);
     glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsVector) * 3 * m_TotalFaces, vertices, GL_STATIC_DRAW);
     
     // Generate another VBO and store the normals in it.
     
     glGenBuffers(1, &m_NormalVBO);
     glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsVector) * 3 * m_TotalFaces, normals, GL_STATIC_DRAW);
     
     // Generate a third VBO and store the texture coordinates in it. 
     
     glGenBuffers(1, &m_TexCoordVBO);
     glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsTexel) * 3 * m_TotalFaces, texCoords, GL_STATIC_DRAW); 
     
     // Clean up our allocated memory because the data is now stored in the GPU.
     
     delete vertices;
     delete normals;
     delete texCoords;
     
     // We no longer need lib3ds.
     
     lib3ds_file_free(m_model);
     m_model = NULL;
}

void Object:: Draw() const
{
      // Enable vertex, normal and texture-coordinate arrays.
     
     glEnableClientState(GL_VERTEX_ARRAY);
     glEnableClientState(GL_NORMAL_ARRAY);
     glEnableClientState(GL_TEXTURE_COORD_ARRAY);  
     
     // Bind the VBO with the normals.
   /*  
     glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO);
     
     // The pointer for the normals is NULL which means that OpenGL will use the currently bound VBO.
     
     glNormalPointer(GL_FLOAT, 0, NULL);
     
     glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordVBO);     
     glTexCoordPointer(2, GL_FLOAT, 0, NULL);     
    */ 
     glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
     glVertexPointer(3, GL_FLOAT, 0, NULL);
     
     // Render the triangles.
     glDrawArrays(GL_TRIANGLES, 0, m_TotalFaces * 3);
   
     glDisableClientState(GL_VERTEX_ARRAY);
     glDisableClientState(GL_NORMAL_ARRAY);    
     glDisableClientState(GL_TEXTURE_COORD_ARRAY);  
    
}
