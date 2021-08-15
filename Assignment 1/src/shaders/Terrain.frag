#version 330

/* Program Uniforms */
uniform sampler2D _tex_water_;
uniform sampler2D _tex_grass_;
uniform sampler2D _tex_snow_;
uniform vec4 light_pos;
uniform vec4 eyePos;  /* Eye view */

/* Recieve from Geom. shader */
in vec2 tex_coords;
in vec3 tex_weights;
in vec4 lighting_color;
in float visibility;
// in vec4 posn[3];
// in vec3 vPos;

out vec4 out_color;


void main() 
{

	//     /* Lighting - Find the face normal of each sub patch */
    // vec3 a = posn[1].xyz - posn[0].xyz;
    // vec3 b = posn[2].xyz - posn[0].xyz;

    

    // // Replace vec3 fdx = dFdx(vPos)  by:
    // vec3 fdx = vec3(dFdx(vPos.x), dFdx(vPos.y), dFdx(vPos.z));

    // // Replace vec3 fdy = dFdy(vPos)  by:
    // vec3 fdy = vec3(dFdy(vPos.x),dFdy(vPos.y),dFdy(vPos.z));

    // vec3 normal_f = normalize((cross(fdx, fdy)));


    // /* Ambiant + Diffuse Lighting - Toggle smoothness */
    // vec4 diffuse;
    // // if (lighting_state == 1)
    //     diffuse = dot(light_pos.xyz, normal_f) * vec4(1);    
    // // else
    // //     diffuse = vec4(0);

    // vec4 lighting_color = diffuse + vec4(0.3);



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

