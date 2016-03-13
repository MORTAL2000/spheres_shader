/*****************************************************************************/
/**
 * @file spheres_instancing.h
 * @brief Implementation of sphere rendering by instancing sphere geometry.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2016/03/12: Initial commit.
 *****************************************************************************/


#ifndef SPHERES_INSTANCING_H_
#define SPHERES_INSTANCING_H_

#include "tools.h"
#include "shader.h"
#include "gl_globals.h"
#include "spheres.h"

#include <stdlib.h>
#include <string>
#include <vector>

/**
 * Sphere rendering by instancing a sphere geometry object.
 * Implements sphere rendering interface.
 */
template<unsigned TNumSpheres>
class SpheresInstancing : Spheres<SpheresInstancing<TNumSpheres>, TNumSpheres>
{
  public:
    SpheresInstancing()
      :_vertexBuffer(0),_vertexArray(0),_tboParams(0),_sphere_vertices(0),_sphere_indices(0)
      {}
    const std::string getDescription() const {
      return "Spheres Rendering: Polygon-based Geometry Instancing.";
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
    GLuint _vertexBuffer, _vertexArray, _tboParams;
    std::vector<GLfloat>  _sphere_vertices;
    std::vector<GLushort> _sphere_indices;
};

template<unsigned TNumSpheres>
int SpheresInstancing<TNumSpheres>::loadShader()
{
  if (_shader.isLoaded())
      _shader.unload();

  _shader.load("sphere_instanced.vert", "sphere_instanced.frag");

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
int SpheresInstancing<TNumSpheres>::create(float radius_mean, float radius_var)
{
  glEnable(GL_DEPTH_CLAMP);
  int err = createBuffers(radius_mean, radius_var);
  err |= loadShader();
  return err;
}


template<unsigned TNumSpheres>
int SpheresInstancing<TNumSpheres>::recompile()
{
  return loadShader();
}

template<unsigned TNumSpheres>
void SpheresInstancing<TNumSpheres>::bind(const float* lightPos, const Camera& camera)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_BUFFER_EXT, _tboParams);
  _shader.bind();
  _shader.setUniformVar("tboParams", 0);
  _shader.setUniformVar("lightPos", lightPos);
  _shader.setUniformMat4("MVPMatrix", (float*)camera.mvpmatrix());
}
template<unsigned TNumSpheres>
void SpheresInstancing<TNumSpheres>::operator()()
{
  glBindVertexArray(_vertexArray);
  glDrawElementsInstancedEXT(GL_QUADS,
                                 _sphere_indices.size(),
                                 GL_UNSIGNED_SHORT,
                                 0,
                                 TNumSpheres
                                );
  glBindVertexArray(0);
}

template<unsigned TNumSpheres>
void SpheresInstancing<TNumSpheres>::unbind()
{
  _shader.unbind();
  glBindTexture(GL_TEXTURE_BUFFER_EXT, 0);
}


template<unsigned TNumSpheres>
int SpheresInstancing<TNumSpheres>::createBuffers(float radius_mean, float radius_var)
{
  srand(2013);
  GLfloat *h_data = new GLfloat[9 * TNumSpheres];
  const float a = -1.f, b = 1.f;
  ///// VERTEX
  for (unsigned int i = 0; i < (9*TNumSpheres); i = i + 9)
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
  glGenBuffers(1, &_vertexBuffer);
  upload_buffer(_vertexBuffer, h_data, 9 * TNumSpheres, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
  createTBO(&_tboParams, _vertexBuffer, GL_R32F, GL_TEXTURE0);

  delete[] h_data;

  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  // ------------
  // create vertex array buffer
  if(!_vertexArray)
    glGenVertexArrays(1, &_vertexArray);

  createSphereGeom();

  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  return 0;
}

template<unsigned TNumSpheres>
void SpheresInstancing<TNumSpheres>::cleanup()
{
  glDeleteVertexArrays(1, &_vertexArray);
  _vertexArray = 0;

  glDeleteBuffers(1, &_vertexBuffer);
  _vertexBuffer = 0;

  glDeleteTextures(1, &_tboParams);
  _tboParams = 0;
}


//-----------------------------------------------------------------------------
// edited from http://stackoverflow.com/questions/5988686/how-do-i-create-a-3d-sphere-in-opengl-using-visual-c/5989676#5989676
//-----------------------------------------------------------------------------
template<unsigned TNumSpheres>
void SpheresInstancing<TNumSpheres>::createSphereGeom( int rings, int sectors )
{
  float const R = 1.f/(float)(rings-1);
  float const S = 1.f/(float)(sectors-1);
  int r, s;
  float radius = 1.0f;
  GLuint sphere_IBO=0, sphere_VBO=0;

  _sphere_vertices.resize(rings * sectors * 6);
  std::vector<GLfloat>::iterator v = _sphere_vertices.begin();
  for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
    float const y = sin( -0.5*M_PI + M_PI * r * R );
    float const x = cos(2.0*M_PI * s * S) * sin( M_PI * r * R );
    float const z = sin(2.0*M_PI * s * S) * sin( M_PI * r * R );

    // vertices
    *v++ = x * radius;
    *v++ = y * radius;
    *v++ = z * radius;
    // normals
    *v++ = x;
    *v++ = y;
    *v++ = z;
  }

  _sphere_indices.resize((rings-1) * (sectors-1) * 4);
  std::vector<GLushort>::iterator i = _sphere_indices.begin();
  for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
    *i++ = r * sectors + s;
    *i++ = r * sectors + (s+1);
    *i++ = (r+1) * sectors + (s+1);
    *i++ = (r+1) * sectors + s;
  }

  glGenBuffers(1, &sphere_VBO);
  glGenBuffers(1,&sphere_IBO);

  glBindVertexArray(_vertexArray);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);

  glBufferData(
      GL_ARRAY_BUFFER,
      _sphere_vertices.size()*sizeof(float),
      _sphere_vertices.data(),
      GL_STATIC_DRAW
    );

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                  _sphere_indices.size()*sizeof(GLushort),
                  _sphere_indices.data(),
                  GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


#endif /* SPHERES_INSTANCING_H_ */
