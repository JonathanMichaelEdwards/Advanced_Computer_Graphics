#version 330
layout (location = 0) out vec4 oColor;

uniform sampler2D renderTex;
uniform int pass;

in vec4 vColour;


void main()
{
     oColor = vColour;
}
