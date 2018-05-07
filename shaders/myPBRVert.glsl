#version 430 core

layout(location = 0) vec3 vertPos;
layout(location = 1) vec2 vertUV;
layout(location = 2) vec3 vertNormals;

uniform vec3 viewerPos;
uniform mat4 M;
uniform mat4 MV;
uniform mat4 MVP;
uniform mat4 normalMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;

void main()
{
  //Vertex shader outputs
  fragPos = vec3(M * vec4(vertPos, 1.0f));
  fragNormal = normalize(normalMatrix * vertNormals);
  fragUV = vertUV;
  gl_Position = MVP * vec4(vertPos, 1.0);
}
