#version 330 core

layout(location = 0) in vec4  SpherePosition;
layout(location = 1) in vec4  SphereColor;
layout(location = 2) in float SphereRadius;

uniform mat4 MVMatrix;
uniform mat4 PMatrix;
uniform vec4 lightPos;
uniform int screenWidth;   // scale to calculate size in pixels

flat out vec4 eye_position;
flat out vec3 sphere_color;
flat out float sphere_radius;
flat out vec3 lightDir;

void main()
{
  // Output vertex position
  eye_position = MVMatrix * SpherePosition;
  sphere_color = SphereColor.xyz;
  sphere_radius = SphereRadius;

  lightDir = normalize(lightPos.xyz);
  float dist = length(eye_position.xyz);
    
  gl_Position = eye_position;
  gl_Position = PMatrix * gl_Position;
// http://stackoverflow.com/questions/8608844/resizing-point-sprites-based-on-distance-from-the-camera
  vec4 projCorner = PMatrix * vec4(sphere_radius, sphere_radius, eye_position.z, eye_position.w);
  gl_PointSize = screenWidth * projCorner.x / projCorner.w;
}