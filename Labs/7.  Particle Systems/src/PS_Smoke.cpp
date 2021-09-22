//  ============================================================================
//  COSC422: Advanced Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: Smoke
//   Demo program to demonstrate the construction of particle systems.
//  =============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include "Shader.h"
#include "loadTGA.h"
using namespace std;

#define 	NUM_TEX_ID 		4  /* Num of different textures */


const int NPART = 1000;    //number of particles
GLuint vaoID, texID[NUM_TEX_ID];   
GLuint theProgram;
GLuint timeLoc;
float simTime = 0;   //simulation time
float CDR=3.14159265/180.0;

GLuint texIndex = 0;
GLuint program;
GLuint texLoc;//, texLoc1, texLoc2, texLoc3, texLoc4;



void loadTextures()
{
    glGenTextures(NUM_TEX_ID, texID);  /* Generate 4 texture IDs */

	/* Smoke Tex unit 0 */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    loadTGA("./models/Smoke1.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	/* Smoke Tex unit 1 */
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    loadTGA("./models/Smoke2.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	/* Smoke Tex unit 2 */
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texID[2]);
    loadTGA("./models/Smoke3.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	/* Smoke Tex unit 3 */
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texID[3]);
    loadTGA("./models/Smoke4.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


void initialise()
{
	loadTextures();
	program = createShaderProg("./src/shaders/PS_Smoke.vert", "./src/shaders/PS_Smoke.frag");
	GLuint matrixLoc = glGetUniformLocation(program, "mvpMatrix");
	timeLoc = glGetUniformLocation(program, "simt");
	glUniform1f(timeLoc, 0);


	int tex_index[4] = { 0, 1, 2, 3 };

	// /*
	//  * Uniform smoke texture varibles 
	//  * - Generate 4 Uniform varibles
	//  */ 
	// texLoc1 = glGetUniformLocation(program, "smokeTex1");
	// glUniform1i(texLoc1, 0);

	// texLoc2 = glGetUniformLocation(program, "smokeTex2");
	// glUniform1i(texLoc2, 1);

	// texLoc3 = glGetUniformLocation(program, "smokeTex3");
	// glUniform1i(texLoc3, 2);

	// texLoc4 = glGetUniformLocation(program, "smokeTex4");
	// glUniform1i(texLoc4, 3);
	texLoc = glGetUniformLocation(program, "smokeTex");
	glUniform1iv(texLoc, 4, tex_index);

// ...v sending vector
// glUniform1iv( location of var., num., value);


	glm::mat4 proj = glm::ortho(-5.0f, 5.0f, -1.0f, 9.0f);  //2D orthographic projection matrix
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &proj[0][0]);

	float vert[NPART], vel[NPART], startTime[NPART], angle[NPART], mag[NPART], omeg[NPART];
	int texindx[NPART];

	//Particle path parameters to be stored in 6 VBOs
	float t = 0;
	for(int i = 0; i < NPART; i ++)
	{
		texindx[i] = (int)glm::linearRand(0.0f, 4.0f);  // tex index (range 0 to 3
		vert[i] = glm::linearRand(-1.0f, 1.0f);
		vel[i] = glm::linearRand(1.0f, 3.f);
		angle[i] = glm::linearRand(-10.0f, 10.0f);
		mag[i] = glm::linearRand(0.1f, 1.0f);
		omeg[i] = glm::linearRand(0.2f, 1.0f);
		startTime[i] = t;
		t += 0.1;
	}
	
	/* 6 VBOs + index */
	GLuint vboID[7];

	glGenVertexArrays(1, &vaoID);

    glBindVertexArray(vaoID);
    // glGenBuffers(6, vboID);
	glGenBuffers(7, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);      // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vel), vel, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);      // Velocity

    glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(startTime), startTime, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);      // Starttime

	glBindBuffer(GL_ARRAY_BUFFER, vboID[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(angle),angle, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);      // angle

	glBindBuffer(GL_ARRAY_BUFFER, vboID[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mag), mag, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);      // mag

	glBindBuffer(GL_ARRAY_BUFFER, vboID[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(omeg), omeg, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(5);      // omeg



	// texture indecies - vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texindx), texindx, GL_STATIC_DRAW);
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(6);      // index





    glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);   //++++++++++++++++++++++
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1, 1, 1, 1);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);  //Important!
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
}


void display()
{
	glUniform1f(timeLoc, simTime);

	//glUniform1f(glGetUniformLocation(program, "texIndex"), texIndex);
	// texLoc4 = glGetUniformLocation(program, "smokeTex");
	// if (texIndex < 100)
	// 	glUniform1i(texLoc, 0);  /* Get Smoke Tex ID 0 */
	// else {
	// 	glUniform1i(texLoc, 3);  /* Get Smoke Tex ID 3 */
	// 	if (texIndex > 200)
	// 		texIndex = 0;
	// }

	// texIndex++;
	//printf("%d\n", texIndex);
	


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_POINTS, 0, NPART);
	glutSwapBuffers();
	glFlush();
}


void update(int value)
{
	simTime += 0.05;
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Smoke sprites");
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

