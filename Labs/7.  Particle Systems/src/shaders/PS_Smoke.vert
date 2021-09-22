#version 330

/* 6 VBOs + index */
layout (location = 1) in float velocity;
layout (location = 2) in float startTime;
layout (location = 3) in float angle;
layout (location = 4) in float mag;
layout (location = 5) in float omeg;
layout (location = 0) in float posX;

layout (location = 6) in int texIndex; // tex index

uniform mat4 mvpMatrix;
uniform float simt;

const float DMAX = 9;   // Maximum distance of a particle.


flat out int tex_index;  // send this to the frag. shader 
out float tex_dist;

void main()
{
    vec4 posn;
    float dist, dx, dy, nx, ny, drift;
    float cdr = 3.14159265/180.0;
    dist = velocity * (simt - startTime);
    dist = mod(dist, DMAX);      
    dx = -sin(angle*cdr);
    dy =  cos(angle*cdr);
    drift = mag*sin(omeg*dist);
    nx = -sin((angle+90)*cdr);
    ny =  cos((angle+90)*cdr);
    posn =  vec4(posX + dist*dx + drift*nx, dist*dy + drift*ny, 0.0, 1.0);
     
    gl_PointSize = 30 * (dist * (DMAX - dist)) / (DMAX - dist);
    tex_dist = dist;

    gl_Position = mvpMatrix * posn;

    tex_index = texIndex;
}

