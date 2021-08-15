#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;  /* A patch is made of a Quad with two triangles. 
                                                   A triangle has 3 verticies */

/* Program Uniforms */
uniform mat4 mvpMatrix;
uniform vec4 eyePos;  /* Eye view */
uniform float water_level;
uniform float snow_level;
uniform vec4 light_pos;
uniform int lighting_state;
uniform int fog_state;

/* Send to Frag. shader */
out vec2 tex_coords;
out vec3 tex_weights;
out vec4 lighting_color;
out float visibility;

#define    AMBIANT     vec4(0.3)

#define    DENSITY     0.05
#define    GRADIENT    1


void main()
{
    float xmin = -45, xmax = +45, zmin = 0, zmax = -100;
    vec4 posn[3];
    vec4 normWorld;

    /* Map water level tex. */
    for (int i = 0; i < 3; i++) 
    {
        posn[i] = gl_in[i].gl_Position;
        if (posn[i].y < water_level)  // Water level threshold 
            posn[i].y = water_level;
    }

    /* Lighting - Find the face normal of each sub patch */
    vec3 a = posn[1].xyz - posn[0].xyz;
    vec3 b = posn[2].xyz - posn[0].xyz;

    vec4 normal = normalize(vec4(cross(a, b), 0));


    /* Ambiant + Diffuse Lighting - Toggle between Ambiant & Diffuse */
    vec4 diffuse;
    if (lighting_state == 0)
        diffuse = dot(light_pos, normal) * vec4(1);  // diffuse brightness
    else
        diffuse = vec4(0);

    lighting_color = AMBIANT + diffuse;


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

        /* Fog calculations - Fixed fog position */
        if (fog_state == 0) { 
            visibility = exp(-pow(length(posn[i] * DENSITY), GRADIENT));  /* Exponential curve */
            visibility = clamp(visibility, 0.0, 1.0);  /* Clamp and send to Frag. shader */
        } else
            visibility = 1.0;  /* No fog */

    
        gl_Position = mvpMatrix * posn[i];
        
        EmitVertex();
    }
    EndPrimitive();
}

