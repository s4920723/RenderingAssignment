#version 430 core

in vec2 wsUV;
out vec4 fragColour;
uniform sampler2D fboTexture;

void main()
{
    fragColour = texture(fboTexture, wsUV);
}
