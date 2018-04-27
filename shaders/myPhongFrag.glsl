#version 430 core

in vec3 wsPosition;
in vec3 wsNormal;
in vec2 wsUV;

struct lightType
{
  vec4 lightPos;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
};

struct materialType
{

};

uniform materialType material;
uniform lightType light;
uniform vec3 diffuseColour;


out vec4 fragColour;



void main()
{

    fragColour=vec4(diffuseColour, 1.0);
}
