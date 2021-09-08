#version 330

in float diffTerm;

in vec4 phongColor;
out vec4 outColor;

void main()
{
   outColor = phongColor;
}
