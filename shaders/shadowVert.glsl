#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

uniform mat4 M;
uniform mat4 MVP;

out vec2 fragUV;

void main()
{
  fragUV = inUV;
  gl_Position =  M * vec4(inPosition, 1.0);
}
