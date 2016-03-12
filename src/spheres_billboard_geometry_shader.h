/*
 * sphere_billboard_geometry_shader.h
 *
 *  Created on: 12.03.2016
 *      Author: mwerner
 */

#ifndef SPHERE_BILLBOARD_GEOMETRY_SHADER_H_
#define SPHERE_BILLBOARD_GEOMETRY_SHADER_H_

#include "tools.h"
#include "shader.h"
#include "gl_globals.h"
#include "spheres.h"

#include <stdlib.h>
#include <string>
#include <vector>

template<unsigned TNumSpheres>
class SpheresBillboardGeometryShader : Spheres<SpheresBillboardGeometryShader<TNumSpheres>, TNumSpheres>
{
  public:
  SpheresBillboardGeometryShader()
      :vertexBuffer(0),vertexArray(0),indexBuffer(0)
      {}
    const std::string getDescription() const {
      return "Spheres Rendering: Billboard using Geometry Shader only.";
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
int SpheresBillboardGeometryShader<TNumSpheres>::loadShader()
{
  if (_shader.isLoaded())
      _shader.unload();

  _shader.load("sphere_geom.vert", "sphere.frag", "sphere_geom.geom");

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
int SpheresBillboardGeometryShader<TNumSpheres>::create(float radius_mean, float radius_var)
{
  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  int err = createBuffers(radius_mean, radius_var);
  err |= loadShader();
  return err;
}


template<unsigned TNumSpheres>
int SpheresBillboardGeometryShader<TNumSpheres>::recompile()
{
  return loadShader();
}

template<unsigned TNumSpheres>
void SpheresBillboardGeometryShader<TNumSpheres>::bind(const float* lightPos, const Camera& camera)
{
  _shader.bind();
  _shader.setUniformVar("lightPos", lightPos);
  _shader.setUniformMat4("MVMatrix", (float*)camera.modelview());
  _shader.setUniformMat4("PMatrix", (float*)camera.projection());
}
template<unsigned TNumSpheres>
void SpheresBillboardGeometryShader<TNumSpheres>::operator()()
{
  glBindVertexArray(vertexArray);
  glDrawArrays(GL_POINTS, 0, TNumSpheres);
  glBindVertexArray(0);
}

template<unsigned TNumSpheres>
void SpheresBillboardGeometryShader<TNumSpheres>::unbind()
{
  _shader.unbind();
}


template<unsigned TNumSpheres>
int SpheresBillboardGeometryShader<TNumSpheres>::createBuffers(float radius_mean, float radius_var)
{
  srand(2013);
  GLfloat *h_data = new GLfloat[12 * TNumSpheres];
  const float a = -1.f, b = 1.f;
  ///// VERTEX
  for (unsigned int i = 0; i < (TNumSpheres * 12); i = i + 12)
  {
    h_data[i] = mrand(a, b); // vertex.x
    h_data[i + 1] = mrand(a, b); // vertex.y
    h_data[i + 2] = mrand(a, b); // vertex.z
    h_data[i + 3] = 1.0f; // vertex.w

    h_data[i + 4] = mrand(0.f, 1.0f); // Red
    h_data[i + 5] = mrand(0.f, 1.0f); // Green
    h_data[i + 6] = mrand(0.f, 1.0f); // Blue
    h_data[i + 7] = 1.0f; // Alpha

    h_data[i + 8] = radius_var * rand() / RAND_MAX + radius_mean;
  }
  ///
  if(!vertexBuffer)
    glGenBuffers(1, &vertexBuffer);

  upload_buffer(vertexBuffer, h_data, 12 * TNumSpheres, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

  delete[] h_data;

  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  // ------------
  // create vertex array buffer
  if(!vertexArray)
    glGenVertexArrays(1, &vertexArray);


  glBindVertexArray(vertexArray);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // color
  glEnableVertexAttribArray(2); // radius
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12*4, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12*4, (GLvoid*)16);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 12*4, (GLvoid*)32);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  return 0;
}

template<unsigned TNumSpheres>
void SpheresBillboardGeometryShader<TNumSpheres>::cleanup()
{
  glDeleteVertexArrays(1, &vertexArray);
  vertexArray = 0;

  glDeleteBuffers(1, &vertexBuffer);
  vertexBuffer = 0;
}


#endif /* SPHERE_BILLBOARD_GEOMETRY_SHADER_H_ */
