#version 400


layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;  /* A patch is made of a Quad with two triangles. 
                                                   A triangle has 3 verticies */


uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;
 
out float diffTerm;  // l.n
out vec4 lighting_color;
// out vec2 texCoord;


void main()
{
    vec4 posn[3];

    /* init posn[] */
    for (int i = 0; i < 3; i++) 
    {
        posn[i] = gl_in[i].gl_Position;
    }

    lighting_color = vec4(0.3);


    /* Texture mapping & Weighting */
    for (int i = 0; i < 3; i++)
    {  
        gl_Position = mvpMatrix * posn[i];
        
        EmitVertex();
    }
    EndPrimitive();
}


