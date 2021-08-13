#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

/* Program Uniforms */
uniform mat4 mvpMatrix;
uniform float water_level;
uniform float snow_level;
uniform vec4 light;

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
    vec4 posn[3];

    /* Map water level tex. */
    for (int i = 0; i < 3; i++) 
    {
        posn[i] = gl_in[i].gl_Position;
        if (posn[i].y < water_level)  // Water level threshold 
            posn[i].y = water_level;
    }


    /* Lighting - Find the normal of the patch */
    vec3 a = posn[1].xyz - posn[0].xyz;
    vec3 b = posn[2].xyz - posn[0].xyz;

    vec4 normal = normalize(vec4(cross(a, b), 0));

    /*  
     * Ambiant + Diffuse Lighting 
     *  - Smooth shading via using Vertex normals
    */
    vec4 diffuse = dot(light, normal) * vec4(1);  // https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/shading-normals
    lighting_color = diffuse + vec4(0.3);


    /* Texture mapping & Weighting */
    for(int i = 0; i < 3; i++)
    { 
        /* Texture weighting */
        if (posn[i].y == water_level)
            tex_weights = vec3(1, 0, 0);    // water
        else if (posn[i].y > snow_level)
            tex_weights = vec3(0, 1, 0);    // snow
        else
            tex_weights = vec3(0, 0, 1);    // grass

        /* Mapping texture coords. */
        tex_coords.s = ((posn[i].x) - xmin) / (xmax - xmin);
        tex_coords.t = ((posn[i].z) - zmin) / (zmax - zmin);

        gl_Position = mvpMatrix * posn[i];
        EmitVertex();
    }

    EndPrimitive();
}

