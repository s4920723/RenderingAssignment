#version 430 core
out vec4 fragColour;
in vec2 fragUV;

layout (binding = 9) uniform sampler2D depthMap;

void main()
{
  vec4 depthValue = texture(depthMap, fragUV);
  //gl_FragDepth = gl_FragCoord.z;
  fragColour = depthValue;
}
