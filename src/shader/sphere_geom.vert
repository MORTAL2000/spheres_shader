#version 330 core
#extension GL_EXT_gpu_shader4 : enable

layout(location = 0) in vec4  SpherePosition;
layout(location = 1) in vec4  SphereColor;
layout(location = 2) in float SphereRadius;

out vec3 sphere_color_in;
out float sphere_radius_in;


void main()
{  
  sphere_color_in = SphereColor.xyz;
  sphere_radius_in = SphereRadius;
  gl_Position = SpherePosition;
}