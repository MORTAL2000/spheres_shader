#version 330 core
#extension GL_EXT_geometry_shader4 : enable

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 MVMatrix;
uniform mat4 PMatrix;
uniform vec4 lightPos;

in vec3 sphere_color_in[];
in float sphere_radius_in[];

flat out vec3 sphere_color;
flat out float sphere_radius;
smooth out vec2 texcoord;
flat out vec4 eye_position;
flat out vec3 lightDir;

void main()
{
  // Output vertex position
  eye_position = MVMatrix * gl_in[0].gl_Position; 
  sphere_color = sphere_color_in[0];
  sphere_radius = sphere_radius_in[0];
  
  lightDir = normalize(lightPos.xyz);
  
  // Vertex 1
  texcoord = vec2(-1.0,-1.0);
  gl_Position = eye_position;
  gl_Position.xy += vec2(-sphere_radius, -sphere_radius);
  gl_Position = PMatrix  * gl_Position;
  EmitVertex();

  // Vertex 2
  texcoord = vec2(-1.0,1.0);
  gl_Position = eye_position;
  gl_Position.xy += vec2(-sphere_radius, sphere_radius);
  gl_Position = PMatrix  * gl_Position;
  EmitVertex();

  // Vertex 3
  texcoord = vec2(1.0,-1.0);
  gl_Position = eye_position;
  gl_Position.xy += vec2(sphere_radius, -sphere_radius);
  gl_Position = PMatrix  * gl_Position;
  EmitVertex();

  // Vertex 4
  texcoord = vec2(1.0,1.0);
  gl_Position = eye_position;
  gl_Position.xy += vec2(sphere_radius, sphere_radius);
  gl_Position = PMatrix  * gl_Position;
  EmitVertex();

  EndPrimitive();
}