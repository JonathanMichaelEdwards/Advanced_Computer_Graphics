//  ========================================================================
//  COSC422: Advanced Computer Graphics (2014);  University of Canterbury.
//
//  FILE NAME: Draw1.cpp
//
//	Draws a triangle using a single VBO (OpenGL 4.2 Core Profile).
//  This program does not use any shader code.
//  ========================================================================

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
using namespace std;

GLuint vao;

void initialise()
{
    // A single triangle
    static const GLfloat verts[] =
    {
        -0.8, -0.8,
         0.8, -0.8,
         0.0,  0.8
    };

	glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("A Triangle");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}


	initialise();
	glutDisplayFunc(display);
	glutMainLoop();
}