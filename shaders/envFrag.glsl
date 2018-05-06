#version 430 core

smooth in vec3 fragPos;
smooth in vec3 fragUV;
smooth in vec3 fragNormal;

uniform samplerCube envMap;

out vec4 fragColour;

void main()
{
    fragColour = texture(envMap, fragUV);
}
