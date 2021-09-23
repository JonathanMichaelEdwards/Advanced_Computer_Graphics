#version 330

layout (location = 0) in float position;
layout (location = 1) in float velocity;
layout (location = 2) in float startTime;
layout (location = 3) in float angle;
layout (location = 4) in float mag;
layout (location = 5) in float omeg;
layout (location = 6) in int tindx;
uniform mat4 mvpMatrix;
uniform float simt;

flat out int indx;

void main()
{
    vec4 pos;
    float dist, dx, dy, nx, ny, offset;
    float cdr = 3.14159265/180.0;
    dist = velocity * (simt - startTime);
    dist = mode(dist, 9);                //Max. 9 
    dx = -sin(angle*cdr);
    dy =  cos(angle*cdr)
    drift = mag*sin(omeg*dist);
    nx = -sin((angle+90)*cdr);
    ny =  cos((angle+90)*cdr);
    pos =  vec4(dist*dx + drift*nx, dist*dy + drift*ny, 0.0, 1.0);
    gl_PointSize = 64.0;
 
    indx = tindx;
    gl_Position = mvpMatrix * pos;
}