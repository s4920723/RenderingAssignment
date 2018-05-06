#version 430 core

in vec3 wsNormal;
in vec2 wsUV;
in vec3 lightDir;
in vec3 eyeDir;

const float PI = 3.14159265359;

struct lightInfo
{
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform sampler2D tilesOnyx;
uniform lightInfo keyLight;
out vec4 fragColour;

// N - normal
// H - half-vector
// V - view dir
// L - light dir

float cookTorrance(vec3 N, vec3 H, vec3 L, vec3 V, float roughness, float IOR)
{
   float NdotH = max(dot(N, H), 0.0);
   float NdotL = max(dot(N, L), 0.0);
   float NdotV = max(dot(N, V), 0.0);
   float VdotH = max(dot(V, H), 0.0);

   //GGX Distribution
   float a = pow((roughness * roughness), 2);
   float distribution = a/(PI* pow(((NdotH*NdotH)*(a -1)+1), 2));

   //Geometric Attenuation
   float invVdotH = 1.0 / max(dot(V, H), 0.0);
   float g1 = ((2.0 * NdotH) * NdotV) * invVdotH;
   float g2 = ((2.0 * NdotH) * NdotL) * invVdotH;
   float geoAtt = min(1.0, min(g1, g2));

   //Fresnel Term
   float g = sqrt((IOR*IOR) + (VdotH*VdotH) - 1);
   float c = VdotH;
   float fresnel = 0.5 * (pow((g-c), 2)/(2*pow((g+c), 2))) * (1+pow((c*(g + c)-1, 2))/pow((c*(g - c)+1, 2)));

   return (distribution * geoAtt * fresnel)/NdotV;
}

void main()
{

    fragColour=vec4(texture(tilesOnyx, wsUV));
}
