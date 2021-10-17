/***************************************************************************************************
 * 
 * FILE: TerrainModel.cpp
 * DATE: Due: 11:55pm, Monday, 27 Sept 2021
 * AUTHOR:  Jonathan Edwards
 * 
 * DESCRIPTION:
 *              COSC422 Assessment 2: Computer Graphics (2021);  University of Canterbury
 * 
 * 				The Keys can be found in the report and/or the README 
 * 
 * SOURCES:
 * 			- textures & models from lab and lectures.
 * 
***************************************************************************************************/

#define _USE_MATH_DEFINES // for C++  
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>  
#include <thread>
#include <mutex>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <IL/il.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>
using namespace std;


typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
MyMesh mesh;
float modelScale;
float xc, yc, zc;
float rotn_x = 0.0, rotn_y = 0.0;
GLuint vaoID;
GLuint mvpMatrixLoc, mvMatrixLoc, norMatrixLoc, lgtLoc, wireLoc, renderer_loc, pencil_blending_loc;
glm::mat4 view, projView;
int num_Elems;
GLuint program;

OpenMesh::Vec3f box;
float xmin, xmax, ymin, ymax, zmin, zmax;


/* Keystroke states */
bool wireframe = false;
bool textures = false;
bool texture_weighting = false;
bool renderer = false;

float thickness_silhouette = 0.5;

/* ASCII Keys - https://www.techonthenet.com/ascii/chart.php */
#define 	SPACE   	0x20
#define 	ESC     	0x1B
#define 	PG_UP     	0x1B
#define 	PG_DN     	0x1B

/* Texture defines */
#define    NUM_TEXTURES             5



/// Textures -----------------------
GLuint texId[NUM_TEXTURES];

void loadGLTextures()
{
	ilInit();
	ilGenImages(NUM_TEXTURES, texId);				// Create an image id similar to glGenTextures()
	// ilBindImage(texId);					// Binding of DevIL image name
	ilEnable(IL_ORIGIN_SET);				// Enable change of origin of referenence
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);		// This is necessay since the default location is upper-left

	ilBindImage(texId[0]);
	if (ilLoadImage("./textures/Pencil_stroke_1.png"))			//if success
	{
		int imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		// Pencil 1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		cout << "Texture successfully loaded. Width = " << imgWidth << " Height = " << imgHeight <<endl;
	}
	else
	{
		cout << "Couldn't load texture. " <<  endl;
	}

	//ilBindImage(texId[1]);
	if (ilLoadImage("./textures/Pencil_stroke_2.png"))			//if success
	{
		int imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		// Pencil 1
		//glGenTextures(2, texId);
		glActiveTexture(GL_TEXTURE1); 
		glBindTexture(GL_TEXTURE_2D, texId[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		cout << "Texture successfully loaded. Width = " << imgWidth << " Height = " << imgHeight <<endl;
	}
	else
	{
		cout << "Couldn't load texture. " <<  endl;
	}

	//ilBindImage(texId[2]);
	if (ilLoadImage("./textures/Pencil_stroke_3.png"))			//if success
	{
		int imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		// Pencil 1
		//glGenTextures(2, texId);
		glActiveTexture(GL_TEXTURE2); 
		glBindTexture(GL_TEXTURE_2D, texId[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		cout << "Texture successfully loaded. Width = " << imgWidth << " Height = " << imgHeight <<endl;
	}
	else
	{
		cout << "Couldn't load texture. " <<  endl;
	}

	//ilBindImage(texId[3]);
	if (ilLoadImage("./textures/Pencil_stroke_4.png"))			//if success
	{
		int imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		// Pencil 1
		//glGenTextures(2, texId);
		glActiveTexture(GL_TEXTURE3); 
		glBindTexture(GL_TEXTURE_2D, texId[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		cout << "Texture successfully loaded. Width = " << imgWidth << " Height = " << imgHeight <<endl;
	}
	else
	{
		cout << "Couldn't load texture. " <<  endl;
	}

	//ilBindImage(texId[4]);
	if (ilLoadImage("./textures/Pencil_stroke_5.png"))			//if success
	{
		int imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		// Pencil 1
		//glGenTextures(2, texId);
		glActiveTexture(GL_TEXTURE4); 
		glBindTexture(GL_TEXTURE_2D, texId[4]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		cout << "Texture successfully loaded. Width = " << imgWidth << " Height = " << imgHeight <<endl;
	}
	else
	{
		cout << "Couldn't load texture. " <<  endl;
	}
}





//Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if (!shaderFile.good()) cout << "Error opening shader file." << endl;
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
		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char* strShaderType = NULL;
		cerr << "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}




// Gets the min max values along x, y, z for scaling and centering the model in the view frustum
void getBoundingBox(float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax)
{
	MyMesh::VertexIter vit = mesh.vertices_begin();
	MyMesh::Point pmin, pmax;

	pmin = pmax = mesh.point(*vit);

	//Iterate over the mesh using a vertex iterator
	for (vit = mesh.vertices_begin()+1; vit != mesh.vertices_end(); vit++)
	{
		pmin.minimize(mesh.point(*vit));
		pmax.maximize(mesh.point(*vit));
	}
	xmin = pmin[0];  ymin = pmin[1];  zmin = pmin[2];
	xmax = pmax[0];  ymax = pmax[1];  zmax = pmax[2];
}




// Initialisation function for OpenMesh, shaders and OpenGL
void initialize()
{
	float CDR = M_PI / 180.0f;

	//============= Load mesh ==================
	if (!OpenMesh::IO::read_mesh(mesh, "./models/Camel.off"))
	{
		cerr << "Mesh file read error.\n";
	}
	getBoundingBox(xmin, xmax, ymin, ymax, zmin, zmax);

	/* texture coordinates */
	xc = (xmin + xmax)*0.5f;
	yc = (ymin + ymax)*0.5f;
	zc = (zmin + zmax)*0.5f;
	box = { xmax - xc,  ymax - yc, zmax - zc };
	modelScale = 1.0 / box.max();


	/* Load textures */
	loadGLTextures();


	//============= Load shaders ==================
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "./src/shaders/NPR.vert");
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "./src/shaders/NPR.geom");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "./src/shaders/NPR.frag");
	
	/* Attach shaders */
	program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderg);
	glAttachShader(program, shaderf);
	glLinkProgram(program);


	//============= Create program object ============
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	//==============Get vertex, normal data from mesh=========
	int num_verts = mesh.n_vertices();
	int num_faces = mesh.n_faces();
	float* vertPos = new float[num_verts * 3];
	float* vertNorm = new float[num_verts * 3];
	num_Elems = num_faces * 6; 

	short* elems = new short[num_Elems];   // Asumption: Triangle mesh
	printf("Triangle mesh num. = %d\n", num_Elems);

	if (!mesh.has_vertex_normals())
	{
		mesh.request_face_normals();
		mesh.request_vertex_normals();
		mesh.update_normals();
	}

	MyMesh::VertexIter vit;  // A vertex iterator
	MyMesh::FaceIter fit;    // A face iterator
	MyMesh::FaceVertexIter fvit; // Face-vertex iterator
	OpenMesh::VertexHandle verH1, verH2;
	OpenMesh::FaceHandle facH;
	MyMesh::Point pos;
	MyMesh::Normal norm;
	int indx;

	// Use a vertex iterator to get vertex positions and vertex normal vectors
	indx = 0;
	for (vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
	{
		verH1 = *vit;				// Vertex handle
		pos = mesh.point(verH1);
		norm = mesh.normal(verH1);
		for (int j = 0; j < 3; j++)
		{
			vertPos[indx] = pos[j];
			vertNorm[indx] = norm[j];
			indx++;
		}
	}

	// GL_TRIANGLE_ADJACENCY - requires 6 verticies per primative
	// 1.    Use a face iterator to get each triangle of the mesh

	// 2.    For the current triangle, use a face-halfedge iterator to find its three half edges

	// 3.    For each halfedge, find the from vertex using from_vertex_handle,  and the vertex on the 
	//			opposite face using opposite_he_opposite_vh  function.  Please refer to slides 5, 6 of “Assignment2.pdf”  
	//			in the assignment section on Learn.

	// 4.    Store the indices of the vertices (2 vertices per halfedge)  in an array

	// Use a face iterator to get the vertex indices for each face /////////////////////////////////// Generate the element array for triangle adjacency
	indx = 0;
	for (fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	{
		facH = *fit;

		OpenMesh::HalfedgeHandle heH = mesh.halfedge_handle(facH); // one of the edges of the current tri.

		verH2 = mesh.from_vertex_handle(heH);  // Vertex A
		elems[indx] = verH2.idx();
		indx++;

		verH2 = mesh.opposite_he_opposite_vh(heH);    // Vertex D1
		elems[indx] = verH2.idx();
		indx++;

		heH = mesh.next_halfedge_handle(heH); 

		verH2 = mesh.from_vertex_handle(heH);  // Vertex B
		elems[indx] = verH2.idx();
		indx++;

		verH2 = mesh.opposite_he_opposite_vh(heH);    // Vertex D2
		elems[indx] = verH2.idx();
		indx++;

		heH = mesh.next_halfedge_handle(heH); 

		verH2 = mesh.from_vertex_handle(heH);  // Vertex C
		elems[indx] = verH2.idx();
		indx++;

		verH2 = mesh.opposite_he_opposite_vh(heH);    // Vertex D3
		elems[indx] = verH2.idx();
		indx++;
	}  // GL_TRIANGLES_ADJACENCY - V3



	// // // GL_TRIANGLES - V2 ---------------------------
	// indx = 0;
	// for (fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	// {
	// 	facH = *fit;
	// 	OpenMesh::HalfedgeHandle heH = mesh.halfedge_handle(facH);


	// 	verH2 = mesh.from_vertex_handle(heH);  // Vertex A
	// 	elems[indx] = verH2.idx();
	// 	indx++;

	// 	verH2 = mesh.to_vertex_handle(heH);    // Vertex B
	// 	elems[indx] = verH2.idx();
	// 	indx++;

	// 	verH2 = mesh.opposite_vh(heH);         // Vertex C
	// 	elems[indx] = verH2.idx();
	// 	indx++;
	// }




	// Original - V1 ---------------------------
	// indx = 0;
	// for (fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	// {
	// 	facH = *fit;
	// 	for (fvit = mesh.fv_iter(facH); fvit.is_valid(); fvit++)
	// 	{
	// 		verH2 = *fvit;				 // Vertex handle
	// 		elems[indx] = verH2.idx();
	// 		indx++;
	// 	}
	// }

 
	mesh.release_vertex_normals();




	//============== Load buffer data ==============
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	GLuint vboID[3];
	glGenBuffers(3, vboID);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* num_verts * 3, vertPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_verts * 3, vertNorm, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[2]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * num_faces * 3, elems, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * num_faces * 6, elems, GL_STATIC_DRAW);

	glBindVertexArray(0);



	// ============== Create uniform variables ============== -----------------------------------
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");
	wireLoc = glGetUniformLocation(program, "wireMode");
	lgtLoc = glGetUniformLocation(program, "lightPos");


	/* Textures */
	GLuint texLoc = glGetUniformLocation(program, "pencil_stroke");  // uniform variables of type Sampler2D in fragment shader;
	int texVals[5] = { 0, 1, 2, 3, 4 };
	glUniform1iv(texLoc, 5, texVals);

	/* Texture mapping - coordinates */
	GLfloat tex_coord_loc = glGetUniformLocation(program, "tex_min_max");  // uniform variables of type Sampler2D in fragment shader;
	float tex_coord_vals[4] = { xmin, xmax, zmin, zmax };
	glUniform1fv(tex_coord_loc, 4, tex_coord_vals);

	/* Modes */
	renderer_loc = glGetUniformLocation(program, "render_mode");
	pencil_blending_loc = glGetUniformLocation(program, "pencil_blending_mode");
	// -----------------------------------------------------------------------------------------



	glm::vec4 light = normalize(glm::vec4(10.0, 1.0, 50.0, 1.0));   /* normalizing light - reduces intensity */
	glm::mat4 proj;
	proj = glm::perspective(60.0f * CDR, 1.0f, 2.0f, 10.0f);  //perspective projection matrix
	view = glm::lookAt(glm::vec3(0, 0, 4.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix

	projView = proj * view;
	glm::vec4 lightEye = light;//view * light
	glUniform4fv(lgtLoc, 1, &lightEye[0]);



	//============== Initialize OpenGL state ==============
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   
}


// ----------------------------------------------------------------------------
// 				Callback function for special keyboard events
// ----------------------------------------------------------------------------
void special(int key, int x, int y)
{
	/* Rotating around model */
	if (key == GLUT_KEY_LEFT) 
		rotn_y -= 5.0;
	else if (key == GLUT_KEY_RIGHT) 
		rotn_y += 5.0;
	else if (key == GLUT_KEY_UP) 
		rotn_x -= 5.0;
	else if (key == GLUT_KEY_DOWN) 
		rotn_x += 5.0;

	/* Zoom into or out of model - Bunny */
	if (key == GLUT_KEY_PAGE_UP)  // zoom in
		modelScale += 0.1;
	else if (key == GLUT_KEY_PAGE_DOWN)  // zoom out
		modelScale -= 0.1;


	glutPostRedisplay();
}


/**************************************************************************************/


// ----------------------------------------------------------------------------
//							ASCII key event callback 
// ----------------------------------------------------------------------------
void 
keyEvents (unsigned char key, int x, int y)
{
	/* Exit program */
	if (key == ESC) 
		exit(EXIT_SUCCESS);


	/* Toggle render */
	if (key == SPACE) 
		renderer = !renderer;


	/* Toggle Wireframe view */
	if (key == 'f') 
	{
		wireframe = !wireframe;
		if (wireframe) 	
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else 			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // solid if true (start)
	}



	// -- Extra keys ---------------------------------

	/* Silhoutte & Crease edges level */
	if (key == 'a')  // dec.
	{
		thickness_silhouette -= 0.05;
		if (thickness_silhouette < 0.3)
			thickness_silhouette = 0.3;
	}
	else if (key == 'q')  // inc.
	{
		thickness_silhouette += 0.05;
		if (thickness_silhouette > 0.8)
			thickness_silhouette = 0.8;
	}

	/* Toggle texture blending weighting - for pencil shading */
	if (key == 't' && renderer) 
		texture_weighting = !texture_weighting;



    glutPostRedisplay();  
}


/********************************************************************************************/




// The main display callback function
void display()
{
	float CDR = M_PI / 180.0;
	glm::mat4 matrix = glm::mat4(1.0);
	matrix = glm::rotate(matrix, rotn_x * CDR, glm::vec3(1.0, 0.0, 0.0));  // rotation about x
	matrix = glm::rotate(matrix, rotn_y * CDR, glm::vec3(0.0, 1.0, 0.0));  // rotation about y
	matrix = glm::scale(matrix, glm::vec3(modelScale, modelScale, modelScale));  // Zoom
	matrix = glm::translate(matrix, glm::vec3(-xc, -yc, -zc));



	/* -------------------------------- Update Uniforms to shader -------------------------------- */
	glm::mat4 viewMatrix = view * matrix;		//The model-view matrix
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);

	glm::mat4 prodMatrix = projView * matrix;   //The model-view-projection matrix
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

	glm::mat4 invMatrix = glm::inverse(viewMatrix);  //Inverse of model-view matrix
	glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);

	/* frame mode */
	// if (wireframe) 
	// 	glUniform1i(wireLoc, 1);
	// else		   
	// 	glUniform1i(wireLoc, 0);
		
	/* Toggle rendering */
	if (renderer) 
		glUniform1i(renderer_loc, 1);  // Pencil shading
	else		   
		glUniform1i(renderer_loc, 0);  // Two-tone
	
	if (texture_weighting) 
		glUniform1i(pencil_blending_loc, 1);  // On
	else		   
		glUniform1i(pencil_blending_loc, 0);  // OFF

	
	/* Thickness of silhouette edge */
	glUniform1f(glGetUniformLocation(program, "thickness_silhouette"), thickness_silhouette);

	/* -------------------------------------------------------------------------------------------- */

	/* Refresh */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES_ADJACENCY, num_Elems, GL_UNSIGNED_SHORT, NULL);  // GL_TRIANGLES -> GL_TRIANGLES_ADJACENCY (triangle adjacency)
	//glDrawElements(GL_TRIANGLES, num_Elems, GL_UNSIGNED_SHORT, NULL);
	//GL_TRIANGLES_ADJACENCY - requires 6 verticies per primative

	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB |GLUT_DEPTH);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Mesh Viewer (OpenMesh)");
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	if (glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialize();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyEvents);
	glutMainLoop();
	return 0;
}

