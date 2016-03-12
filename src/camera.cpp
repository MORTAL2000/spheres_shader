#include "camera.h"
#include "gl_globals.h"

#include <glm/gtc/matrix_transform.hpp>

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
Camera::Camera()
{
  _pos = glm::vec4(1.f, 1.f, 1.f, 1.f);
  _target = glm::vec3(0.f, 0.f, 0.f);
  _up = glm::vec3(0.f, 0.f, 1.f);
  _modelview = glm::mat4(1.0f);
  _projection = glm::mat4(1.0f);
  _screen = glm::vec2(800,600);
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
Camera::~Camera()
{

}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::setupCamera(float pos_x,
                         float pos_y,
                         float pos_z,
                         float pos_w,
                         float view_x,
                         float view_y,
                         float view_z,
                         float up_x,
                         float up_y,
                         float up_z)
{
  _pos = glm::vec4(pos_x, pos_y, pos_z, pos_w); // set position
  _target = glm::vec3(view_x, view_y, view_z); // set view
  _up = glm::vec3(up_x, up_y, up_z); // set the up vector
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::moveCamera(float speed)
{
  glm::vec3 vVector = _target - glm::vec3(_pos); // Get the view vector

  // forward positive camera speed and backward negative camera speed.
  _pos.x = _pos.x + vVector.x * speed;
  _pos.y = _pos.y + vVector.y * speed;
  _target.x = _target.x + vVector.x * speed;
  _target.y = _target.y + vVector.y * speed;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::distCamera(float speed)
{
  glm::vec3 vVector = _target - glm::vec3(_pos); // Get the view vector
  glm::vec3 pos = glm::vec3(_pos) + speed * vVector;

  if (glm::dot(_target - pos, vVector) > 0.01f)
  {
    // forward positive camera speed and backward negative camera speed.
    _pos = glm::vec4(pos, _pos.w);
  }
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::rotate(float speed)
{
  glm::vec3 vVector = _target - glm::vec3(_pos); // Get the view vector

  _target.y =
      (float) (_pos.y + sin(speed) * vVector.x + cos(speed) * vVector.y);
  _target.x =
      (float) (_pos.x + cos(speed) * vVector.x - sin(speed) * vVector.y);
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::rotatePosition(float speed)
{
  glm::vec3 vVector = glm::vec3(_pos) - _target;

  _pos.y =
      (float) (_target.y + sin(speed) * vVector.x + cos(speed) * vVector.y);
  _pos.x =
      (float) (_target.x + cos(speed) * vVector.x - sin(speed) * vVector.y);
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::strafeCamera(float speed)
{
  glm::vec3 vVector = _target - glm::vec3(_pos); // Get the view vector
  glm::vec3 vOrthoVector; // Orthogonal vector for the view vector

  vOrthoVector.x = -vVector.y;
  vOrthoVector.y = vVector.x;

  // left positive cameraspeed and right negative -cameraspeed.
  _pos.x = _pos.x + vOrthoVector.x * speed;
  _pos.y = _pos.y + vOrthoVector.y * speed;
  _target.x = _target.x + vOrthoVector.x * speed;
  _target.y = _target.y + vOrthoVector.y * speed;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void Camera::apply()
{
  _modelview = glm::mat4(1.0f);
  _modelview *= glm::lookAt(glm::vec3(_pos), _target, _up);
  _mvpmatrix = _projection * _modelview;
}
//-----------------------------------------------------------------------------
// applyProjection
//-----------------------------------------------------------------------------
void Camera::applyProjection(float fov, int w, int h, float zNear, float zFar)
{
  _screen.x = w;
  _screen.y = h;
  fov = fov/180.0f*3.14159265f;
  _projection = glm::mat4(1.0f);
  _projection *= glm::perspective(fov, (GLfloat) w / h, zNear, zFar);
  _mvpmatrix = _projection * _modelview;
}
//-----------------------------------------------------------------------------
// applyProjection
//-----------------------------------------------------------------------------
/*void Camera::applyProjection(float fov, float aspect, float zNear, float zFar)
{
  _projection = glm::mat4(1.0f);
  _projection *= glm::perspective(fov, aspect, zNear, zFar);
  _mvpmatrix = _projection * _modelview;
}*/
//-----------------------------------------------------------------------------
// http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
//-----------------------------------------------------------------------------
void Camera::computeNormalMatrix(glm::mat3* mat)
{
  glm::mat3 submv = glm::mat3(_modelview);
  submv = glm::inverse(submv);
  *mat = glm::transpose(submv);
}
