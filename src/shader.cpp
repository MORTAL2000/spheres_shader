/*****************************************************************************/
/**
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 *****************************************************************************/
#include "shader.h"
#include <fstream>
#include <string>
using std::ifstream;

static char buffer[1024];
static int len = 0;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ShaderManager::ShaderManager()
{
  init();
  setShaderLocation("./shader/");
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ShaderManager::ShaderManager(const char * shader_location)
{
  init();
  setShaderLocation(shader_location);
}
//-----------------------------------------------------------------------------
//bundles a VS and a PS into a program
//-----------------------------------------------------------------------------
void ShaderManager::load(const char * vertexshader,
                         const char * pixelshader,
                         const char * geoshader)
{
  if (_isLoaded)
  {
    printf("Shader already loaded to application.\n");
    exit(1);
  }
  if (_programID == 0)
    _programID = glCreateProgram();
  if (vertexshader != NULL)
  {
    _vertexShaderID = loadShader(vertexshader, GL_VERTEX_SHADER);
    glAttachShader(_programID, _vertexShaderID);
  }
  if (pixelshader != NULL)
  {
    _pixelShaderID = loadShader(pixelshader, GL_FRAGMENT_SHADER);
    glAttachShader(_programID, _pixelShaderID);
  }
  if (geoshader != NULL)
  {
    _geoShaderID = loadShader(geoshader, GL_GEOMETRY_SHADER);
    glAttachShader(_programID, _geoShaderID);

    glProgramParameteriEXT(_programID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
    glProgramParameteriEXT(_programID, GL_GEOMETRY_OUTPUT_TYPE_EXT,
                           GL_TRIANGLE_STRIP);
    glProgramParameteriEXT(_programID, GL_GEOMETRY_VERTICES_OUT_EXT, 4);
  }
  _isLoaded = true;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int ShaderManager::link()
{
  glLinkProgram(_programID);
  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;

  glGetProgramInfoLog(_programID, 1000, &len, buffer);
  if (len)
  {
    printf("Could not link %u: '%s'\n", _programID, buffer);
    return 1;
  }
  return 0;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
GLuint ShaderManager::loadShader(const char *filename, int type)
{
  if (filename == NULL)
  {
    printf("ERROR. No Filename given.\n");
    exit(1);
  }
  std::string filename_full = _shader_location;
  GLhandleARB handle;

  // shader Compilation variable
  GLint result; // Compilation code result
  GLint errorLoglength;
  char* errorLogText;
  GLsizei actualErrorLogLength;

  filename_full += filename;

  handle = glCreateShaderObjectARB(type);
  if (!handle)
  {
    //We have failed creating the vertex shader object.
    printf("Failed creating vertex shader object from file: %s.", filename_full);
    exit(1);
  }

  std::string content;
  readEntireFile(&content, filename_full.c_str());
  GLchar const *shader_source = content.c_str();
  GLint const shader_length = content.size();
  glShaderSource(handle, //The handle to our shader
      1, //The number of files.
      &shader_source, //An array of const char * data, which represents the source code of theshaders
      &shader_length);

  glCompileShaderARB(handle);

  //Compilation checking.
  glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

  // If an error was detected.
  if (!result)
  {
    //We failed to compile.
    printf("Shader '%s' failed compilation.\n", filename_full);

    //Attempt to get the length of our error log.
    glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                              &errorLoglength);

    //Create a buffer to read compilation error message
    errorLogText = (char*) malloc(sizeof(char) * errorLoglength);

    //Used to get the final length of the log.
    glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength,
                    errorLogText);

    // Display errors.
    printf("%s\n", errorLogText);

    // Free the buffer malloced earlier
    free(errorLogText);
    exit(1);
  }

  return handle;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int ShaderManager::getUniformVarID(const char *name)
{
  glGetUniformLocation(_programID, name);
  if(CHECK_GLERROR()!=GL_NO_ERROR){
    fprintf(stderr, "Could not get uniform location '%s'.\n", name);
    return -1;
  }
  return glGetUniformLocation(_programID, name);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformVar(const char* name, int var[4])
{
  glUniform4i(getUniformVarID(name), var[0], var[1], var[2], var[3]);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformVar(const char* name, int var)
{
  glUniform1i(getUniformVarID(name), var);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformVar(const char* name, float var)
{
  glUniform1f(getUniformVarID(name), var);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformVar(const char* name, float var[4])
{
  glUniform4f(getUniformVarID(name), var[0], var[1], var[2], var[3]);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformVar(const char* name, const float* var)
{
  glUniform4f(getUniformVarID(name), var[0], var[1], var[2], var[3]);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformMat3(const char* name, float* array)
{
  glUniformMatrix3fv(getUniformVarID(name), 1, GL_FALSE, array);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformMat4(const char* name, float* array)
{
  glUniformMatrix4fv(getUniformVarID(name), 1, GL_FALSE, array);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformMat3(const char* name, const float* array)
{
  glUniformMatrix3fv(getUniformVarID(name), 1, GL_FALSE, array);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::setUniformMat4(const char* name, const float* array)
{
  glUniformMatrix4fv(getUniformVarID(name), 1, GL_FALSE, array);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::bind()
{
  glUseProgram(_programID);
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::unbind()
{
  glUseProgram(0);
  glDisable(GL_VERTEX_PROGRAM_ARB);
  glDisable(GL_FRAGMENT_PROGRAM_ARB);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool ShaderManager::isLoaded()
{
  return _isLoaded;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::readEntireFile(std::string* content, const char * filename)
{
  // --- Read file
  std::string line;
  ifstream myfile(filename);
  if (myfile.is_open())
  {
    while (myfile.good())
    {
      getline(myfile, line);
      *content += line + "\n";
    }
    myfile.close();
  }
  else
  {
    printf("Could not open file '%s'.\n", filename);
    exit(1);
  }
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ShaderManager::unload()
{
  if (_programID == 0)
  {
    printf("Error occurred on unloading Shader.\n");
    exit(1);
  }
  if (_vertexShaderID)
    glDetachShader(_programID, _vertexShaderID);
  if (_pixelShaderID)
    glDetachShader(_programID, _pixelShaderID);
  if (_geoShaderID)
    glDetachShader(_programID, _geoShaderID);
  _isLoaded = false;

}

void
ShaderManager::init()
{
  _programID=0;
  _vertexShaderID=0;
  _pixelShaderID=0;
  _geoShaderID=0;
  _isLoaded=false;
  _shader_location="./";
}

void
ShaderManager::setShaderLocation(const char* shader_location)
{
  _shader_location = shader_location;
}
