#version 430 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec3 vertNormals;

uniform vec3 lightPos;
uniform mat4 M;
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;
out vec3 fragLightPos;

void main()
{
  //Vertex shader outputs
  fragPos = vec3(M * vec4(vertPos, 1.0f));
  fragNormal = normalize(normalMatrix * vertNormals);
  fragUV = vertUV;
  fragLightPos = vec3(M * vec4(lightPos, 1.0f));
  gl_Position = MVP * vec4(vertPos, 1.0);
}
