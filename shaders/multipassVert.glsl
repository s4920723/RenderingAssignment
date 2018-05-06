#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

uniform mat4 M;
uniform mat4 MV;
uniform mat3 normalMatrix;
uniform mat4 MVP;

out vec2 wsUV;

void main()
{
    wsUV = inUV;
    gl_Position = M * vec4(inPosition, 1.0);
}
