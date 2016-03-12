#version 330 core
#extension GL_EXT_gpu_shader4 : require
#extension GL_ARB_draw_instanced : require

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
 
uniform samplerBuffer tboParams;
uniform mat4 MVPMatrix; // modelviewprojection

out vec3 normal;
out vec3 position;
out vec3 color;

void main()
{
    vec4  transform = vec4(
                        texelFetchBuffer(tboParams, 9*gl_InstanceID).r,
                        texelFetchBuffer(tboParams, 9*gl_InstanceID+1).r,
                        texelFetchBuffer(tboParams, 9*gl_InstanceID+2).r,
                        texelFetchBuffer(tboParams, 9*gl_InstanceID+3).r);
    color           = vec3(
                        texelFetchBuffer(tboParams, 9*gl_InstanceID+4).r,
                        texelFetchBuffer(tboParams, 9*gl_InstanceID+5).r,
                        texelFetchBuffer(tboParams, 9*gl_InstanceID+6).r);
    float radius    = texelFetchBuffer(tboParams, 9*gl_InstanceID+8).r;
    
    vec4  vertex    = vec4((in_Position * transform.w * radius + transform.xyz), 1.0);
    

//color = vec4(cos(9.423*strength),sin(-9.423*strength+3.141),sin(9.423*strength-0.782),1.0);
    normal   = in_Normal;
    gl_Position = MVPMatrix * vertex;
}