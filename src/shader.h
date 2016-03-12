/*****************************************************************************/
/**
 * @brief OpenGL shader helper class.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 * @sa http://pages.cs.wisc.edu/~shenoy/
 *****************************************************************************/
#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#include "gl_globals.h"
#include <string>

//Manages vertex and pixel shaders
class ShaderManager
{
public:
  ShaderManager();
  ShaderManager(const char* shader_location);
  void load(const char * vertexshader,
            const char * pixelshader,
            const char * geoshader = NULL);
  int link();
  void bind();
  void setShaderLocation(const char* );
  int  getUniformVarID(const char * name);

  void setUniformVar(const char* name, int varValue);
  void setUniformVar(const char* name, int var[4]);
  void setUniformVar(const char* name, float varValue);
  void setUniformVar(const char* name, float var[4]);
  void setUniformVar(const char* name, const float* var);

  void setUniformMat3(const char* name, float* array);
  void setUniformMat3(const char* name, const float* array);
  void setUniformMat4(const char* name, float* array);
  void setUniformMat4(const char* name, const float* array);

  void unbind();
  void unload();
  bool isLoaded();

  GLuint programID() const{ return _programID; }

private:
  GLuint loadShader(const char * filename, int type);
  void readEntireFile(std::string* content, const char * filename);
  void init();
private:
  GLuint _programID;
  GLuint _vertexShaderID;
  GLuint _pixelShaderID;
  GLuint _geoShaderID;
  bool  _isLoaded;
  std::string _shader_location;
};

#endif
