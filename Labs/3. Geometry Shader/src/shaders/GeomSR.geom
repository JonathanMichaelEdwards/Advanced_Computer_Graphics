#version 400

layout (lines) in;
layout (triangle_strip, max_vertices = 80) out;
uniform mat4 mvpMatrix;

void main()
{
    // Receive's the end points of each line segment in world coordinates 
    //      - From the Vertex Shader (via gl_in[].glPosition)
    float x1 = gl_in[0].gl_Position.x;
    float y1 = gl_in[0].gl_Position.y;
    float x2 = gl_in[1].gl_Position.x;
    float y2 = gl_in[1].gl_Position.y;

    float xnew, znew;
    float theta, ctheta, stheta;
    int i;
    
    for(i = 0; i <= 36; i++)
    {
        theta = i * 0.17453;  //step size = 10 degs
        ctheta = cos(theta);
        stheta = sin(theta);
        xnew = x1 * ctheta;
        znew = -x1 * stheta;
        gl_Position = mvpMatrix * vec4(xnew, y1, znew, 1);

        EmitVertex();  // The function EmitVertex() must be called when a geometry 
                       // shader outputs a vertex. 

        xnew = x2 * ctheta;
        znew = -x2 * stheta;
        gl_Position = mvpMatrix * vec4(xnew, y2, znew, 1);

        EmitVertex();
    }

    EndPrimitive();
}

