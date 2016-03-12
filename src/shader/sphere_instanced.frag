#version 330 core

in vec3 normal;
in vec3 color;

uniform vec4 lightPos;
 
out vec4 out_Color;
 
void main()
{
  float dot1 = dot(normalize(lightPos.xyz), normal);
  float backlight = clamp(-dot1,0.0,0.5);
  vec3 ambient = vec3(0.1, 0.1, backlight); // ambient
  
  float diffuse = clamp(dot1, 0.0, 1.0);
  
  out_Color = vec4(ambient + diffuse * color, 1.0);
}