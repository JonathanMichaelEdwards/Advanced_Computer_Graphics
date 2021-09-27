#version 330


/* Program Uniforms */
uniform int render_mode;
uniform int pencil_blending_mode;

in float diffTerm;
in vec4 lighting_color;
out vec4 outColor;


void main() 
{
    outColor = lighting_color;
}

