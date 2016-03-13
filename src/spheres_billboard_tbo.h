/*****************************************************************************/
/**
 * @file spheres_billboard_tbo.h
 * @brief Implementation of sphere rendering by billboards stored as TBOs.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2016/03/12: Initial commit.
 *****************************************************************************/

#ifndef SPHERES_BILLBOARD_TBO_H_
#define SPHERES_BILLBOARD_TBO_H_

#include "tools.h"
#include "shader.h"
#include "gl_globals.h"
#include "spheres.h"

#include <stdlib.h>
#include <string>
#include <vector>

/**
 * Sphere rendering by raycasting on billboards whose data is stored in texture buffer objects.
 * Implements sphere rendering interface.
 */
template<unsigned TNumSpheres>
class SpheresBillboardTBO : Spheres<SpheresBillboardTBO<TNumSpheres>, TNumSpheres>
{
  public:
  SpheresBillboardTBO()
      :_vertexBuffer(0),_vertexArray(0),_indexBuffer(0),_tbo(0),_tboData(0)
      {}
    const std::string getDescription() const {
      return "Spheres Rendering: Billboard and Texture Buffer Object.";
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
    GLuint _vertexBuffer, _vertexArray, _indexBuffer;
    GLuint _tbo, _tboData;
};

template<unsigned TNumSpheres>
int SpheresBillboardTBO<TNumSpheres>::loadShader()
{
  if (_shader.isLoaded())
      _shader.unload();

  _shader.load("sphere.vert", "sphere.frag");

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
int SpheresBillboardTBO<TNumSpheres>::create(float radius_mean, float radius_var)
{
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  int err = createBuffers(radius_mean, radius_var);
  err |= loadShader();
  return err;
}


template<unsigned TNumSpheres>
int SpheresBillboardTBO<TNumSpheres>::recompile()
{
  return loadShader();
}

template<unsigned TNumSpheres>
void SpheresBillboardTBO<TNumSpheres>::bind(const float* lightPos, const Camera& camera)
{
  glBindTexture(GL_TEXTURE_BUFFER, _tbo);
  _shader.bind();
  _shader.setUniformVar("lightPos", lightPos);
  _shader.setUniformMat4("MVMatrix", (float*)camera.modelview());
  _shader.setUniformMat4("PMatrix", (float*)camera.projection());
  _shader.setUniformVar("SphereParams", 0); // texture slot
}
template<unsigned TNumSpheres>
void SpheresBillboardTBO<TNumSpheres>::operator()()
{
  glBindVertexArray(_vertexArray);
  glDrawElements(GL_TRIANGLES, 6*TNumSpheres, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

template<unsigned TNumSpheres>
void SpheresBillboardTBO<TNumSpheres>::unbind()
{
  _shader.unbind();
  glBindTexture(GL_TEXTURE_BUFFER, 0);
}


template<unsigned TNumSpheres>
int SpheresBillboardTBO<TNumSpheres>::createBuffers(float radius_mean, float radius_var)
{
  srand(2013);
    GLfloat *h_data = new GLfloat[12 * TNumSpheres];
    const float a = -1.f, b = 1.f;
    ///// VERTEX
    for (unsigned int i = 0; i < (TNumSpheres) * 12; i = i + 12)
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
    glGenBuffers(1, &_tboData);
    upload_buffer(_tboData, h_data, 12*TNumSpheres, GL_TEXTURE_BUFFER, GL_STATIC_DRAW);
    glGenTextures(1, &_tbo);
    glBindTexture(GL_TEXTURE_BUFFER, _tbo);
    glTexBufferEXT(GL_TEXTURE_BUFFER, GL_RGBA32F, _tboData);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    delete[] h_data;
    h_data = 0;
    if (CHECK_GLERROR() != GL_NO_ERROR)
      return 1;
    // ----------
    /// per vertex data  (impostor space)
    h_data = new float[TNumSpheres*8];

    /// Impostor Space
    for (unsigned int i = 0; i < (TNumSpheres) * 8;
          i = i + 8)
    {
      h_data[i+0] = -1.0f;
      h_data[i+1] = -1.0f;

      h_data[i+2] =  1.0f;
      h_data[i+3] = -1.0f;

      h_data[i+4] = -1.0f;
      h_data[i+5] =  1.0f;

      h_data[i+6] =  1.0f;
      h_data[i+7] =  1.0f;
    }
    glGenBuffers(1, &_vertexBuffer);
    upload_buffer(_vertexBuffer, h_data, 8 * TNumSpheres, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    delete[] h_data;

    //
    if(!_indexBuffer)
      glGenBuffers(1,&_indexBuffer);

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
    upload_buffer(_indexBuffer, indices, 6*TNumSpheres, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

    delete[] indices;

    if (CHECK_GLERROR() != GL_NO_ERROR)
      return 1;
    // ------------
    // create vertex array buffer
    if(!_vertexArray)
      glGenVertexArrays(1, &_vertexArray);


    glBindVertexArray(_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glEnableVertexAttribArray(0); // space
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (CHECK_GLERROR() != GL_NO_ERROR)
      return 1;
    return 0;
}

template<unsigned TNumSpheres>
void SpheresBillboardTBO<TNumSpheres>::cleanup()
{
  glDeleteVertexArrays(1, &_vertexArray);
  _vertexArray = 0;

  glDeleteBuffers(1, &_vertexBuffer);
  _vertexBuffer = 0;

  glDeleteBuffers(1, &_tboData);
  _tboData = 0;
  glDeleteTextures(1, &_tbo);
  _tbo = 0;

  glDeleteBuffers(1, &_indexBuffer);
  _indexBuffer = 0;
}




#endif /* SPHERES_BILLBOARD_TBO_H_ */
