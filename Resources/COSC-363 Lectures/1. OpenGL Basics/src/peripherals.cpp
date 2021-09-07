/** ============================================================================
 *	Author:       Jonathan Edwards
 *	Date:         01/31/2020 (V1)
 *	Description:  peripherals.cpp contains button events to kill the window.
 *  ==========================================================================*/


#include <stdio.h>
#include <GL/glut.h>
#include "peripherals.h"
#include "display.h"


// ASCII Keys
#define QUIT    'q'
#define ESC     0x1B


// int posX, posY, posZ = 0;

/** ============================================================================
 * 							Special key event callback 
 *  ===========================================================================*/
void special(int key, int x, int y)
{


    if (key == GLUT_KEY_END) exit(EXIT_SUCCESS);

    // Rotating camera
    if (key == GLUT_KEY_DOWN) getY(-1);
    if (key == GLUT_KEY_UP) getY(1);

    if (key == GLUT_KEY_LEFT) getX(-1);
    if (key == GLUT_KEY_RIGHT) getX(1);


    glutPostRedisplay();
}


/** ============================================================================
 * 							ASCII key event callback 
 *  ===========================================================================*/
void keyEvents(unsigned char key, int x, int y)
{
    static bool keyCheck_w = true;

    if (key == QUIT) exit(EXIT_SUCCESS);
	else if (key == ESC) exit(EXIT_SUCCESS);
    
    // Adding wire frame view if 'w' pressed
    if (key == 'w') {
        if (keyCheck_w) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if (!keyCheck_w) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        keyCheck_w = !keyCheck_w;
    }

    glutPostRedisplay();
}