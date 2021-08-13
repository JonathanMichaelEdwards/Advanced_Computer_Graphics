#version 410

// layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 texCoord;

uniform sampler2D _tex_water;
uniform sampler2D _tex_grass;
uniform sampler2D _tex_snow;

// in vec2 TexCoord;
// in float diffTerm;

in vec2 texCoord;
flat in int water_level_state;
flat in int snow_level_state;


vec4 blue = vec4(0, 0, 1, 1);


void main() 
{
	/* Get colour from textures */
	vec4 tex_water = texture(_tex_water, texCoord); 
	vec4 tex_grass = texture(_tex_grass, texCoord);
	vec4 tex_snow = texture(_tex_snow, texCoord);

	// gl_FragColor = tex_moss; //diffTerm*(0.8*texColor1 + 0.2*texColor2);  //Apply lighting term.
	// gl_FragColor = tex_water; //vec4(0, 0, 1, 1);


	if (water_level_state == 1) 
		gl_FragColor = tex_water;  /* Water Texture */
	else if (snow_level_state == 1) 
		gl_FragColor = tex_snow;  /* Water Texture */
	else
		gl_FragColor = tex_grass;
}
