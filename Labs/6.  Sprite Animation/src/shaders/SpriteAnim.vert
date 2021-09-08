#version 330
layout (location = 0) in vec3 position;
uniform mat4 mvpMatrix;

void main()
{
    gl_PointSize = 200.0;
    gl_Position = mvpMatrix * vec4(position, 1);
}