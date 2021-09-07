 /** ============================================================================
 *	Author:       Jonathan Edwards
 *	Date:         02/17/2020 (V1)
 *	Description:  Displays a teapot using glut.
 *  ==========================================================================*/


#include <iostream>
#include <GL/glut.h>
#include "display.h"



/** ============================================================================
 *           				Calculate FPS for Window
 * 
 *  - Code computes the average frames per second (fps), and also the average			
 *    time it takes to render one frame. These stats are appended to the caption 
 *    bar window.
 *  ===========================================================================*/
void showFPS(void)
{
	char title[TITLE_SPACE] = {0};
	static double endTime = 0.0;
	static int frameCount = 0;
	
	// Time difference
	double startTime = glutGet(GLUT_ELAPSED_TIME); 
	double elapsedTime = startTime-endTime;

	// Display FPS every 1/4 a second
	if (elapsedTime > QUATER_SEC) {
		sprintf(title, "Teapot    FPS: %.2f", (frameCount / elapsedTime) * FPS_SEC);
		glutSetWindowTitle(title);

		endTime = startTime;
		frameCount = 0;
	}
	frameCount++;
}


int getY(int y)
{
	static int a = 0;

	if (!y) return a;
	a += y;

	return a;
}


int getX(int x)
{
	static int a = 0;

	if (!x) return a;
	a += x;

	return a;
}

/** ============================================================================
 *  				  Display OpenGL graphics onto the screen					
 *  ===========================================================================*/
void display(void)  
{
	float light_pos[4] = {0., 10., 10., 1.0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Camera and Light position
	gluLookAt(getX(0), getY(0), 12, getX(0), getY(0), 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glColor3f(0.0, 1.0, 1.0);
	glutSolidTeapot(1.0);

	glFlush(); 

	showFPS();
}
