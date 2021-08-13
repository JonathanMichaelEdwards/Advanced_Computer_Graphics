#version 330

/* Program Uniforms */
uniform sampler2D _tex_water_;
uniform sampler2D _tex_grass_;
uniform sampler2D _tex_snow_;

/* Recieve from Geom. shader */
in vec2 tex_coords;
in vec3 tex_weights;
in vec4 lighting_color;


void main() 
{
	/* Get color from each texture */
	vec4 _tex_water = texture(_tex_water_, tex_coords); 
	vec4 _tex_grass = texture(_tex_grass_, tex_coords);
	vec4 _tex_snow = texture(_tex_snow_, tex_coords);


	vec4 tex_water = _tex_water * tex_weights.x;
	vec4 tex_snow  = _tex_snow  * tex_weights.y;
	vec4 tex_grass = _tex_grass * tex_weights.z;
	

	// gl_FragColor = lighting_color * (tex_water + tex_snow + tex_grass);
	gl_FragColor = tex_water + tex_snow + tex_grass;
}








// #version 400

// layout (triangles) in;
// layout (triangle_strip, max_vertices = 3) out;

// uniform mat4 mvpMatrix;
// uniform vec4 light;
// uniform vec3 scale;
// in vec4 color[3];
// out vec4 oColor;
// out vec3 textureWeights;
// out vec2 textureCoords;

// vec4 calculateN(vec3 p1, vec3 p2, vec3 p3) {
//     return normalize(vec4(cross(p2 - p1, p3 - p1), 0));
// }

// void main()
// {
//     // Levels
//     float H = 10;
//     float waterScale = scale.x;
//     float grassScale = scale.y;
//     float grassSnowScale = scale.z;
//     float waterLevel = H * waterScale;
//     float grassLevel = H * grassScale;
//     float grassSnowLevel = H * grassSnowScale;

//     // Modify position to not go below water level
//     vec4 outPosition[3];
//     for (int i = 0; i < 3; i++) {
//         outPosition[i] = gl_in[i].gl_Position;
//         if (outPosition[i].y < waterLevel) outPosition[i].y = waterLevel;
//     }

//     // Lighting calculations
//     vec4 n = calculateN(outPosition[0].xyz, outPosition[1].xyz, outPosition[2].xyz);
//     vec4 diffuse = dot(light, n) * vec4(1);
//     oColor = diffuse + vec4(0.3);

//     for (int i = 0; i < 3; i++) {

//         // Assign texture coordinates
//         float textureScaleFactor = 30;
//         textureCoords.s = (outPosition[i].x + 45) / textureScaleFactor;
//         textureCoords.t = (outPosition[i].z + 100) / textureScaleFactor;

//         // Assign texture weights
//         float y = outPosition[i].y;
//         if (y == waterLevel){
//             textureWeights = vec3(1, 0, 0);
//         } else if (y < grassLevel) {
//             textureWeights = vec3(0, 1, 0);
//         } else if (y < grassSnowLevel){
//             float snowTerm = (y - grassLevel) / (grassSnowLevel - grassLevel);
//             textureWeights = vec3(0, 1 - snowTerm, snowTerm);
//         } else {
//             textureWeights = vec3(0, 0, 1);
//         }

//         gl_Position = mvpMatrix * outPosition[i];
//         EmitVertex();
//     }
//     EndPrimitive();
// }