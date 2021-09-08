//  ========================================================================
//  COSC422: Advanced Computer Graphics (2016);  University of Canterbury.
//
//  FILE NAME: GS_SurfRevln.cpp
//  See  Ex05_SurfaceRevln.pdf  for details

//	Demonstrates the use of geometry shader in geometry amplification
//  Files required:  Vase.dat, GeomSR.vert, GeomSR.frag
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

GLuint vao;
float CDR = 3.14159265/180.0;   //Degrees to radians conversion

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

void initialise()
{
	GLuint matrixLoc;

	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "./src/shaders/GeomSR.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "./src/shaders/GeomSR.frag");
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "./src/shaders/GeomSR.geom");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
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

	glm::mat4 proj = glm::perspective(40.0f*CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 10.0, 30.0), glm::vec3(0.0, 5.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 prodMatrix = proj * view;        // Model-view matrix
	matrixLoc = glGetUniformLocation(program, "mvpMatrix");
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

	//Read coordinates of base curve vertices
    ifstream infile;
    infile.open("./src/data/Vase.dat", ios::in);
    float verts[57];
	int nvert;
	infile >> nvert;
	for(int i = 0; i < nvert; i++)
	{
		infile >> verts[3*i] >> verts[3*i + 1];
		verts[3*i + 2] = 0;   // z-coord = 0
	}

	glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINE_STRIP, 0, 19);  // draw 19 strips
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Surface of revolution");
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

