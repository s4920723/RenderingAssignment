#version 430 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

uniform vec3 viewerPos;
uniform mat4 M;
uniform mat4 MV;
uniform mat3 normalMatrix;
uniform mat4 MVP;

smooth out vec3 fragPos;
smooth out vec3 fragUV;
smooth out vec3 fragNormal;

void main()
{
  fragNormal = normalize(normalMatrix * inNormal);
  fragPos = vec3(MV * vec4(inPos, 1.0) );
  fragUV = inPos;
  gl_Position = MVP*vec4(inPos, 1.0);
}
