#version 430 core

out float fragColour;

void main()
{
  fragColour= gl_FragCoord.z;
}
