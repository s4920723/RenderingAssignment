#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

struct lightInfo
{
        vec4 position;
        vec4 ambient;
        vec4 diffuse;
        vec4 specular;
};

uniform lightInfo keyLight;
uniform vec3 viewerPos;
uniform mat4 M;
uniform mat4 MV;
uniform mat3 normalMatrix;
uniform mat4 MVP;

out vec3 wsNormal;
out vec2 wsUV;
out vec3 lightDir;
out vec3 eyeDir;
out vec3 reflectionDir;

void main()
{
  wsNormal = normalize(normalMatrix * inNormal);
  vec4 wsPosition = M * vec4(inPosition, 1.0f);
  vec4 eyePos = MV * vec4(inPosition, 1.0f);
  eyeDir = normalize(wsPosition.xyz - viewerPos);
  lightDir = normalize(keyLight.position.xyz - eyePos.xyz);
  lightDir /= length(lightDir);
  reflectionDir = reflect(eyeDir, normalize(inNormal));



  wsUV = inUV;
  gl_Position = MVP*vec4(inPosition, 1.0);
}
