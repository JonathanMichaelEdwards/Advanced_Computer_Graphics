/*  =================================================================
*  COSC363: Computer Graphics (2020);  University of Canterbury.
*
*  FILE NAME: Teapot.cpp
*  A very basic OpenGL program to display a teapot
*  =================================================================*/

// Compile and Run:
// - Linux:   g++ Teapot.cpp -o Teapot -lGL -lGLU -lglut && ./Teapot
// - Windows: g++ Teapot.cpp -o Teapot -lOpenGL32 -lGLU32 -lfreeGLUT && Teapot



#include <GL/freeglut.h>


//==================================================================
// This is the main display module containing OpenGL function calls for
// rendering a scene

void display(void) 
{ 
	float lpos[4] = {0., 10., 10., 1.0};  //light's position

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    gluLookAt(0, 2, 11, 0, 0, 0, 0, 1, 0);		//Camera position and orientation

	glLightfv(GL_LIGHT0,GL_POSITION, lpos);		//Set light's position

    glColor3f(0.0, 1.0, 1.0);					//Teapot's material color
	glRotated(10, 1, 0, 0);						//A rotation about the x-axis
    glutSolidTeapot(1.0);

	glFlush(); 
} 

//==================================================================
//  Initialize function:
//  Used for initializing OpenGL state (lighting, color material, depth test etc.)

void initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   //Background color

	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
 	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);			//The projection matrix of the camera
	glLoadIdentity();
	glFrustum(-2.0, 2.0, -2.0, 2.0, 10.0, 1000.0);   //Camera frustum definition
}


//==================================================================
//  The Main function: 
//  Initializes glut window and registers call backs

int main(int argc, char **argv) 
{ 
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);  
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Teapot");
	initialize();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0; 
}
