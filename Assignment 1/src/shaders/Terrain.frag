#version 330

/* Program Uniforms */
uniform sampler2D _tex_water_;
uniform sampler2D _tex_grass_;
uniform sampler2D _tex_snow_;

/* Recieve from Geom. shader */
in vec2 tex_coords;
in vec3 tex_weights;
in vec4 lighting_color;
in float visibility;
in float diffTerm_NL; 

out vec4 out_color;


void main() 
{
	/* Get color from each texture */
	vec4 _tex_water = texture(_tex_water_, tex_coords); 
	vec4 _tex_grass = texture(_tex_grass_, tex_coords);
	vec4 _tex_snow = texture(_tex_snow_, tex_coords);


	vec4 tex_water = _tex_water * tex_weights.x;
	vec4 tex_snow  = _tex_snow  * tex_weights.y;
	vec4 tex_grass = _tex_grass * tex_weights.z;
	

	/* 
	 * Output color of objects, then Mix skycolor with 
	 * visibility to get fog effect.
	*/
	vec4 sky_color = vec4(0.3f);  // grey
	out_color = lighting_color * (tex_water + tex_snow + tex_grass);
	out_color = mix(sky_color, out_color, visibility);  // Final output - Mix Fog
}

