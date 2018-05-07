#version 430 core

in vec3 wsNormal;
in vec2 wsUV;
in vec3 lightDir;
in vec3 eyeDir;
in vec3 reflectionDir;

struct lightInfo
{
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct materialInfo
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform samplerCube envMap;
uniform sampler2D albidoMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform lightInfo keyLight;
out vec4 fragColour;


vec4 phong()
{
    vec3 N = normalize(wsNormal);
    vec3 E = normalize(eyeDir);
    vec3 L = normalize(lightDir);

    //AMBIENT
    float ambientStrenght = 0.1;
    vec4 ambient = ambientStrenght * keyLight.ambient;

    //DIFFUSE
    vec4 diffuse = max(dot(N, L), 0.0) * keyLight.diffuse;

    //SPECULAR
    float specIntesity = 1;
    float highlight = 32;
    vec3 reflectDir = reflect(-L, N);
    float spec = pow(max(dot(E, reflectDir), 0.0), highlight);
    vec4 specular = specIntesity * spec * vec4(1.0, 1.0, 1.0, 1.0);

    return diffuse + specular + ambient;
}

void main()
{

    fragColour=vec4(phong() * texture(albidoMap, wsUV));
    vec4 reflectionColour = texture(envMap, reflectionDir) * texture(roughnessMap, wsUV);
    //fragColour = fragColour * reflectionColour;
}
