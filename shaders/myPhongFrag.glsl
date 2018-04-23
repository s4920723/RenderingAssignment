#version 430 core

in vec3 WSVertexPosition;
in vec3 WSVertexNormal;
in vec2 WSTexCoord;

out vec4 fragColour;

uniform vec3 diffuseColour;

void main()
{
    fragColour=vec4(diffuseColour, 1.0);
}
