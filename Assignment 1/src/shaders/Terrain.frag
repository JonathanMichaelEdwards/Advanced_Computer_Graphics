#version 410

// layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 texCoord;

uniform sampler2D grass;
// uniform sampler2D tSampler2;

// in vec2 TexCoord;
// in float diffTerm;

in vec2 texCoord;

void main() 
{
	vec4 tex_grass = texture(grass, texCoord);  // Get colour from texture  
	// vec4 texColor2 = texture(tSampler2, TexCoord);  // Moss

	// gl_FragColor = tex_moss; //diffTerm*(0.8*texColor1 + 0.2*texColor2);  //Apply lighting term.
     gl_FragColor = tex_grass; //vec4(0, 0, 1, 1);
}
