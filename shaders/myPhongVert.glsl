#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

uniform mat4 M;
uniform mat4 MV;
uniform mat3 normalMatrix;
uniform mat4 MVP;


smooth out vec3 wsPosition;
smooth out vec3 wsNormal;
smooth out vec2 wsUV;

void main()
{
  wsNormal = normalize(inNormal * normalMatrix);
  wsPosition = vec3(MV * vec4(inPosition, 1.0f));
  wsUV = inUV;
  gl_Position = MVP*vec4(inPosition, 1.0);
}
