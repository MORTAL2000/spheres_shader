#version 330 core
#extension GL_EXT_gpu_shader4 : enable

layout(location = 0) in vec2  SphereImpostorSpace;

uniform samplerBuffer SphereParams;
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
  int id = int(gl_VertexID/4) * 3;
  texcoord = SphereImpostorSpace;
  // Output vertex position
  eye_position = MVMatrix * texelFetchBuffer(SphereParams, id);
  sphere_color = texelFetchBuffer(SphereParams, id+1).xyz;
  sphere_radius = texelFetchBuffer(SphereParams, id+2).x;

  lightDir = normalize(lightPos.xyz);
  
  /*if( (gl_VertexID%4)==0 )
    sphere_color = vec3(1.0,0.0,0.0);
  else if( (gl_VertexID%4)==1 )
    sphere_color = vec3(0.0,1.0,0.0);
  else if( (gl_VertexID%4)==2 )
    sphere_color = vec3(0.0,0.0,1.0);
  else
    sphere_color = vec3(1.0,1.0,1.0);*/
    
  gl_Position = eye_position;
  gl_Position.xy += sphere_radius * SphereImpostorSpace;
  gl_Position = PMatrix * gl_Position;
}