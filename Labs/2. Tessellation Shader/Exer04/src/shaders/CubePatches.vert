#version 330

layout (location = 0) in vec4 position;

    // No need to times position by mvpMatrix (This is passed through to the Evaluation Shader)

void main()
{   
    gl_Position = position;
}