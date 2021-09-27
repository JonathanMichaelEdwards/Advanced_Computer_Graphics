//===========================================================
//  COSC422 (2021) Advanced Computer Graphics
//  Mesh subdivision using Charles-Loop Algorithm
//  See Exer12.pdf for details
//============================================================

#define _USE_MATH_DEFINES  
#include <cmath>  
#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <GL/freeglut.h>
using namespace std;


typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;
MyMesh mesh;
float modelScale;
float xc, yc, zc;
float angleX = 0, angleY = 0;


// Gets the min max values along x, y, z for scaling and centering the model in the view frustum
//
void getBoundingBox(float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax)
{
	MyMesh::VertexIter vit = mesh.vertices_begin();
	MyMesh::Point pmin, pmax;

	pmin = pmax = mesh.point(*vit);

	//Iterate over the mesh using a vertex iterator
	for (vit = mesh.vertices_begin() + 1; vit != mesh.vertices_end(); vit++)
	{
		pmin.minimize(mesh.point(*vit));
		pmax.maximize(mesh.point(*vit));
	}
	xmin = pmin[0];  ymin = pmin[1];  zmin = pmin[2];
	xmax = pmax[0];  ymax = pmax[1];  zmax = pmax[2];
}


void initialize()
{
	float xmin, xmax, ymin, ymax, zmin, zmax;

	if (!OpenMesh::IO::read_mesh(mesh, "Hand.off"))
	{
		cerr << "read error\n";
	}

	getBoundingBox(xmin, xmax, ymin, ymax, zmin, zmax);

	if (!mesh.has_face_normals()) mesh.request_face_normals();

	mesh.update_face_normals();

	xc = (xmin + xmax)*0.5;
	yc = (ymin + ymax)*0.5;
	zc = (zmin + zmax)*0.5;
	OpenMesh::Vec3f box = { xmax - xc,  ymax - yc, zmax - zc };
	modelScale = 1.0 / box.max();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20, 1.0, 5, 20);

	cout << "No. of vertices = " << mesh.n_vertices() << endl;
	cout << "No. of faces = " << mesh.n_faces() << endl;
}

void special(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) angleY--;
	else if (key == GLUT_KEY_RIGHT) angleY++;
	else if (key == GLUT_KEY_UP) angleX--;
	else if (key == GLUT_KEY_DOWN) angleX++;
	glutPostRedisplay();
}


void drawObject()
{
	MyMesh::FaceIter fit;
	MyMesh::FaceHandle feh;
	MyMesh::FaceVertexIter fvit;
	MyMesh::Point p;
	MyMesh::Normal norm;

	for (fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	{
		feh = *fit;
		norm = mesh.normal(feh);
		glNormal3fv(norm.data());
	    glBegin(GL_TRIANGLES);
		for (fvit = mesh.fv_iter(feh); fvit.is_valid(); fvit++)
		{
			p = mesh.point(*fvit);
			glVertex3fv(p.data());
		}
		glEnd();
	}
}


void display()
{
	float lpos[4] = { 5.0, 0.0, 10.0, 1.0 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 10.0, 0, 0, 0, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPushMatrix();
	    glColor3f(1, 1, 1);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1., 1.);
		glRotatef(angleX, 1, 0, 0);
		glRotatef(angleY, 0, 1, 0);
		glScalef(modelScale, modelScale, modelScale);
		glTranslatef(-xc, -yc, -zc);
		drawObject();
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();
		glColor3f(0, 0, 1);
		glRotatef(angleX, 1, 0, 0);
		glRotatef(angleY, 0, 1, 0);
		glScalef(modelScale, modelScale, modelScale);
		glTranslatef(-xc, -yc, -zc);
		drawObject();
	glPopMatrix();
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB |GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Loop Subdivision");
	initialize();

	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutMainLoop();
	return 0;
}

