#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

/* Program Uniforms */
uniform mat4 mvpMatrix;
uniform float water_level;
uniform float snow_level;

/* Send states to Frag. shader */
flat out int water_level_state;  
flat out int snow_level_state;  

/* Send to Frag. shader */
out vec2 tex_coords;
out vec3 tex_weights;
out vec4 lighting_color;


void main()
{
    float xmin = -45, xmax = +45, zmin = 0, zmax = -100;


    /* Lighting - Find the normal of the patch */
    vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    // vec3 normal = normalize(cross(a, b));
    
    // vec3 normal = -normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));



    for(int i = 0; i < 3; i++)
    { 
        vec4 posn = gl_in[i].gl_Position;

        /* Texture weighting */
        if (posn.y < water_level) {
            posn.y = water_level;  // Flat plane movement
            tex_weights = vec3(1, 0, 0);    // water
        } else if (posn.y > snow_level)
            tex_weights = vec3(0, 1, 0);    // snow
        else
            tex_weights = vec3(0, 0, 1);    // grass

        /* Mapping texture coords. */
        tex_coords.s = ((posn.x) - xmin) / (xmax - xmin);
        tex_coords.t = ((posn.z) - zmin) / (zmax - zmin);

        gl_Position = mvpMatrix * posn;
        EmitVertex();
    }

    EndPrimitive();
}

