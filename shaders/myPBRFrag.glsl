#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
const float PI = 3.14159265359;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 viewerPos;
uniform float IOR;
out vec4 fragColour;

//Texture uniforms
uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;
uniform sampler2D normalMap;


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
  float VdotH2 = clamp((dot(V,H), 0.0, 1.0));
  float chi = chiGGX( VdotH2 / clamp((dot(V,N), 0.0, 1.0)) );
  VdotH2 = VdotH2 * VdotH2;
  float tan2 = ( 1 - VdotH2 ) / VdotH2;
  return (chi * 2) / ( 1 + sqrt( 1 + roughness * roughness * tan2 ) );
}

float fresnel(vec3 V, vec3 H, float IOR)
{
  float VdotH = dot(V,H);
  float g = sqrt((IOR*IOR) + (VdotH*VdotH) - 1);
  float c = VdotH;
  return 0.5 * (pow((g-c), 2)/(2*pow((g+c), 2))) * (1+pow((c*(g + c)-1, 2))/pow((c*(g - c)+1, 2)));
}


void main()
{
  //Read values from maps
  vec3 albedo = pow(texture(albedoMap, fragUV), vec3(2.2));
  float metallic = texture(metallicMap,fragUV).r;
  float roughness = texture(roughnessMap, fragUV).r;
  float ao = texture(aoMap, fragUV).r;

  //Calculate all direction vectors
  vec3 N = fragNormal;
  vec3 V = normalize(viewerPos - fragPos);
  vec3 L = normalize(lightPos - fragPos);;
  vec3 H = normalize(L + V);

  //Calculate radiance based on distance
  float distance = length(lightPos - fragPos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = lightCol * attenuation;

  //Calculate cook-torrance
  float NDF = GGXDistribution(N, H, roughness);
  float G = GeometicAttenuation(V, H, N, roughness);
  vec3 F = fresnel(V, H, IOR);
  vec3 nominator    = NDF * G * F;
  float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  vec3 specular = nominator / denominator;

  //Energy conservation
  vec3 kS = F; //specular light
  vec3 kD = vec3(1.0) - kS; //specular light
  kD *= 1.0 - metallic;



  vec3 outColour
  outColour = pow(outColour, 0.454);
  fragColour = vec4(outColour, 1.0);
}
