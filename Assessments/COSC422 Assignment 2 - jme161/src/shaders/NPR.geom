#version 330


layout (triangles_adjacency) in;
//layout (triangles) in;
layout (triangle_strip, max_vertices = 27) out; 


#define     TRI_ITR     6  // 6 elements for each triangle


/* Program Uniforms */
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

uniform float tex_min_max[4];
 

/* Outputs to Fragment shader */
out float diffTerm;  // l.n
out vec2 tex_coords;


void main()
{
    vec4 posn_eye[TRI_ITR];

    /* posnEye */
    for (int i = 0; i < TRI_ITR; i++) 
    {
        posn_eye[i] = mvMatrix * gl_in[i].gl_Position;  // eye coord space
    }


    /* Lighting - Find the face normal of each sub patch - render main normal for lighting  - anti clockwise first */
    // Normals - Centre face
    vec3 zero_a = posn_eye[2].xyz - posn_eye[0].xyz;
    vec3 zero_b = posn_eye[4].xyz - posn_eye[0].xyz;

    vec3 normal_unit = normalize(cross(zero_a, zero_b));  // Centre face - normalEye
    //vec4 normal_unit_eye = norMatrix * normalize(vec4(cross(zero_a, zero_b), 0));  // Centre face - normalEye -- add dark shading
    vec3 light_vect = normalize(lightPos.xyz - posn_eye[0].xyz);  // light source vector - light source to the light
    
    diffTerm = dot(light_vect, normal_unit);  // l.n




    // vec3 one_a = posn[2].xyz - posn[1].xyz;
    // vec3 one_b = posn[0].xyz - posn[1].xyz;




    // //vec4 normal = vec4(_normal[0], _normal[1], _normal[2], 0);//normalize(vec4(cross(a, b), 0));
    // //vec4 normal = normalize(vec4(cross(zero_a, zero_b), 0));


    // //vec4 normal = vec4(_normal[0], _normal[1], _normal[2], 0);//normalize(vec4(cross(a, b), 0));
    
    // //vec4 normal = normalize(vec4(cross(zero_a, zero_b), 0));  // 


    // // How do u calculate the normals (4 faces)
    // for (int i = 0; i < 4; i++)  // 4 faces - 6 vertices
    // {
    //     /* Lighting - Find the face normal of each sub patch */
    //     vec3 a = posn[i+1].xyz - posn[i].xyz;
    //     vec3 b = posn[i+2].xyz - posn[i].xyz;

    //     //vec4 normal = vec4(_normal[0], _normal[1], _normal[2], 0);//normalize(vec4(cross(a, b), 0));
    //     vec4 normal = normalize(vec4(cross(a, b), 0));

    //     //diffTerm
    //     lighting_color = vec4(0.3) + dot(lightPos, normal);  // diffuse brightness

    // }


    /* Texture mapping & Weighting */
    for (int i = 0; i < TRI_ITR; i++)
    {  
        /* Mapping texture coords. */
        tex_coords.s = ((posn_eye[i].x) - tex_min_max[0]) / (tex_min_max[1] - tex_min_max[0]);
        tex_coords.t = ((posn_eye[i].z) - tex_min_max[2]) / (tex_min_max[3] - tex_min_max[2]);

        gl_Position = mvpMatrix * gl_in[i].gl_Position;
        
        EmitVertex();
    }
    EndPrimitive();
}



// #version 330


// layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 normal;

// uniform mat4 mvMatrix;
// uniform mat4 mvpMatrix;
// uniform mat4 norMatrix;
// uniform vec4 lightPos;
 
// out float diffTerm;
// out vec2 texCoord;


// void main()
// {
// 	vec4 posnEye = mvMatrix * vec4(position, 1);
// 	vec4 normalEye = norMatrix * vec4(normal, 0);
// 	vec3 lgtVec = normalize(lightPos.xyz - posnEye.xyz); 
// 	vec3 unitNormal = normalize(normalEye.xyz);

// 	diffTerm = max(dot(lgtVec, unitNormal), 0.2);  // limit the dark area

// 	gl_Position = mvpMatrix * vec4(position, 1);
// }
