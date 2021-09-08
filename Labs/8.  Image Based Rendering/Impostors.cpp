//  ========================================================================
//  COSC422: Advanced Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: Imposotrs.cpp
//
//  This program shows the construction of impostors using framebuffer objects
//  See Ex09.pdf for details.
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Teapot.h"
using namespace std;

Teapot* teapot;
float angle = 0;
GLuint vaoID, fboID, renderTex;
GLuint matrixLoc1, matrixLoc2, matrixLoc3, passLoc;
glm::mat4 view, proj;   //View and projection matrices
float CDR = 3.14159265/180.0;


void initialise()
{
	teapot = new Teapot(1.0);

// --- Uniform locations ---
    GLuint program = createShaderProg("Impostors.vert", "Impostors.frag");
	matrixLoc1 = glGetUniformLocation(program, "mvMatrix");    //Suffix 'T' indicates teapot
	matrixLoc2 = glGetUniformLocation(program, "mvpMatrix");
	matrixLoc3 = glGetUniformLocation(program, "norMatrix");

	passLoc = glGetUniformLocation(program, "pass");
	GLuint lgtLoc = glGetUniformLocation(program, "lightPos");

	//View and projection matrices for the teapot
	view = glm::lookAt(glm::vec3(0, 10, 20), glm::vec3(0, 2, 0), glm::vec3(0.0, 1.0, 0.0)); 
	proj = glm::perspective(20.0f*CDR, 1.0f, 10.0f, 100.0f);

	//Model view projection matrix for sprite
	glm::mat4 mvpMatrixS = proj * view;
	GLuint matrixLoc4 = glGetUniformLocation(program, "mvpMatrixS");
	glUniformMatrix4fv(matrixLoc4, 1, GL_FALSE, &mvpMatrixS[0][0]);

	glm::vec4 light = glm::vec4(0.0, 50.0, 100.0, 1.0);             //Light's position
    glm::vec4 lightEye = view * light;     //Light position in eye coordinates
	glUniform4fv(lgtLoc, 1, &lightEye[0]);

	GLfloat vert[30] = {0};   //x,y,z coords of 10 points
	for(int i = 0; i < 5; i++)
	{
		vert[3*i] = -(float)i*0.2 + 3;
		vert[3*i + 2] = -5*i;
		vert[3*i + 15] = -vert[3*i];
		vert[3*i + 17] = vert[3*i + 2];
	}
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

	GLuint vboID;
    glGenBuffers(1, &vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);      // Vertex position

    //========= Create a texture object ====================
    glGenTextures(1, &renderTex);
    glActiveTexture(GL_TEXTURE0);  // Use texture unit 0
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, 512, 512,0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLuint texLoc = glGetUniformLocation(program, "renderTex");  //This is the sampler2D object name in shader
	glUniform1i(texLoc, 0);

    //========= Create a framebuffer object here ====================


	//===============================================================

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  //use default
	glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
}


void display()  
{
	glm::mat4 identity = glm::mat4(1.0);
	glm::mat4 matrix = glm::rotate(identity, angle*CDR, glm::vec3(0.0, 1.0, 0.0));  //rotation about x for teapot
	matrix = glm::rotate(matrix, -90*CDR, glm::vec3(1.0, 0.0, 0.0));  //rotation about x for teapot
	glm::mat4 mvMatrix = view * matrix;        //Model-view matrix for teapot
	glm::mat4 mvpMatrix = proj * mvMatrix;   //The model-view-projection transformation for teapot
	glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation for teapot

	glUniformMatrix4fv(matrixLoc1, 1, GL_FALSE, &mvMatrix[0][0]);
	glUniformMatrix4fv(matrixLoc2, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(matrixLoc3, 1, GL_TRUE, &invMatrix[0][0]);

	glUniform1i(passLoc, 0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	teapot->render();
	glFlush();
}
	

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (512, 512); 
	glutInitWindowPosition (20, 10);
	glutCreateWindow ("Impostors");
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

   initialise ();

   glutDisplayFunc(display); 
   glutMainLoop();
   return 0;
}