#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;  /* A patch is made of a Quad with two triangles. 
                                                   A triangle has 3 verticies */

/* Program Uniforms */
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 invMatrix;
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
// out vec4 posn[3];
// out vec3 vPos;

#define DENSITY   0.05
#define GRADIENT  1


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
        
        normWorld = vec4(eyePos.xyz * normalize(posn[i].xyz), 0.0);
    }

    /* Lighting - Find the face normal of each sub patch */
    vec3 a = posn[1].xyz - posn[0].xyz;
    vec3 b = posn[2].xyz - posn[0].xyz;
    // vec4 normal_f = normalize(vec4(cross(a, b), 0));
    // vec4 normal_f = normalize(vec4(cross(dFdx(a), dFdy(b)), 0));
    

    // Replace vec3 fdx = dFdx(vPos)  by:
    // vec3 fdx = vec3(dFdx(eyePos.x), dFdx(eyePos.y), dFdx(eyePos.z));

    // Replace vec3 fdy = dFdy(vPos)  by:
    // vec3 fdy = vec3(dFdy(vPos.x),dFdy(vPos.y),dFdy(vPos.z));

    vec4 normal_f = normalize(vec4(cross(a, b), 0));


    // vec3 p;
    // p.x = (posn[0].x + posn[1].x + posn[2].x) / 3;
    // p.y = (posn[0].y + posn[1].y + posn[2].y) / 3;
    // p.z = (posn[0].z + posn[1].z + posn[2].z) / 3;

    // vec4 normal_v = normalize(vec4(p, 1));


    // vec4 posnEye = mvMatrix * posn;
    // vec4 lgtVec = normalize(light_pos - eyePos); 


    /* Ambiant + Diffuse Lighting - Toggle smoothness */
    

    vec4 diffuse;
    // if (lighting_state == 1)
        diffuse = dot(light_pos, normWorld) * vec4(1);  // diffuse brightness
    // else
    //     diffuse = vec4(0);

    lighting_color = diffuse + vec4(0.3);








    // vec4 diffuse_f = dot(light, normal_f) * vec4(1);  // https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/shading-normals
    // vec4 diffuse_v = dot(light, normal_v) * vec4(1);
    
    // lighting_color = diffuse_f + vec4(0.3);
    // lighting_color = diffuse_v + vec4(0.3);





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

        /* Fog calculations - Fixed Pos.*/
        if (fog_state == 0) { 
            visibility = exp(-pow(length(posn[i]*DENSITY), GRADIENT));  // Exponential curve
            visibility = clamp(visibility, 0.0f, 1.0f);
        } else
            visibility = 1.0f;


        // vec4 posnEye = mvMatrix * posn[i];
        // vec4 normalEye = invMatrix * vec4(normal[i], 0);
        // vec4 lgtVec = normalize(light_pos - posnEye); 
        // vec4 viewVec = normalize(vec4(-posnEye.xyz, 0)); 
        // vec4 material = vec4(0.0, 1.0, 1.0, 1.0);  // cyan
        // vec4 ambOut = vec4(0.3);
        // float diffTerm = max(dot(lgtVec, normalEye), 0);
        // vec4 diffOut = diffTerm * vec4(1);

        // lighting_color = ambOut + diffOut;


        /* Ambiant + Diffuse Lighting - Toggle smoothness */
        // vec4 diffuse;
        // if (lighting_state == 1)
        //     diffuse = dot(light_pos, normal_f) * vec4(1);    
        // else
        //     diffuse = dot(light_pos, normal_v) * vec4(1);

    
        gl_Position = mvpMatrix * posn[i];
        
        EmitVertex();
    }
    EndPrimitive();
}

