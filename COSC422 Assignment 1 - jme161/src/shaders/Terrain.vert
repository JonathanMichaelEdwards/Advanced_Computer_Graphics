#version 330

layout (location = 0) in vec4 position;  /* Input in world coordinate */

void main()
{
   gl_Position = position;  /* pass to Control shader */
}

