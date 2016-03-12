#version 330 core

uniform mat4 PMatrix;

flat in vec3 sphere_color;
flat in float sphere_radius;
flat in vec4 eye_position;
flat in vec3 lightDir;

out vec4 out_Color;

void main()
{     
    // r^2 = (x - x0)^2 + (y - y0)^2 + (z - z0)^2
    vec2 texcoord = gl_PointCoord* 2.0 - vec2(1.0);
    float x = texcoord.x;
    float y = texcoord.y;
    float zz = 1.0 - x*x - y*y;

    if (zz <= 0.0)
      discard;

    float z = sqrt(zz);    
    vec4 pos = eye_position;
    pos.z += sphere_radius*z;
    pos = PMatrix * pos;
    gl_FragDepth = 0.5*(pos.z / pos.w)+0.5;
    
    vec3 normal = vec3(x,y,z);
    float diffuseTerm = clamp(dot(normal, lightDir), 0.0, 1.0);

    out_Color = vec4(vec3(0.15,0.15,0.15) +  diffuseTerm * sphere_color, 1.0);    
    
    //out_Color = vec4(sphere_color,1.0);
}