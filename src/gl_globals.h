/*****************************************************************************/
/**
 * @file
 * @brief Some globals.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 *****************************************************************************/

#ifndef GL_GLOBALS_H_
#define GL_GLOBALS_H_

//-----------------------------------------------------------------------------

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/wglext.h>
typedef unsigned int uint;
#define M_PI       3.14159265358979323846
#define M_WINDOWS 1
#endif

#if defined(LINUX) || defined(__linux)
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glx.h>
#include <string.h>
#endif

#include <stdio.h>


#define SHADER_LOCATION "shader/"

// define this to disable OpenGL error checking.
#ifndef NO_CHECK_GLERROR
#define CHECK_GLERROR() checkGLError(__FILE__, __LINE__)
#else
#define CHECK_GLERROR() GL_NO_ERROR
#endif

/**
 *
 * @param f
 * @param line
 * @return
 */
inline int checkGLError(const char* f, const int line)
{
  GLuint err = glGetError();
  if (err != GL_NO_ERROR)
    fprintf(stderr, "%s:%d: OpenGL Error: %s\n", f, line, gluErrorString(err));
  return err;
}

/**
 *
 * @param buffer
 * @param h_data
 * @param count
 * @param target
 * @param access
 */
template<typename T>
inline void upload_buffer(GLuint buffer,
            T* h_data,
            GLuint count,
            GLenum target,
            GLenum access)
{
  glBindBuffer(target, buffer);
  /*
   * We do not use glBufferData for data upload, since it would load into
   * host ram as pinned memory instead of loading into GPU RAM.
   * With glMapBuffer+memcpy we ensure to have our data located on GPU.
   */
  glBufferData(target, count * sizeof(T), NULL, access);
  void* d_data = (void*) glMapBuffer(target, GL_READ_WRITE);
  if (d_data == NULL)
  {
    fprintf(stderr, "Could not map gpu buffer.\n");
    exit(1);
  }
  memcpy(d_data, (const void*) h_data, count * sizeof(T));
  if (!glUnmapBuffer(target))
  {
    fprintf(stderr, "Unmap buffer failed.\n");
    exit(1);
  }
  d_data = NULL;
  // release buffer
  glBindBuffer(target, 0);
}

/**
 *
 * @param tbo_id
 * @param tbo_data_id
 * @param type
 * @param unit
 */
inline void createTBO( GLuint* tbo_id, GLuint tbo_data_id, GLint type, GLenum unit )
{
  glActiveTexture(unit);
  if(*tbo_id!=0){
    glDeleteTextures(1, tbo_id);
  }
  glGenTextures(1, tbo_id);
  glBindTexture(GL_TEXTURE_BUFFER, *tbo_id);
  glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, type, tbo_data_id);
  glBindBuffer(GL_TEXTURE_BUFFER_EXT, 0);
}

#endif /* GL_GLOBALS_H_ */
