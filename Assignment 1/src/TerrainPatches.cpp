//  ========================================================================
//  COSC363: Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: TerrainPatches.cpp
//  See Lab-11.pdf for details.
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  Requires files  TerrainPatches.vert, TerrainPatches.frag
//                  TerrainPatches.cont, TerrainPatches.eval
//  ========================================================================
#define  GLM_FORCE_RADIANS
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadTGA.h"
using namespace std;


// ASCII Keys
#define QUIT    'q'
#define SPACE   0x20
#define ESC     0x1B


typedef enum
{
	MT_COOK,
	MT_RUAPEHU
} Terrain;


// Globals
GLuint vaoID;
GLuint eyeLoc, levelLoc;
GLuint mvpMatrixLoc;
float CDR = 3.14159265/180.0;     //Conversion from degrees to rad (required in GLM 0.9.6)
float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];       //Element array for 81 quad patches
glm::mat4 projView;
glm::vec4 eyePos(0.0, 20.0, 30.0, 40.0);  /* scene view (x, y, z, zoom) */
glm::vec3 lookPos(0.0, 0.0, -60.0);  /* scene view */
float angle = 0.f;
bool view = false;



//Generate vertex and element data for the terrain floor
void generateData()
{
	int indx, start;
	//verts array
	for(int i = 0; i < 10; i++)   //100 vertices on a 10x10 grid
	{
		for(int j = 0; j < 10; j++)
		{
			indx = 10*i + j;
			verts[3*indx] = 10*i - 45;		//x
			verts[3*indx+1] = 0;			//y
			verts[3*indx+2] = -10*j;		//z
		}
	}

	//elems array
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			indx = 9*i +j;
			start = 10*i + j;
			elems[4*indx] = start;
			elems[4*indx+1] = start+10;
			elems[4*indx+2] = start+11;
			elems[4*indx+3] = start+1;			
		}
	}
}

// Loads terrain texture
void 
loadTextures(Terrain terrain)
{
	GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
	
	// Load either MtCook.tga or MtRuapehu.tga
	switch (terrain) 
	{
		case MT_COOK:
			loadTGA("./src/height_maps/MtCook.tga");
			break;
		case MT_RUAPEHU:
			loadTGA("./src/height_maps/MtRuapehu.tga");
			break;
	}

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


void 
Load_View(void)
{
	if (!(view=!view))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wire view if true
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



//Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

//Initialise the shader program, create and load buffer data
void initialise()
{
	// glm::mat4 proj, view;   //Projection and view matrices

	//--------Load terrain height map-----------
	loadTextures(MT_COOK);

	//--------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "./src/shaders/Terrain.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "./src/shaders/Terrain.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "./src/shaders/Terrain.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "./src/shaders/Terrain.eval");
	// GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "./src/shaders/Terrain.geom");

	//--------Attach shaders---------------------
	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);
	// glAttachShader(program, shaderg);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	GLuint texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);
	eyeLoc = glGetUniformLocation(program, "eyePos");
	glUniform1i(eyeLoc, 0);


	//---------Load buffer data-----------------------
	generateData();

	GLuint vboID[2];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);  // Number of Patches specified

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
	glm::mat4 proj, view;   //Projection and view matrices
	proj = glm::perspective(30.0f*CDR, 1.25f, 20.0f, 500.0f);  //perspective projection matrix

	/*  lookAt(eye, centre, up)
	 * eye  - (x_rotate, y_height, z_zoom)
	 * centre - (0, 0, cam_pitch)
	 * up - Orientation of the camera
	*/
	view = glm::lookAt( glm::vec3(eyePos.x, eyePos.y, eyePos.z), lookPos, glm::vec3(0.0, 1.0, 0.0)); //view matrix
	projView = proj * view;  // Product matrix

	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &projView[0][0]);
	glUniform4fv(eyeLoc, 1, &eyePos[0]); // map value inside shader

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 81*4, GL_UNSIGNED_SHORT, NULL);  /* Drawing Patches */
	glFlush();
}


/********************************************************************************************/
// Button Events - For moving around the scene
// 		note: eyePos is a vec3(x, y, z)

// ----------------------------------------------------------------------------
//							Special key event callback 
// ----------------------------------------------------------------------------
void special(int key, int x, int y)
{
	if(key == GLUT_KEY_LEFT) angle -= 0.1;  //Change direction
	else if(key == GLUT_KEY_RIGHT) angle += 0.1;
	else if(key == GLUT_KEY_DOWN)
	{  //Move backward
		eyePos.x -= sin(angle);
		eyePos.z += cos(angle);
	}
	else if(key == GLUT_KEY_UP)
	{ //Move forward
		eyePos.x += sin(angle);
		eyePos.z -= cos(angle);
	}

	lookPos.x = eyePos.x + 90*sin(angle);  // angle the person is looking
	lookPos.z = eyePos.z - 90*cos(angle);

	glutPostRedisplay();
}


/********************************************************************************************/


// ----------------------------------------------------------------------------
//							ASCII key event callback 
// ----------------------------------------------------------------------------
void 
keyEvents(unsigned char key, int x, int y)
{
	/* Exit program */
    if (key == QUIT) exit(EXIT_SUCCESS);
	else if (key == ESC) exit(EXIT_SUCCESS);

	/* Zoom */
	if (key == '+') 
	{
		eyePos.y -= 0.5;
		eyePos.w += 0.01;
	}
	else if (key == '-') 
	{
		eyePos.y += 0.5;
		eyePos.w -= 0.01;
	}

	/* Toggle Wireframe view */
	if (key == SPACE) Load_View();

	/* Change Textures */
	if (key == '1') loadTextures(MT_COOK);
	if (key == '2') loadTextures(MT_RUAPEHU);


    glutPostRedisplay();  
}


/********************************************************************************************/


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Terrain");
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
	glutSpecialFunc(special);
	glutKeyboardFunc(keyEvents);
	glutMainLoop();
}

