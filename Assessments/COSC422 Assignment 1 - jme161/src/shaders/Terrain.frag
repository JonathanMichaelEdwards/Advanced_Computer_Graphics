#version 330

/* Program Uniforms */
uniform sampler2D _tex_water;
uniform sampler2D _tex_grass;
uniform sampler2D _tex_snow;

/* Recieve from Geom. shader */
in vec2 tex_coords;
in vec3 tex_weights;
in vec4 lighting_color;
in float visibility;

out vec4 out_color;  /* Output fragment */

#define   SKY_COLOR    vec4(0.3)  // grey


void main() 
{
	/* Get color from each texture times the weight*/
	vec4 tex_water = texture(_tex_water, tex_coords)  * tex_weights.x;
	vec4 tex_snow  = texture(_tex_snow,  tex_coords)  * tex_weights.y;
	vec4 tex_grass = texture(_tex_grass, tex_coords)  * tex_weights.z;
	

	// /* 
	//  * Output color of objects, then Mix skycolor with 
	//  * visibility to get fog effect.
	// */
	out_color = lighting_color * (tex_water + tex_snow + tex_grass);
	out_color = mix(SKY_COLOR, out_color, visibility);  // Final output - Mix Fog
}

