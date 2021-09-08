//  ========================================================================
//  COSC422: Advanced Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: TeapotNPR.cpp
//
//	The program loads and displays the mesh data for a teapot.
//  The vertex shader includes code for both transformations and lighting.
//
//   See Exer10.pdf for details
//  ========================================================================
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "loadTGA.h"
using namespace std;

GLuint matrixLoc1, matrixLoc2, matrixLoc3, lgtLoc, flagLoc;
int angle = 0;
ifstream ifile;
unsigned int vaoID;
int numTri;
float CDR = 3.14159265/180.0;  //Conversion from degrees to radians


//Load 3 pencil stroke textures  (*not used*)
void loadTextures()
{
	char* filename[3] = {"./models/PENCIL0.tga", "./models/PENCIL1.tga", "./models/PENCIL2.tga"};
	GLuint texID[3];
    glGenTextures(3, texID);

    for(int i = 0; i < 3; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);  //Texture unit
		glBindTexture(GL_TEXTURE_2D, texID[i]);
		loadTGA(filename[i]);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    glGenerateMipmap(GL_TEXTURE_2D);
	}
}


void initialise()
{
	int numVert, numNorm;
	GLuint program = createShaderProg("./src/shaders/TeapotNPR.vert", "./src/shaders/TeapotNPR.frag");
	matrixLoc1 = glGetUniformLocation(program, "mvMatrix");
	matrixLoc2 = glGetUniformLocation(program, "mvpMatrix");
	matrixLoc3 = glGetUniformLocation(program, "norMatrix");
	lgtLoc = glGetUniformLocation(program, "lightPos");
	flagLoc = glGetUniformLocation(program, "flag");
	GLuint texLoc = glGetUniformLocation(program, "pencilStroke");  //uniform variables of type Sampler2D in fragment shader;
	int texVals[3] = {0, 1, 2};
	glUniform1iv(texLoc, 3, texVals);

	loadTextures();

	//Read teapot data
	ifile.open("./models/Teapot_low.dat", ios::in);

	ifile >> numVert >> numTri ;	//Number of vertices, number of triangles
											//Note:  Number of vertices must be equal to number of normals
    float * vert = new float[ numVert * 3 ];
	float * norm = new float[ numVert * 3 ];
    unsigned int * elem = new unsigned int[numTri * 3];

	for(int i = 0;  i < numVert; i++)   //Read vertex coords
		ifile >> vert[3*i] >> vert[3*i+1] >> vert[3*i+2];

	int n;
	for(int i = 0;  i < numTri; i++)
		ifile >> n >> elem[3*i] >> elem[3*i+1] >> elem[3*i+2];

	for(int i = 0;  i < numVert; i++)   //Read normal coords
		ifile >> norm[3*i] >> norm[3*i+1] >> norm[3*i+2];

	ifile.close();
	
	float * varray = new float[numTri * 9];   //Model definition using vertex coords instead of indices (elements)
	float * narray = new float[numTri * 9];   //to faciliteate the definition of texture coordinates at vertices

	int k;   //temp index
	int indx = 0;
	for(int i = 0; i < numTri; i++)     //For each triangle
	{
		for(int j = 0; j < 3; j++)
		{
			k = 3 * elem[3*i + j];      //get indices of vertices
			for(int ncoord = 0; ncoord < 3; ncoord++)
			{
				varray[indx+ncoord] = vert[k+ncoord];
				narray[indx+ncoord] = norm[k+ncoord];
			}
			indx += 3;
		}
	}

	//Generate buffers
    glGenVertexArrays( 1, &vaoID);
    glBindVertexArray(vaoID);

	GLuint vboID[2];
    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, 9 * numTri * sizeof(float), varray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER, 9 * numTri * sizeof(float), narray, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);  // Vertex normal

    delete [] vert;
    delete [] norm;
    delete [] elem;
	delete [] varray;
	delete [] narray;
    glBindVertexArray(0);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	glLineWidth(4.0);
}

//Timer call back function for continuous rotation of the teapot
void update(int value)
{
	angle++;
	if(angle == 360) angle = 0;
	glutTimerFunc(50.0, update, 0);
	glutPostRedisplay();
}

//The main display function
void display()
{
	float cam_x = 50*sin(angle*CDR);   //camera position
	float cam_z = 50*cos(angle*CDR);
	glm::vec4 light = glm::vec4(30.0, 20.0, 30.0, 1.0);
	glm::mat4 proj = glm::perspective(10.0f*CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	glm::mat4 view = glm::lookAt(glm::vec3(cam_x, 20.0, cam_z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
	glm::mat4 mvpMatrix = proj * view;    //The model-view-projection matrix
	glm::vec4 lightEye = view * light;     //Light position in eye coordinates
	glm::mat4 invMatrix = glm::inverse(view);  //Inverse of model-view matrix for normal transformation
	glUniformMatrix4fv(matrixLoc1, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrixLoc2, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(matrixLoc3, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here
	glUniform4fv(lgtLoc, 1, &lightEye[0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vaoID);
	glUniform1i(flagLoc, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3*numTri);

	glFlush();
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("A Teapot");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit())
	{
		cerr << "Unable to initialize GLEW  ...exiting" << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutTimerFunc(50, update, 0); 
	glutMainLoop();
}

