//  ========================================================================
//  COSC422: Advanced Computer Graphics (2014);  University of Canterbury.
//
//  FILE NAME: Draw5.cpp
//
//	Draws a tetrahedron using element array buffer. (OpenGL 4.2 Core Profile).
//  Shows the implementation of model, view and projection transformations.
//  The tetrahedron is displayed as a wireframe object (no lighting), and
//  is animated by continuously rotating it about the y-axis. 
//  Requires shaders:  tetrahedron.vert, tetrahedron.frag
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
GLuint theProgram;
GLuint matrixLoc;
float angle = 0.0;

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
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "Tetrahedron.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "Tetrahedron.frag");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
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
	matrixLoc = glGetUniformLocation(program, "mvpMatrix");

    // A polygonal line forming three triangles
    static const GLfloat verts[] =
    {
        10.0,  10.0,  10.0,
        10.0, -10.0, -10.0,
        -10.0, 10.0, -10.0,
        -10.0, -10.0, 10.0
    };

	static const GLushort vindx[] = {0, 1, 2, 0, 2, 3, 0, 3, 1, 1, 3, 2};

	GLuint ebo, vbo;

	glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vindx), vindx, GL_STATIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void update(int value)
{
	angle++;
	glutTimerFunc(50.0, update, 0);
	glutPostRedisplay();
}

void display()
{
	glm::mat4 proj = glm::perspective(20.0f, 1.0f, 100.0f, 1000.0f);  //perspective projection matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 150.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
	glm::mat4 matrix = glm::mat4(1.0);
	matrix = glm::rotate(matrix, angle, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix
	glm::mat4 prodMatrix = proj*view*matrix;   //The model-view-projection transformation
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("A tetrahedron");
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

