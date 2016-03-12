/*
 * spheres_billboard_vbo.h
 *
 *  Created on: 12.03.2016
 *      Author: mwerner
 */

#ifndef SPHERES_BILLBOARD_VBO_H_
#define SPHERES_BILLBOARD_VBO_H_

#include "tools.h"
#include "shader.h"
#include "gl_globals.h"
#include "spheres.h"

#include <stdlib.h>
#include <string>
#include <vector>

template<unsigned TNumSpheres>
class SpheresBillboardVBO : Spheres<SpheresBillboardVBO<TNumSpheres>, TNumSpheres>
{
  public:
  SpheresBillboardVBO()
      :vertexBuffer(0),vertexArray(0),indexBuffer(0)
      {}
    const std::string getDescription() const {
      return "Spheres Rendering: Billboard and Vertex Buffer Object.";
    }
    int create(float radius_mean, float radius_var);
    int recompile();
    void bind(const float* lightPos, const Camera& camera);
    void operator()();
    void unbind();
    void cleanup();
  private:
    int createBuffers(float radius_mean, float radius_var);
    int loadShader();
    void createSphereGeom( int rings=10, int sectors=10 );
  private:
    ShaderManager _shader;
    GLuint vertexBuffer, vertexArray, indexBuffer;
};

template<unsigned TNumSpheres>
int SpheresBillboardVBO<TNumSpheres>::loadShader()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_CLAMP);
  if (_shader.isLoaded())
      _shader.unload();

  _shader.load("sphere_elem.vert", "sphere_elem.frag");

  int s = _shader.link();
  if (s)
  {
    printf("Error occurred.\n");
    return 1;
  }
  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;

  return 0;
}

template<unsigned TNumSpheres>
int SpheresBillboardVBO<TNumSpheres>::create(float radius_mean, float radius_var)
{
  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  int err = createBuffers(radius_mean, radius_var);
  err |= loadShader();
  return err;
}


template<unsigned TNumSpheres>
int SpheresBillboardVBO<TNumSpheres>::recompile()
{
  return loadShader();
}

template<unsigned TNumSpheres>
void SpheresBillboardVBO<TNumSpheres>::bind(const float* lightPos, const Camera& camera)
{
  _shader.bind();
  _shader.setUniformVar("lightPos", lightPos);
  _shader.setUniformMat4("MVMatrix", (float*)camera.modelview());
  _shader.setUniformMat4("PMatrix", (float*)camera.projection());
}
template<unsigned TNumSpheres>
void SpheresBillboardVBO<TNumSpheres>::operator()()
{
  glBindVertexArray(vertexArray);
  glDrawElements(GL_TRIANGLES,6*TNumSpheres,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);
}

template<unsigned TNumSpheres>
void SpheresBillboardVBO<TNumSpheres>::unbind()
{
  _shader.unbind();
}


template<unsigned TNumSpheres>
int SpheresBillboardVBO<TNumSpheres>::createBuffers(float radius_mean, float radius_var)
{
  srand(2013);
  GLfloat *h_data = new GLfloat[44 * TNumSpheres];
  const float a = -1.f, b = 1.f;

  ///// VERTEX
  // data for each vertex (Pos1-4, Color1-4, Radius1-4, TexCoord1-4)
  unsigned int i = 0;
  while(i < 16*(TNumSpheres))
  {
    h_data[i] = mrand(a, b); // vertex.x
    h_data[i + 1] = mrand(a, b); // vertex.y
    h_data[i + 2] = mrand(a, b); // vertex.z
    h_data[i + 3] = 1.0f; // vertex.w

    memcpy(h_data+i+4,h_data+i,4*sizeof(GLfloat));
    memcpy(h_data+i+8,h_data+i,4*sizeof(GLfloat));
    memcpy(h_data+i+12,h_data+i,4*sizeof(GLfloat));

    i+=16;
  }
  while(i < 32*(TNumSpheres))
  {
    h_data[i] = mrand(0.f, 1.0f); // Red
    h_data[i + 1] = mrand(0.f, 1.0f); // Green
    h_data[i + 2] = mrand(0.f, 1.0f); // Blue
    h_data[i + 3] = 1.0f; // Alpha

    memcpy(h_data+i+4,h_data+i,4*sizeof(GLfloat));
    memcpy(h_data+i+8,h_data+i,4*sizeof(GLfloat));
    memcpy(h_data+i+12,h_data+i,4*sizeof(GLfloat));

    i+=16;
  }
  while(i < 36*(TNumSpheres))
  {
    h_data[i] = radius_var * rand() / RAND_MAX + radius_mean;
    h_data[i+1]=h_data[i];
    h_data[i+2]=h_data[i];
    h_data[i+3]=h_data[i];

    i+=4;
  }
  while(i < 44*(TNumSpheres))
  {
    h_data[i+0] = -1.0f;
    h_data[i+1] = -1.0f;

    h_data[i+2] =  1.0f;
    h_data[i+3] = -1.0f;

    h_data[i+4] = -1.0f;
    h_data[i+5] =  1.0f;

    h_data[i+6] =  1.0f;
    h_data[i+7] =  1.0f;

    i+=8;
  }
  ///
  if(!vertexBuffer)
    glGenBuffers(1, &vertexBuffer);

  upload_buffer(vertexBuffer, h_data, 44 * TNumSpheres, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

  delete[] h_data;
  h_data = 0;
  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  // ----------

  if(!indexBuffer)
    glGenBuffers(1,&indexBuffer);

  GLuint *indices = new GLuint[6*TNumSpheres];
  int j=0;
  for(int k=0; k<6*TNumSpheres; k+=6)
  {
    indices[k]   = j;
    indices[k+1] = j+1;
    indices[k+2] = j+2;
    indices[k+3] = j+3;
    indices[k+4] = j+2;
    indices[k+5] = j+1;
    j+=4;
  }
  upload_buffer(indexBuffer, indices, 6*TNumSpheres, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

  delete[] indices;

  // create vertex array buffer
  if(!vertexArray)
    glGenVertexArrays(1, &vertexArray);

  glBindVertexArray(vertexArray);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // color
  glEnableVertexAttribArray(2); // radius
  glEnableVertexAttribArray(3); // texcoord
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(16*sizeof(GLfloat)*TNumSpheres));
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(32*sizeof(GLfloat)*TNumSpheres));
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(36*sizeof(GLfloat)*TNumSpheres));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  return 0;
}

template<unsigned TNumSpheres>
void SpheresBillboardVBO<TNumSpheres>::cleanup()
{
  glDeleteVertexArrays(1, &vertexArray);
  vertexArray = 0;

  glDeleteBuffers(1, &vertexBuffer);
  vertexBuffer = 0;

  glDeleteBuffers(1, &indexBuffer);
  indexBuffer = 0;
}


#endif /* SPHERES_BILLBOARD_VBO_H_ */
