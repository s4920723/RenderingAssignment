#version 430 core

in vec2 wsUV;

out vec4 fragColour;

uniform sampler2D fboTexture;
uniform sampler2D fboDepth;
uniform vec2 fboSize;
uniform float focalDepth;
uniform float blurRadius;
uniform bool blurSwitch;

//samples taken from http://www.geeks3d.com/20100628/3d-programming-ready-to-use-64-sample-poisson-disc/
const vec2 poissonDisc[12] = {vec2(-0.326212,-0.40581),vec2(-0.840144,-0.07358),
                              vec2(-0.695914,0.457137),vec2(-0.203345,0.620716),
                              vec2(0.96234,-0.194983),vec2(0.473434,-0.480026),
                              vec2(0.519456,0.767022),vec2(0.185461,-0.893124),
                              vec2(0.507431,0.064425),vec2(0.89642,0.412458),
                              vec2(-0.32194,-0.932615),vec2(-0.791559,-0.59771)};

float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

vec4 poissonFilter(vec2 texPos, float sigma)
{
    vec4 colour = texture(fboTexture, texPos);
    float angle = random(texPos);
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    for (int i = 0; i < 12; ++i)
    {
      vec2 samplePos = texPos + 2 * sigma * rot * poissonDisc[i];
      colour += texture(fboTexture, samplePos);
    }
    return colour * 0.076923077;
}

void main()
{
  if (blurSwitch)
  {
    vec2 texPos = gl_FragCoord.xy / fboSize;
    float sigma = abs(focalDepth - texture(fboDepth, texPos).x) * blurRadius;
    fragColour = poissonFilter(texPos, sigma);
  }
  else
  {
    vec2 texPos = gl_FragCoord.xy / fboSize;
    fragColour = texture(fboTexture, texPos);
  }
}
