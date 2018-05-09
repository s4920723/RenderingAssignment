#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec3 fragLightPos;
const float PI = 3.14159265359;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 camPos;
uniform float IOR;
out vec4 fragColour;

//Texture uniforms
uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;
uniform sampler2D normalMap;
uniform samplerCube envMap;
uniform sampler2D albedoMap2;


// N - normal
// H - half-vector
// V - view dir
// L - light dir


vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, fragUV).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(fragUV);
    vec2 st2 = dFdy(fragUV);

    vec3 N   = normalize(fragNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float chiGGX(float v)
{
  return v > 0 ? 1 : 0;
}

float GGXDistribution(vec3 N, vec3 H, float roughness)
{
  float NdotH = dot(N,H);
  float alpha2 = roughness * roughness;
  float NdotH2 = NdotH * NdotH;
  float den = NdotH2 * alpha2 + (1 - NdotH2);
  return (chiGGX(NdotH) * alpha2) / ( PI * den * den );
}

float GeometicAttenuation(vec3 V, vec3 H, vec3 N, float roughness)
{
  float VdotH2 = clamp(dot(V,H), 0.0, 1.0);
  float chi = chiGGX( VdotH2 / clamp(dot(V,N), 0.0, 1.0) );
  VdotH2 = VdotH2 * VdotH2;
  float tan2 = ( 1 - VdotH2 ) / VdotH2;
  return (chi * 2) / ( 1 + sqrt( 1 + roughness * roughness * tan2 ) );
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


void main()
{
  //Read values from maps
  vec3 albedo = pow(texture(albedoMap2, fragUV).xyz, vec3(2.2));
  float metallic = texture(metallicMap,fragUV).r;
  float roughness = texture(roughnessMap, fragUV).r;
  float ao = texture(aoMap, fragUV).r;

  //Calculate all direction vectors
  vec3 N = getNormalFromMap();
  vec3 V = normalize(camPos - fragPos);
  vec3 L = normalize(fragLightPos - fragPos);;
  vec3 H = normalize(L + V);
  vec3 R = reflect(V, N);

  //Environment map
  vec3 envColour = texture(envMap, R).rgb;
  vec3 F0 = mix(albedo, envColour, metallic);

  //Calculate radiance based on distance
  float distance = length(fragLightPos - fragPos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = lightCol * attenuation;

  //Calculate cook-torrance
  float NDF = GGXDistribution(N, H, roughness);
  float G = GeometicAttenuation(V, H, N, roughness);
  vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
  vec3 nominator    = NDF * G * F;
  float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  vec3 specular = nominator / denominator;

  //Energy conservation
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;

  vec3 Lo = (kD * albedo/PI + specular) * radiance * max(dot(N, L), 0.0);

  vec3 ambient = vec3(0.03) * albedo * ao;
  vec3 outColour = ambient + Lo;
  outColour = outColour/(outColour + vec3(1.0));
  outColour = pow(outColour, vec3(0.454));
  fragColour = vec4(outColour, 1.0);
}
