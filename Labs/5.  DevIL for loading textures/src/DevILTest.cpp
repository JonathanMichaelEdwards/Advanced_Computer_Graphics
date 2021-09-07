//  ========================================================================
//   COSC422  Advanced Computer Graphics
//   Program to test the loading of textures using DevIL library
//   Input:  Colors.png
//  ========================================================================

#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
#include <IL/il.h>
using namespace std;

GLuint texId;

void loadGLTextures()
{
	ilInit();
	ILuint imageId;
	ilGenImages(1, &imageId);				// Create an image id similar to glGenTextures()
	ilBindImage(imageId);					// Binding of DevIL image name
	ilEnable(IL_ORIGIN_SET);				// Enable change of origin of referenence
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);		//This is necessay since the default location is upper-left

	if (ilLoadImage("./images/Colors.png"))			//if success
	{
		int imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		int imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		//OpenGL Texturing
		glGenTextures(1, &texId); 
		glBindTexture(GL_TEXTURE_2D, texId);
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


//---------------------------------------------------------------------
void initialise(void) 
{
    loadGLTextures();

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glClearColor (1.0, 1.0, 1.0, 1.0);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(30.0, 1.0, 100.0, 5000.0); 
}

//---------------------------------------------------------------------
void display(void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (0, 100, 500, 0, 100, 0, 0, 1, 0); 
	glBindTexture(GL_TEXTURE_2D, texId);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);   //A simple quad
	  glTexCoord2f(0, 0);  glVertex3f(-100, 0, 0);
	  glTexCoord2f(1, 0);  glVertex3f(100, 0., 0);
	  glTexCoord2f(1, 1);  glVertex3f(100, 200., 0);
	  glTexCoord2f(0, 1);  glVertex3f(-100, 200, 0);
	glEnd();

	glFlush();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH );
   glutInitWindowSize (700, 700); 
   glutInitWindowPosition (50, 50);

   glutCreateWindow("DevIL test");

   initialise();
   glutDisplayFunc(display); 
 
   glutMainLoop();
   return 0;
}
