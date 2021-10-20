// ============================================================================
// 	Author:       Jonathan Edwards
//
// 	Description:  ....
// ============================================================================


// Standard libaries
#include <stdio.h>

// OpenGL libaries
#include <GL/glut.h>

// Personal libaries
#include "peripherals.h"
#include "display.h"


// ASCII Keys
#define QUIT    'q'
#define SPACE   0x20
#define ESC     0x1B


// For the fps counter
/** ------------------------------------------------------------------------------
// 						Perform background processing tasks
//  @param delay - Delay in milli seconds
//  --------------------------------------------------------------------------- */
static void idle(int delay)
{
	glutTimerFunc(delay, idle, delay);
}


static int viewState = 0;
float angle_cam=0;
GLdouble angle_cam_top=0; 



// ----------------------------------------------------------------------------
//							Special key event callback 
// ----------------------------------------------------------------------------
void special(int key, int x, int y)
{
	if (key == GLUT_KEY_END) exit(EXIT_SUCCESS);

	if (viewState) {
		if(key == GLUT_KEY_LEFT) topBottomLeft();       //angle_cam_top -= 0.1;  //Change direction
		else if(key == GLUT_KEY_RIGHT) topBottomRight(); //angle_cam_top += 0.1;
		if(key == GLUT_KEY_DOWN)
			topBottomBack();
		else if(key == GLUT_KEY_UP)
			topBottomForward();
		
		getView(viewState);  // get view info
	} 
	else if (!viewState) {
		if(key == GLUT_KEY_LEFT) angle_cam -= 0.1;  //Change direction
		else if(key == GLUT_KEY_RIGHT) angle_cam += 0.1;
		else if(key == GLUT_KEY_DOWN)
		{  //Move backward
			moveBack(angle_cam);
		}
		else if(key == GLUT_KEY_UP)
		{ //Move forward
			// position the person it from the centre
			moveForward(angle_cam);
		}
		lookRotation(angle_cam, viewState);
	}
	
    glutTimerFunc(TIMER_DELAY, idle, TIMER_DELAY);
}



bool model_enhan = false;

// ----------------------------------------------------------------------------
//							ASCII key event callback 
// ----------------------------------------------------------------------------
void keyEvents(unsigned char key, int x, int y)
{
	// exit program
    if (key == QUIT) exit(EXIT_SUCCESS);
	else if (key == ESC) exit(EXIT_SUCCESS);

	// signal to change view
	if (key == 'v' || key == 'V') {
		viewState++;
		lookRotation(angle_cam, viewState);  // 3d person view
		if (viewState == 2) {
			viewState = 0;
			getView(viewState);  // top down view
		}
	} 

	// when top down view - allow zoom
	if (viewState == 1) {
		if (key == '+') zoom(1);
		else if (key == '-') zoom(-1);
	}
	
	if (key == SPACE) spacePressed(true);
	if (key == 'd' || key == 'D') doorAction(true);

	if (key == 'E' || key == 'e')
		get_model_en(model_enhan=!model_enhan);

    glutTimerFunc(TIMER_DELAY, idle, TIMER_DELAY);  
}
