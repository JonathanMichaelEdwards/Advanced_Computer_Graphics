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
out float normals[4];  // 4 faces to compare from


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
    vec3 zero_a = posn_eye[2].xyz - posn_eye[0].xyz;  // Vertex 0
    vec3 zero_b = posn_eye[4].xyz - posn_eye[0].xyz;

    vec3 normal_unit = normalize(cross(zero_a, zero_b));  // Centre face - normalEye
    vec3 light_vect = normalize(lightPos.xyz - posn_eye[0].xyz);  // light source vector - light source to the light
    
    diffTerm = dot(light_vect, normal_unit);  // l.n


    /* 
     * Silhouette Edge Identification 4 - faces (3 outside + 1 centre) to consider. 
     * Find 3 outside face normals. - Right hand rule (Anti-Clockwise)
     */
    vec3 one_a = posn_eye[2].xyz - posn_eye[1].xyz;  // Vertex 1
    vec3 one_b = posn_eye[0].xyz - posn_eye[1].xyz;

    vec3 two_a = posn_eye[4].xyz - posn_eye[3].xyz;  // Vertex 3
    vec3 two_b = posn_eye[2].xyz - posn_eye[3].xyz;

    vec3 three_a = posn_eye[0].xyz - posn_eye[5].xyz;  // Vertex 5
    vec3 three_b = posn_eye[4].xyz - posn_eye[5].xyz;


    // Surface normals - 4 faces
    vec3 normal_0 = normalize(cross(zero_a, zero_b));  // Face 1
    vec3 normal_1 = normalize(cross(one_a, one_b));  // Face 2
    vec3 normal_2 = normalize(cross(two_a, two_b));  // Face 3
    vec3 normal_3 = normalize(cross(three_a, three_b));  // Face 4
    

    normals[0] = normal_0.z;
    normals[1] = normal_1.z;
    normals[2] = normal_2.z;
    normals[3] = normal_3.z;


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

