#version 330 core

layout(location = 0) in vec4  SpherePosition;
layout(location = 1) in vec4  SphereColor;
layout(location = 2) in float SphereRadius;
layout(location = 3) in vec2  SphereTexCoord;

uniform mat4 MVMatrix;
uniform mat4 PMatrix;
uniform vec4 lightPos;

smooth out vec2 texcoord;
flat out vec4 eye_position;
flat out vec3 sphere_color;
flat out float sphere_radius;
flat out vec3 lightDir;

void main()
{
  texcoord = SphereTexCoord;
  // Output vertex position
  eye_position = MVMatrix * SpherePosition;
  sphere_color = SphereColor.xyz;
  sphere_radius = SphereRadius;

  lightDir = normalize(lightPos.xyz);
  
  gl_Position = eye_position;
  gl_Position.xy += sphere_radius * texcoord;
  gl_Position = PMatrix * gl_Position;
}