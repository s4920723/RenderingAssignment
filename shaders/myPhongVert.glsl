#version 430 core

layout (location = 0) in vec3 inVerts;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

uniform mat4 M;
uniform mat4 MV;
uniform mat3 normalMatrix;
uniform mat4 MVP;

smooth out vec3 WSVertexPosition;
smooth out vec3 WSVertexNormal;
smooth out vec2 WSTexCoord;

void main()
{
  WSVertexNormal = normalize(inNormal * normalMatrix);
  WSVertexPosition = vec3(MV * vec4(inVerts, 1.0f));
  WSTexCoord = inUV;
  gl_Position = MVP*vec4(inVerts, 1.0);
}
