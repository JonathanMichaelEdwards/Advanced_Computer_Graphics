// ============================================================================
// 	Author:       Jonathan Edwards
//
// 	Description:  ....
// ============================================================================


#include "display.h"
#include "peripherals.h"



// Window options
#define 	WIDTH 		1000
#define 	HEIGHT 		1000
#define 	WINDOW_POS 	10



const float grey[4] = {0.2, 0.2, 0.2, 1.0};
const float orange[4]  = {1, 0.5, 0, 1};
const float white[4]  = {1, 1, 1, 1};
float mat[4] = { 1.0, 0.75, 0.5, 1.0 };


int period = 0;       //Animation duration in ticks.//
int tick = 0;    //current tick//
const int time_step = 50;/////////////////////////////////////// Movement 



/** ------------------------------------------------------------------------------
//  								Initialize OpenGL
// 
//  @param pjtPath - absolute path directory
//  --------------------------------------------------------------------------- */
void initialize(char *pjtPath, char *OS)
{
    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);  // Background colour
	
	// Load and enable Textures
	loadTexture();
	loadMeshFile("./models/Cannon.off");	


	//---- Load model and textures -- anumation ------
	char *file_name = { 0 };

	// sprintf(file_name, "%s/%s", MODEL_PATH_1, MODEL_1);  // formating file path
	// loadModel(file_name, 0);  // Model - mantalpice
	
	// sprintf(file_name, "%s/%s", MODEL_PATH_2, MODEL_2);  // formating file path
	
	// if (!anim_scene) 
	// 	period = 
	// else 
	// 	period = 
	
	period = loadModel("./models/Mov/Dance.bvh", aiProcess_Debone);  // Movement

	// loadGLTextures_devil();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_SMOOTH);

	// Enable light sources
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	// glEnable(GL_LIGHT3);

	//	Define light's ambient, diffuse, specular properties
	glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, orange);
	glLightfv(GL_LIGHT0, GL_SPECULAR, orange);

	// spotlight LIGHT_1
	glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, orange);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40); 
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90); 

	// spotlight LIGHT_2 - Guard tourch
	glLightfv(GL_LIGHT2, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT2, GL_SPECULAR, white);

	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30); 
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 90); 

	// //	Define light's ambient, diffuse, specular properties
	// glLightfv(GL_LIGHT3, GL_AMBIENT, grey);
	// glLightfv(GL_LIGHT3, GL_DIFFUSE, orange);
	// glLightfv(GL_LIGHT3, GL_SPECULAR, orange);


	glEnable(GL_COLOR_MATERIAL);
	// glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);  
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 
  
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
	// glMaterialfv(GL_FRONT, GL_SPECULAR, white);    
  	// glMaterialf(GL_FRONT, GL_SHININESS, 50);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);    
  	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);
	

	glMatrixMode(GL_PROJECTION);
	gluPerspective(60., 1., 1., 500.); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
		
	// getPath(pjtPath, atoi(OS));   // Error link
} 


/** ------------------------------------------------------------------------------
// 						Perform background processing tasks
// 
//  @param delay  - delay in milli seconds
//  --------------------------------------------------------------------------- */
void idle(int delay)
{
	glutPostRedisplay();
	glutTimerFunc(delay, idle, delay);
}



//----Timer callback for continuous rotation of the model about y-axis----
void update(int value)
{
	if (tick > period) tick = 0;
	updateNodeMatrices(tick);
	glutTimerFunc(time_step, update, 0);
	tick++;
	glutPostRedisplay();
}


// ----------------------------------------------------------------------------
//         		 Initialize glut window and register call backs
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
   	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT); 
	// glutInitWindowPosition(WINDOW_POS, WINDOW_POS);
	glutCreateWindow("Skeleton Makeover    FPS: ...");

    if (argv[1] == NULL) argv[1] = "0";
	initialize(argv[0], argv[1]);

	// Events
	glutDisplayFunc(display);
	glutKeyboardFunc(keyEvents);
	glutSpecialFunc(special);

	// Timers
	glutTimerFunc(time_step, update, 0);
	glutTimerFunc(TIMER_DELAY, idle, TIMER_DELAY);  // fps counter
	glutTimerFunc(20, ballBounce, 0);    // ball physics
	glutTimerFunc(20, animateDoor, 0);   // animate door ball
	glutTimerFunc(10, cannonBall, 0);    // animate cannon ball

	if (wallHit) // animate the guard
		glutTimerFunc(10, guardAnimation, 0); 
	else
		glutTimerFunc(20, guardAnimation, 0); 
	

	glutMainLoop();
	aiReleaseImport(get_scene());
	

	return EXIT_SUCCESS;
}

