#include "Teapot.h"
#include <cstdlib>  //Required for NULL
#include <fstream>
#include <GL/glew.h>
using namespace std;

bool Teapot:: read_flag = false;

Teapot::Teapot(float size)
{
	if(!read_flag)      //read data only once
	{
		ifstream ifile;
		//Read teapot data
		ifile.open("Teapot.dat", ios::in);

		ifile >> numVert >> numNorm >> numTri;	//Number of vertices, number of normals, number of triangles
												//Note:  Number of vertices must be equal to number of normals

		for(int i = 0;  i < numVert; i++)
			ifile >> vert[3*i] >> vert[3*i+1] >> vert[3*i+2];

		for(int i = 0;  i < numNorm; i++)
			ifile >> norm[3*i] >> norm[3*i+1] >> norm[3*i+2];

		for(int i = 0;  i < numTri; i++)
			ifile >> elem[3*i] >> elem[3*i+1] >> elem[3*i+2];

		ifile.close();

		for(int i = 0;  i < 3*numVert; i++) vert[i] *= size;   //scale teapot
		read_flag = true;
	}

	//Generate buffers
    glGenVertexArrays( 1, &vaoID);
    glBindVertexArray(vaoID);
	GLuint vboID[3];
    glGenBuffers(3, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norm), norm, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);  // Vertex normal

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elem), elem, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Teapot::render() const
{
    glBindVertexArray(vaoID);
    glDrawElements(GL_TRIANGLES, 3*numTri, GL_UNSIGNED_INT, NULL);
}

