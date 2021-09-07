//  ========================================================================
//  COSC422: Advanced Computer Graphics (2014);  University of Canterbury.
//
//  FILE NAME: version.cpp
//
//	Prints the OpenGL version supported by the hardware.
//  ========================================================================

#include <iostream>
#include <gl/glew.h>
#include <GL/glut.h>
using namespace std;

void display(void)   //Nothing to display!
{ 
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush(); 
} 

void initialise()
{
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	GLint major, minor;
	glGetIntegerv(GL_MINOR_VERSION, &minor);   //requires glew.h
	glGetIntegerv(GL_MAJOR_VERSION, &major);   //requires glew.h
	cout << "OpenGL version: " << version << endl;
	cout << "OpenGL vendor: " << vendor << endl;
	cout << "OpenGL renderer: " << renderer << endl;
	cout << "Version (ints): " << major << "." << minor << endl;
}


//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char **argv) 
{ 
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
	glutInitWindowSize(200, 200);
	glutInitWindowPosition(600, 20);
	glutCreateWindow("OpenGL Info");
	initialise();
	glutDisplayFunc(display); 

	glutMainLoop(); 

	return EXIT_SUCCESS;
}