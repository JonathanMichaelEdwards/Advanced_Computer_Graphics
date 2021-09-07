//  ============================================================================
//  COSC422: Advanced Computer Graphics (2021);  University of Canterbury.
//
//   Ex.7:  Sprite Animation
//   See Ex07.pdf for details.
//  =============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <IL/il.h>
#include "Shader.h"
using namespace std;

const int NUMFRAMES = 14;   //Total number of frames on sprite sheet
GLuint vaoID, texID;
int frameX, frameY;       //Frame's position within sprite sheet
int frameWid, frameHgt;   //Frame's width and height
float aspectRatio;

GLuint theProgram;
GLuint  matrixLoc, texLoc, aspectLoc;  //Locations of uniform variables
BYTE *imgData;				//sprite frame image
glm::mat4 projView;


void loadGLTextures()
{
	ilInit();
	ILuint imageID;
	int imgWidth, imgHeight;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);   
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);  

	//Load Sprite Sheet
	if (ilLoadImage("./images/Runner.png"))   
	{
		imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		cout << "Image successfully loaded.  Width = " << imgWidth << ", Height = " << imgHeight << endl;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	//Extract frames from the image and use them as textures
	//Please remove the following four lines...
	frameX = 24;      //>>>These variables must be converted into arrays
	frameY = 200;
	frameWid = 122;
	frameHgt = 180;
	

	glGenTextures(1, &texID);    //>>> Modify to generate NUMFRAMES ids.	

	//Use a for-loop for the frames here...
	//for(int i = 0;  i < NUMFRAMES; i++) {
	glActiveTexture(GL_TEXTURE0);		     //Texture units. Replace with GL_TEXTURE0+i
	glBindTexture(GL_TEXTURE_2D, texID);     
	imgData = new BYTE[frameWid * frameHgt * 3];
	ilCopyPixels(frameX, frameY, 0, frameWid, frameHgt, 1, IL_RGB, IL_UNSIGNED_BYTE, imgData);

	aspectRatio = (float)frameWid / (float)frameHgt;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWid, frameHgt, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	delete[] imgData;
	//End for-loop here

	ilBindImage(0);
	ilDeleteImage(imageID);
}

void initialise()
{
	float CDR = 3.14159265 / 180.0;
	glm::mat4 view, proj;

	loadGLTextures();

	GLuint program = createShaderProg("./src/shaders/SpriteAnim.vert", "./src/shaders/SpriteAnim.frag");

	matrixLoc = glGetUniformLocation(program, "mvpMatrix");   //Matrix without transformation (for trees and road)
	texLoc = glGetUniformLocation(program, "spriteTex");
	aspectLoc  = glGetUniformLocation(program, "aspect");

	glUniform1i(texLoc, 0);
	glUniform1f(aspectLoc, aspectRatio);










	proj = glm::perspective(40.0f*CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	view = glm::lookAt(glm::vec3(0, 0, 30.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
	projView = proj * view; 
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &projView[0][0]);

	GLfloat vert[3] = { 0 };  //a single point at the origin (0, 0, 0)

	GLuint vboID;

	glGenVertexArrays(1, &vaoID);

    glBindVertexArray(vaoID);
    glGenBuffers(1, &vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);      // Vertex position

    glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);  //Important!
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_POINTS, 0, 1);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Sprite Animation");
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

