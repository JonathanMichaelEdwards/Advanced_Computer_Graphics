/***************************************************************************************************
 * 
 * FILE: TerrainModel.cpp
 * DATE: Due: 11:55pm, Sunday, 15 August 2021
 * AUTHOR:  Jonathan Edwards
 * 
 * DESCRIPTION:
 *              COSC422 Assessment 1: Computer Graphics (2021);  University of Canterbury
 * --
 * 				The tessellation and geometry shader stages of the OpenGL-4 programmable pipeline 
 * 				have found several applications in modelling and rendering of three dimensional objects. 
 * 				In this assignment, you will use these shader stages to generate a terrain model.
 * 
 * 				The Keys can be found in the report and/or the README 
 * 
 * SOURCES:
 * 			- height maps from lab.
 * 			- mesh models from https://www.textures.com/library
 * 			- JPG to TGA converter https://www.freeconvert.com/jpg-to-tga
 * 
***************************************************************************************************/



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


/* ASCII Keys */
#define QUIT    'q'
#define SPACE   0x20
#define ESC     0x1B


/* Texture defines */
#define    NUM_TEXTURES             4

#define    TEX_ID_HEIGHT_MAP        0
#define    TEX_ID_WATER             1
#define    TEX_ID_GRASS             2
#define    TEX_ID_SNOW              3

typedef enum
{
	MT_COOK,
	MT_RUAPEHU
} Terrain;


// Globals
GLuint program;
GLuint vaoID, texID[NUM_TEXTURES];
GLuint eyeLoc, lightLoc;
GLuint mvpMatrixLoc, mvMatrixLoc, norMatrixLoc;
float CDR = 3.14159265/180.0;     //Conversion from degrees to rad (required in GLM 0.9.6)
float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];       //Element array for 81 quad patches
glm::mat4 proj, view, projView;  //Matrices
glm::vec4 eyePos(0.0, 20.0, 30.0, 40.0);  /* scene view (x, y, z, zoom) */
glm::vec3 lookPos(0.0, 0.0, -60.0);  /* scene view */
float angle = 0.f;
GLuint water_Loc, snow_Loc;
float water_level = 3;
float snow_level = 5;
float density = 0.05;  /* Fog level */
int frame_view, lighting_state, cracking_state, fog_state, sky_color_state = 0;  /* Prg. states */



// Generate vertex and element data for the terrain floor
void 
generateData()
{
	int indx, start;
	// verts array
	for (int i = 0; i < 10; i++)   // 100 vertices on a 10x10 grid
	{ 
		for(int j = 0; j < 10; j++)
		{
			indx = 10*i + j;
			verts[3*indx] = 10*i - 45;		//x
			verts[3*indx+1] = 0;			//y
			verts[3*indx+2] = -10*j;		//z
		}
	}

	// elems array
	for (int i = 0; i < 9; i++)
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


// Loads Textures
void 
loadTextures(Terrain terrain)
{
    glGenTextures(NUM_TEXTURES, texID);  /* Generate texture ID */


	/* Load Height Maps */
    glActiveTexture(GL_TEXTURE0);  /* Texture unit 0 */
    glBindTexture(GL_TEXTURE_2D, texID[TEX_ID_HEIGHT_MAP]);
	
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


	/* Load Water Texture */
    glActiveTexture(GL_TEXTURE1);  /* Texture unit 1 */
    glBindTexture(GL_TEXTURE_2D, texID[TEX_ID_WATER]);

	loadTGA("./src/mesh_models/Water.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	/* Load Grass Texture */
    glActiveTexture(GL_TEXTURE2);  /* Texture unit 2 */
    glBindTexture(GL_TEXTURE_2D, texID[TEX_ID_GRASS]);

	loadTGA("./src/mesh_models/Grass.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	/* Load Snow Texture */
    glActiveTexture(GL_TEXTURE3);  /* Texture unit 3 */
    glBindTexture(GL_TEXTURE_2D, texID[TEX_ID_SNOW]);

	loadTGA("./src/mesh_models/Snow.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


// Change world space view - via spacebar
void 
Frame_View(void)
{
	if (!(frame_view = !frame_view))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Wire view if true
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


// Change sky color - via 'c'
void 
Sky_Color(void)
{
	/* Sky color toggle update */
	if (!(sky_color_state = !sky_color_state))
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);  /* Sky color is gery - Nice for fog */ 
	else 
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  /* Sky color is white - Checks cracking */
}


// Loads a shader file and returns the reference to a shader object
GLuint 
loadShader(GLenum shaderType, string filename)
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
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}


// Initialise the shader program, create and load buffer data
void 
initialise(void)
{
	/* Load Textures */
	loadTextures(MT_COOK);

	/* Load shaders */
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "./src/shaders/Terrain.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "./src/shaders/Terrain.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "./src/shaders/Terrain.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "./src/shaders/Terrain.eval");
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "./src/shaders/Terrain.geom");

	/* Attach shaders */
	program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);
	glAttachShader(program, shaderg);

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


	/* Mapping Uniform elements for shader's use */
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");
	eyeLoc = glGetUniformLocation(program, "eyePos");  
	lightLoc = glGetUniformLocation(program, "light_pos");


	/* Passing Textures to shaders */
	/* Pass Height Map texture to Eval. shader */
	GLuint tex_map_Loc = glGetUniformLocation(program, "heightMap"); 
	glUniform1i(tex_map_Loc, TEX_ID_HEIGHT_MAP);

	/* Pass Water texture to Frag. shader */
	GLuint water_Loc = glGetUniformLocation(program, "_tex_water");
	glUniform1i(water_Loc, TEX_ID_WATER);

	/* Pass Grass texture to Frag. shader */
	GLuint grass_Loc = glGetUniformLocation(program, "_tex_grass");
	glUniform1i(grass_Loc, TEX_ID_GRASS);

	/* Pass Grass texture to Frag. shader */
	GLuint snow_Loc = glGetUniformLocation(program, "_tex_snow");
	glUniform1i(snow_Loc, TEX_ID_SNOW);



	//---------Load buffer data-----------------------
	generateData();

	GLuint vboID[2];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);   // Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);  //Vertex indices

    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);  // Number of Patches specified

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);  /* Sky color - grey */ 
}


// Display function to compute uniform values based on transformation parameters and to draw the scene
void 
display(void)
{
	//Projection and view matrices
	proj = glm::perspective(30.0f*CDR, 1.25f, 20.0f, 1000.0f);  //perspective projection matrix

	/*  lookAt(eye, centre, up)
	 * eye  - (x_rotate, y_height, z_zoom)
	 * centre - (0, 0, cam_pitch)
	 * up - Orientation of the camera
	*/
	glm::mat4 mvMatrix = glm::lookAt(glm::vec3(eyePos.x, eyePos.y, eyePos.z), lookPos, glm::vec3(0.0, 1.0, 0.0)); //view matrix
	glm::mat4 mvpMatrix = proj * mvMatrix; // Product matrix
	glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation
	glm::vec4 light = normalize(glm::vec4(-100, 10, 10, 1.0));  /* normalizing light - reduces intensity */

	/* Mapping Uniform to shaders */
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);
	glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here
	glUniform4fv(eyeLoc, 1, &eyePos[0]);   // map eyePos inside shader
	glUniform4fv(lightLoc, 1, &light[0]);  // map light source inside shader

	/* Update water level texture, Pass water level to Geom. shader */
	glUniform1f(glGetUniformLocation(program, "water_level"), water_level);

	/* Update snow texture, Pass snow level to Geom. shader */
	glUniform1f(glGetUniformLocation(program, "snow_level"), snow_level);

	/* Update fog level, Pass fog level to Geom. shader */
	glUniform1f(glGetUniformLocation(program, "density"), density);

	/* Update lighting normals (i.e. toggle smooth shading), Pass lighting state to Eval. shader */
	glUniform1i(glGetUniformLocation(program, "lighting_state"), lighting_state);

	/* Update Cracking state, Pass cracking state to Cont. shader */
	glUniform1i(glGetUniformLocation(program, "cracking_state"), cracking_state);

	/* Update Fog State, Pass fog state to Cont. shader */
	glUniform1i(glGetUniformLocation(program, "fog_state"), fog_state);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    // if (key == QUIT) exit(EXIT_SUCCESS);
	if (key == ESC) exit(EXIT_SUCCESS);

	/* Zoom */
	if (key == '+' || key == '=') 
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
	if (key == SPACE) Frame_View();

	/* Change Textures */
	if (key == '1') loadTextures(MT_COOK);
	if (key == '2') loadTextures(MT_RUAPEHU);

	/* Water Level */
	if (key == 'a')  // dec.
	{
		water_level -= 0.1;
		if (water_level < 0)
			water_level = 0;
	}
	else if (key == 'q')  // inc.
	{
		water_level += 0.1;
		if (water_level > 8)
			water_level = 8;
	}


	/* Snow Level */
	if (key == 's')  // dec.
	{
		snow_level -= 0.1;
		if (snow_level < 4)
			snow_level = 4;
	}
	else if (key == 'w')  // inc.
	{
		snow_level += 0.1;
		if (snow_level > 10)
			snow_level = 10;
	}

	
	/* Toggle Fog*/
	if (key == 'f') fog_state = !fog_state;

	/* Fog Level */
	if (!fog_state) 
	{
		if (key == 'd')  // dec.
		{
			density -= 0.001;
			if (density <  0.03)
				density =  0.03;
		}
		else if (key == 'e')  // inc.
		{
			density += 0.001;
			if (density >  0.07)
				density =  0.07;
		}
	}


	/* Toggle Lighting Normals - toggle smooth shading */
	if (key == 'l') lighting_state = !lighting_state;

	/* Toggle Cracking*/
	if (key == 'c') cracking_state = !cracking_state;

	/* Toggle Sky color */
	if (key == 'b') Sky_Color();


    glutPostRedisplay();  
}


/********************************************************************************************/


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Terrain Model - jme161");
	glutInitContextVersion (4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

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

