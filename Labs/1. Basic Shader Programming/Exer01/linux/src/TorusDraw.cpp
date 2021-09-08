//  ========================================================================
//  COSC422: Advanced Computer Graphics;  University of Canterbury (2016)
//
//  FILE NAME: TorusDraw.cpp
//
//	The program generates and loads the mesh data for a torus.
//  The vertex shader includes code for both transformations and lighting.
//  Required files: Shader.h, Torus.h, Torus.cpp, Torus.vert, Torus.frag
//  Ref: Exer01.pdf
//  ========================================================================

#include <iostream>
#include <GL/glew.h>
#include <unistd.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Torus.h"
using namespace std;
#define  GLM_FORCE_RADIANS

GLuint mvMatrixLoc, mvpMatrixLoc, norMatrixLoc, lgtLoc;
float angle = 0;
Torus *torus;
float CDR = 3.14159265/180.0;   //Conversion from degrees to radians (required in GLM 0.9.6)

void initialise()
{
	   char cwd[4096];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);	
   } else {
       perror("getcwd() error");
   }
   
	// GLuint program = createShaderProg("./src/shaders/Torus.vert", "./src/shaders/Torus.frag"); // load shaders
	GLuint program = createShaderProg("./src/shaders/Torus_2.vert", "./src/shaders/Torus_2.frag"); // load shaders



	// Gets the locations of uniform variables defined by the vertex shader. 
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");
	lgtLoc = glGetUniformLocation(program, "lightPos");

	torus = new Torus(4.0, 8.0, 36, 36);
	
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void update(int value)
{
	angle++;  // Updating the rotating angle
	glutTimerFunc(50.0, update, 0);
	glutPostRedisplay();
}

void display()
{
	glm::vec4 light = glm::vec4(20.0, 10.0, 20.0, 1.0);
	glm::mat4 proj = glm::perspective(60.0f*CDR, 1.0f, 10.0f, 40.0f);  //perspective projection matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 25.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
	glm::mat4 mvMatrix = glm::rotate(view, angle*CDR, glm::vec3(1.0, 0.0, 0.0));  //rotation matrix
	glm::mat4 mvpMatrix = proj * mvMatrix;   //The model-view-projection matrix
	glm::vec4 lightEye = view * light;     //Light position in eye coordinates
	glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here
	glUniform4fv(lgtLoc, 1, &lightEye[0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	torus->render();

	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("A Torus");
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
	glutTimerFunc(50, update, 0); 
	glutMainLoop();
}

