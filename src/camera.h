/*****************************************************************************/
/**
 * @brief Camera class with OpenGL representation as transformation matrices.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 * @sa http://www.morrowland.com/apron/tut_gl.php
 *****************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS

#include "gl_globals.h"
#include <glm/glm.hpp>


/**
 * Camera Control Class
 * Reference: http://www.morrowland.com/apron/tut_gl.php
 */
class Camera
{
  public:
    Camera();
    ~Camera();

    void strafeCamera(float speed);
    void rotatePosition(float speed);
    void moveCamera(float speed);
    void distCamera(float speed);
    void rotate(float speed);
    void setupCamera( float pos_x,  float pos_y,  float pos_z, float pos_w,
                      float target_x, float target_y, float target_z,
                      float up_x,   float up_y,   float up_z);

    void advanceTargetZ(float z);
    void advancePositionZ(float z);
    void computeNormalMatrix(glm::mat3* mat);

    const glm::vec4& position_glm() const;
    const GLfloat* position() const;
    const glm::vec3& target() const;
    const glm::vec3& up() const;
    const glm::ivec2& screen() const;

    const GLfloat* modelview() const;
    const GLfloat* projection() const;
    const GLfloat* mvpmatrix() const;
    const GLfloat* modelview_inverse();

    const glm::mat4 modelview_glm() const;
    const glm::mat4 projection_glm() const;
    const glm::mat4 mvpmatrix_glm() const;
    const glm::mat4 modelview_inverse_glm();


    void applyProjection(float fov, int w, int h, float zNear=0.01f, float zFar=100.f);
    //void applyProjection(float fov, float aspect, float zNear=0.01f, float zFar=100.f);
    void apply();

  private:
    glm::ivec2 _screen;
    glm::vec4 _pos;
    glm::vec3 _target;
    glm::vec3 _up;
    glm::mat4 _modelview;
    glm::mat4 _projection;
    glm::mat4 _mvpmatrix;
    glm::mat4 _mvinverse;
};

inline const glm::vec4& Camera::position_glm() const{
  return _pos;
}
inline const GLfloat* Camera::position() const{
  return (GLfloat*)&_pos.x;
}
inline const GLfloat* Camera::modelview() const{
  return (GLfloat*)&_modelview[0];
}
inline const GLfloat* Camera::projection() const{
  return (GLfloat*)&_projection[0];
}
inline const GLfloat* Camera::mvpmatrix() const {
  return (GLfloat*)&_mvpmatrix[0];
}
inline const GLfloat* Camera::modelview_inverse() {
  _mvinverse = glm::inverse(_modelview);
  return (GLfloat*)&_mvinverse[0];
}
inline const glm::mat4 Camera::modelview_glm() const{
  return _modelview;
}
inline const glm::mat4 Camera::projection_glm() const{
  return _projection;
}
inline const glm::mat4 Camera::mvpmatrix_glm() const {
  return _mvpmatrix;
}
inline const glm::mat4 Camera::modelview_inverse_glm() {
  _mvinverse = glm::inverse(_modelview);
  return _mvinverse;
}
inline const glm::vec3& Camera::target() const{
  return _target;
}
inline const glm::vec3& Camera::up() const{
  return _up;
}
inline void Camera::advanceTargetZ(float z)
{
  _target.z += z;
  if(_target.z>1.0f)
    _target.z=1.0f;
  if(_target.z<-1.0f)
    _target.z=-1.0f;
}
inline void Camera::advancePositionZ(float z)
{
  _pos.z += z;
}

inline const glm::ivec2& Camera::screen() const{
  return _screen;
}
#endif // CAMERA_H
