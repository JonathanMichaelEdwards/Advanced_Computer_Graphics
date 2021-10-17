

// ============================================================================
// 	Author:       Jonathan Edwards
//
// 	Description:  ....
// ============================================================================


// Standard libaries
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>  
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>


// OpenGL libaries
#include <GL/freeglut.h>
#include <GL/glut.h>

// Personal libaries
#include "display.h"
#include "loadTGA.h"
#include "peripherals.h"




#define M_PI		3.14159265358979323846
/// create wall structure -------------------------------------------------------
#define WALL_ROT_THETA      22
#define WALL_ROT_RAD        (WALL_ROT_THETA*M_PI) / 180
#define RAD_TO_DEG(rad)     (rad*180) / M_PI

#define WALL_SCALE_WIDTH    0.3
#define WALL_SCALE_HEIGHT   3
#define WALL_SCALE_LENGTH   8
#define FLOOR_BED           -1
#define WIDTH_SPACE         9

// move wall in place respect to angles
#define WALL_X     WALL_SCALE_LENGTH*sin(WALL_ROT_RAD)
#define WALL_Z     WALL_SCALE_LENGTH*cos(WALL_ROT_RAD)


const int doorHeight = WALL_SCALE_HEIGHT;
const float doorPointLocal[3] = {0.15, 0, 0};                     // pivot point
// const float doorPointGlobal[3] = { };   // diff of point


#define PI		3.14159265358979323846
#define GRAVITY 9.81
#define DRAG_CUBE   1.05  // face  , edge = 0.8
#define DRAG_SPHERE 0.45 
#define AIR_DENSITY 1.225

#define BOX_FRAG_SIZE                   0.1
#define BALL_RADIUS                    0.1


#define MASS_BOX_PIECE                  0.01
#define AREA_BOX_PIECE(size)            size * size * size       // sphere area 4 * PI * (0.1*0.1);
#define V_Terminal(mass, area, drag, scale)    sqrt((2*(mass)*GRAVITY) / (drag*AIR_DENSITY*area)) / scale



#define GL_CLAMP_TO_EDGE                        0x812F



#define FLOOR_X  26
#define FLOOR_Z  26

#define VERTICAL_THETA       90
#define VEL_THETA(theta)     (theta * PI) / 180    // converts angle from (deg to rad)
#define CANNON_BALL_VEL_THETA     38.88 
#define AIR_FRI       0.35



// #define THREADS_BOX_COLL  6







// #if wallHit == 1
// 	#define t     0.001   // good slow-motion camera setting for animation       "0.0001" or "0.00001"
// #else 
// #define t     0.00001 
// #endif










#define MAGIC_CUBES       8


double statBallPosX[MAGIC_CUBES] = { -3.5 , -2.5 , -1.5, -0.5, 0.5, 1.5, 2.5, 3.5 };
double statBallPosY[MAGIC_CUBES] = { 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7 };

double ballPosY[MAGIC_CUBES][MAGIC_CUBES] = { 0 };
double ballPosX[MAGIC_CUBES][MAGIC_CUBES] = { 0 };  // pos width
double ballPosZ[MAGIC_CUBES][MAGIC_CUBES] = { 0 };
double speedY[MAGIC_CUBES][MAGIC_CUBES] = { 0 };

bool initPos = false;


// bouncing ball
double ballBounceY = 1+BALL_RADIUS;
double radiusBallBounce[5] = { 0.05, 0.1, 0.2, 0.25, 0.3};
double _radiusBallBounce = radiusBallBounce[0];
double massBallBounce[5] = { 0.1, 0.5, 10., 30., 60.};
double _massBallBounce = massBallBounce[0];
double ballBounceX = 0;
float glassAlpha = 0.25f;


#define FRAG_BOX_START  2.0


float posRand_X[MAGIC_CUBES+1][MAGIC_CUBES+1]= { 0 };
float posRand_Z[MAGIC_CUBES+1][MAGIC_CUBES+1]= { 0 };

bool horizontalCol[MAGIC_CUBES][MAGIC_CUBES] = { false };
bool   reset[MAGIC_CUBES]  = { false };
bool detectColl[MAGIC_CUBES][MAGIC_CUBES] = { false };

bool cannonBallHit = false;
// double speedX = 0;

enum {
	RANGE_OUT = 0,
	RIGHT = 1,
	LEFT = 2
};


bool _spacePressed = false;
bool _doorAction = false;
bool _spacePressedBallBounce = false;
float doorTheta = 90.f;
bool doorCount = false;



#define DIST_Z     200
#define DIST_X     200
#define HEIGHT_Y   160





pthread_t threads[MAGIC_CUBES];
pthread_t threadsBoxColl[MAGIC_CUBES];
pthread_mutex_t *lock;
pthread_mutex_t *lockBoxColl;
pthread_mutex_t _lockBoxColl = PTHREAD_MUTEX_INITIALIZER;  // thread safe
pthread_mutex_t _lock = PTHREAD_MUTEX_INITIALIZER;  // thread safe


int z = 0;
int z_2 = 0;

double da[MAGIC_CUBES] = { 0 };
int h[MAGIC_CUBES] = { 0 };
int b[MAGIC_CUBES] = { 0 };
int c[MAGIC_CUBES] = { 0 };

// int change = 0;
bool _iState = false; 

bool objCollision[MAGIC_CUBES] = { false };
bool chkCount[MAGIC_CUBES][MAGIC_CUBES] = { false };
bool objStill[MAGIC_CUBES][MAGIC_CUBES] = { false };

int OS = 0;


void boxCube(void);




/** -----------------------------------------------------------------------------
//               Get the absolute path directory from the main file
//  
//  @param path    - absolute path directory	
// 
//  @param return  - directory of the absolute path from the main file
//  -------------------------------------------------------------------------- */
char *getPath(char *path, int _OS) 
{ 
	static char *dirPath = dirname(path);  // Retrieve path excluting file
	OS = _OS;

	return dirPath;
}


// ----------------------------------------------------------------------------
//           				Calculate FPS for Window
// 
//  - Code computes the average frames per second (fps), and also the average			
//    time it takes to render one frame. These stats are appended to the caption 
//    bar window.
// ----------------------------------------------------------------------------
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
		sprintf(title, "Museum    FPS: %.2f", (frameCount / elapsedTime) * FPS_SEC);
		glutSetWindowTitle(title);

		endTime = startTime;
		frameCount = 0;
	}
	frameCount++;
}




static bool viewState = 0;
float look_x, look_z=-1, eye_x, eye_z=20;  //Camera parameters
GLdouble x_view=0,z_view=0, x_view_2=0,z_view_2=0, top_x=0, top_z=-1, top_x_2=0, top_z_2=-1, _zoom_=0;
  

// 3d person camera  ----------------------------------------
void moveBack(float angle)
{
	float _eye_x = eye_x - 0.5*sin(angle);  // temp pos x
	float _eye_z = eye_z + 0.5*cos(angle);  // temp pos z

	if ((-FLOOR_X <= _eye_x && _eye_x <= FLOOR_X) && (-FLOOR_Z <= _eye_z && _eye_z <= FLOOR_Z)) {
		eye_x = _eye_x;
		eye_z = _eye_z;
	}
}


void moveForward(float angle)
{
	float _eye_x = eye_x + 0.5*sin(angle);  // temp pos x
	float _eye_z = eye_z - 0.5*cos(angle);  // temp pos z

	if ((-FLOOR_X <= _eye_x && _eye_x <= FLOOR_X) && (-FLOOR_Z <= _eye_z && _eye_z <= FLOOR_Z)) {
		eye_x = _eye_x;
		eye_z = _eye_z;
	}
}


void lookRotation(float angle, bool _view)
{
	look_x = eye_x + 200*sin(angle);  // angle the person is looking
	look_z = eye_z - 200*cos(angle);
	viewState = _view;
}
/// --------------------------------------




// top down camera  ----------------------------------------

void zoom(GLdouble _zoom)
{
	_zoom_ += _zoom;
}


void topBottomForward(void)
{
	z_view_2 -= 1;
}


void topBottomBack(void)
{
	z_view_2 += 1;
}


void getView(bool _view)
{
	viewState = _view;
}


void topBottomLeft(void)
{
	x_view_2 += 1;
}


void topBottomRight(void)
{
	x_view_2 -= 1;
}

/// --------------------------------------


#define TEX 11
GLuint txId[TEX] = { 0 };   //Texture ids


// load textures - skybox
void _loadTGA(const char *_fileLoc, int index)
{
	char loc[100] = { 0} ;

	glBindTexture(GL_TEXTURE_2D, txId[index]);

	sprintf(loc, "../Models/skybox6/%s.tga", _fileLoc);
	loadTGA(loc);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


// load the texture
//--------------------------------------------------------------------------------
void loadTexture()				
{
	glGenTextures(TEX, txId); 	// Create xxx texture ids

	// skybox textures
	_loadTGA("front", 0);
	_loadTGA("back", 1);
	_loadTGA("left", 2);
	_loadTGA("right", 3);
	_loadTGA("down", 4);
	_loadTGA("up", 5);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// wall museum
	glBindTexture(GL_TEXTURE_2D, txId[6]);
	loadTGA("../Models/wallScene.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	// roof museum
	glBindTexture(GL_TEXTURE_2D, txId[7]);
	loadTGA("../Models/roof.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	// wall 2 scene
	glBindTexture(GL_TEXTURE_2D, txId[8]);
	loadTGA("../Models/wall_2.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	// door
	glBindTexture(GL_TEXTURE_2D, txId[9]);
	loadTGA("../Models/door.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	// fountain texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[10]);
	loadTGA("../Models/fountain.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}


//--------------------------  sky box ------------------------------------------------------
void back(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[1]); // use floor ID 

	glBegin(GL_QUADS);
	glTexCoord2f(0,  1);       glVertex3f(-DIST_X, -HEIGHT_Y, DIST_Z);
	glTexCoord2f(0,  0);       glVertex3f(-DIST_X, HEIGHT_Y, DIST_Z);
	glTexCoord2f(1, 0);        glVertex3f(DIST_X, HEIGHT_Y, DIST_Z);
	glTexCoord2f(1, 1);        glVertex3f(DIST_X, -HEIGHT_Y, DIST_Z); 
	
	glEnd();
}


void front(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[0]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);       glVertex3f(DIST_X, -HEIGHT_Y, -DIST_Z);  
		glTexCoord2f(0,  0);       glVertex3f(DIST_X, HEIGHT_Y, -DIST_Z);  
		glTexCoord2f(1, 0);        glVertex3f(-DIST_X, HEIGHT_Y, -DIST_Z);  
		glTexCoord2f(1, 1);        glVertex3f(-DIST_X, -HEIGHT_Y, -DIST_Z); 
	glEnd();
}


void right(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[3]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);      glVertex3f(DIST_X, -HEIGHT_Y, DIST_Z);
		glTexCoord2f(0,  0);      glVertex3f(DIST_X, HEIGHT_Y, DIST_Z);
		glTexCoord2f(1, 0);       glVertex3f(DIST_X, HEIGHT_Y, -DIST_Z);
		glTexCoord2f(1, 1);       glVertex3f(DIST_X, -HEIGHT_Y, -DIST_Z);
	glEnd();
}


void left(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[2]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);      glVertex3f(-DIST_X, -HEIGHT_Y, -DIST_Z);
		glTexCoord2f(0,  0);      glVertex3f(-DIST_X, HEIGHT_Y, -DIST_Z);
		glTexCoord2f(1, 0);       glVertex3f(-DIST_X, HEIGHT_Y, DIST_Z);
		glTexCoord2f(1, 1);       glVertex3f(-DIST_X, -HEIGHT_Y, DIST_Z);
	glEnd();
}


void floorBed(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[4]); // use floor ID 

	glRotatef(-90, 0, 1, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);       glVertex3f(DIST_X, -HEIGHT_Y, DIST_Z);
		glTexCoord2f(0,  0);       glVertex3f(DIST_X, -HEIGHT_Y, -DIST_Z);
		glTexCoord2f(1, 0);        glVertex3f(-DIST_X, -HEIGHT_Y, -DIST_Z);
		glTexCoord2f(1, 1);        glVertex3f(-DIST_X, -HEIGHT_Y, DIST_Z);
	glEnd();
}


void top(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[5]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);       glVertex3f(DIST_X, HEIGHT_Y,- DIST_Z);
		glTexCoord2f(0,  0);       glVertex3f(DIST_X, HEIGHT_Y, DIST_Z);
		glTexCoord2f(1, 0);        glVertex3f(-DIST_X, HEIGHT_Y, DIST_Z);
		glTexCoord2f(1, 1);        glVertex3f(-DIST_X, HEIGHT_Y, -DIST_Z);
	glEnd();
}


// draw and render skybox
void skyBox(void)
{	
    glPushMatrix();

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

		glColor3f(1, 1, 1);

		// draw skybox
		back();
		front();
		left();
		right();
		floorBed();
		top();

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);

    glPopMatrix();
}
//--------------------------------------------------------------------------------




//----------draw a floor plane-------------------
void drawFloor(void)
{
	glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);
		glColor3f(1, 1, 1);
		for(float x = -FLOOR_X; x < FLOOR_X; x += 0.5) {
			for(float z = -FLOOR_Z; z < FLOOR_Z; z += 0.5) {
				glVertex3f(x, FLOOR_BED, z);
				glVertex3f(x, FLOOR_BED, z+0.5);
				glVertex3f(x+0.5, FLOOR_BED, z+0.5);
				glVertex3f(x+0.5, FLOOR_BED, z);
			}
		}
	glEnd();
}


// ---- draw wall that the player is in
//--------------------------  wall ------------------------------------------------------
void backWall(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[6]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  0);       glVertex3f(-FLOOR_X, FLOOR_BED, FLOOR_Z);
		glTexCoord2f(10,  0);       glVertex3f(FLOOR_X, FLOOR_BED, FLOOR_Z);
		glTexCoord2f(10, 1);        glVertex3f(FLOOR_X,  0.25, FLOOR_Z);
		glTexCoord2f(0, 1);        glVertex3f(-FLOOR_X,  0.25, FLOOR_Z); 
	glEnd();
}


void frontWall(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[6]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);       glVertex3f(FLOOR_X, FLOOR_BED, -FLOOR_Z);  
		glTexCoord2f(0,  0);       glVertex3f(FLOOR_X, 0.25, -FLOOR_Z);  
		glTexCoord2f(10, 0);        glVertex3f(-FLOOR_X, 0.25, -FLOOR_Z);  
		glTexCoord2f(10, 1);        glVertex3f(-FLOOR_X, FLOOR_BED, -FLOOR_Z); 
	glEnd();
}


void rightWall(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[6]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);      glVertex3f(FLOOR_X, FLOOR_BED, FLOOR_Z);
		glTexCoord2f(0,  0);      glVertex3f(FLOOR_X,  0.25, FLOOR_Z);
		glTexCoord2f(10, 0);       glVertex3f(FLOOR_X,  0.25, -FLOOR_Z);
		glTexCoord2f(10, 1);       glVertex3f(FLOOR_X, FLOOR_BED, -FLOOR_Z);
	glEnd();
}


void leftWall(void)
{
	glBindTexture(GL_TEXTURE_2D, txId[6]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);      glVertex3f(-FLOOR_X, FLOOR_BED, -FLOOR_Z);
		glTexCoord2f(0,  0);      glVertex3f(-FLOOR_X,  0.25, -FLOOR_Z);
		glTexCoord2f(10, 0);       glVertex3f(-FLOOR_X,  0.25, FLOOR_Z);
		glTexCoord2f(10, 1);       glVertex3f(-FLOOR_X, FLOOR_BED, FLOOR_Z);
	glEnd();
}


// draw and render player wall
void sceneWall(void)
{	
    glPushMatrix();

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

		glColor3f(1, 1, 1);

		// draw player wall limit
		backWall();
		frontWall();
		rightWall();
		leftWall();


		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);

    glPopMatrix();
}

// -----------------------------------




//// -------------- ball and ground collision -------------------------------
void spacePressed(bool _state)
{
	if (_state) {
		_spacePressed=true;
		_spacePressedBallBounce=true;
	} else {
		_spacePressed=false;
		_spacePressedBallBounce=false;
	}
}


void doorAction(bool _state)
{
	if (_state) _doorAction=true;
	else _doorAction=false;
}




// reaction force - when obj 'z' has been hit
void boxChangeSpeed(int j)
{	
	double scaleThread = 1.0;
	float impactForce = 1.;

	if (wallHit)
		impactForce = 0.9;

	if (speedY[z][z_2] < 0)
		speedY[z][z_2] *= -impactForce;
	else 
		speedY[z][z_2] *= impactForce;

	if (speedY[j][z_2] < 0)
		speedY[j][z_2] *= impactForce;
	else 
		speedY[j][z_2] *= -impactForce;

	if (OS == 1)
		scaleThread = (THREADS_BOX_COLL*THREADS_BOX_COLL);

	ballPosY[z][z_2] += speedY[z][z_2] / scaleThread;
	ballPosY[j][z_2] += speedY[j][z_2] / scaleThread;
}


void boxCollision(int j)
{
	if (!chkCount[z][z_2]) {
		boxChangeSpeed(j);
	}
	
	// When the cannon ball hits the gravitational less exclosure - disruppt movement 
	if (wallHit) {
		posRand_X[z][z_2] *= -0.5;
		posRand_X[j][z_2] *= -0.5;

		posRand_Z[z][z_2] *= -0.5;
		posRand_Z[j][z_2] *= -0.5;

		ballPosX[z][z_2] += posRand_X[z][z_2];
		ballPosZ[j][z_2] += posRand_Z[j][z_2];
	}
}


void *_boxDetectBoxCollision(void *arg)
{
	if (OS == 1) pthread_mutex_lock(lockBoxColl);  // lock thread

	// check to see if ball 'z' hits any other balls
	for (int j = 0; j < MAGIC_CUBES; j++) {
		if (j == z) break;
		else if ((ballPosX[z][z_2]-0.05) <= (ballPosX[j][z_2]+0.05) && (ballPosX[j][z_2]-0.05) <= (ballPosX[z][z_2]+0.05)) { 
			if ((ballPosY[z][z_2]-0.05) <= (ballPosY[j][z_2]+0.05) && (ballPosY[j][z_2]-0.05) <= (ballPosY[z][z_2]+0.05)) {
				if ((ballPosZ[z][z_2]-0.05) <= (ballPosZ[j][z_2]+0.05) && (ballPosZ[j][z_2]-0.05) <= (ballPosZ[z][z_2]+0.05)) {
					
					boxCollision(j);

					chkCount[z][z_2] = true;
					chkCount[j][z_2] = true;
				} else {
					chkCount[z][z_2] = false;
					chkCount[j][z_2] = false;
				}}
			else {
				chkCount[z][z_2] = false;
				chkCount[j][z_2] = false;
			}} 
		else {
			chkCount[z][z_2] = false;
			chkCount[j][z_2] = false;
		}
	}


	if (OS == 1) pthread_mutex_unlock(lockBoxColl);  // unlock thread

	return NULL;
}


void boxDetectBoxCollision()
{
	if (OS == 1) {
		// Linux
		for (int i = 0; i < THREADS_BOX_BOX_COLL; i++) {
			lockBoxColl = &_lockBoxColl;
			pthread_create(&threadsBoxColl[i], NULL, _boxDetectBoxCollision, NULL);
		}

		for (int i = 0; i < THREADS_BOX_BOX_COLL; i++) 
			pthread_join(threadsBoxColl[i], NULL);
	} else {
		// Windows 
		_boxDetectBoxCollision(NULL);
	}
}


void boxDetectGroundCollision()
{
	double scaleThread = 1.0;

	float t = 0.0005;
	if (wallHit)
		t = 0.001;

	// box piece's collision with floor
	// if current pos y plus last speed is greater then floor bed, calculate new pos due to speed and direction
	if (!chkCount[z][z_2]) {
		if (ballPosY[z][z_2]+speedY[z][z_2] >= FLOOR_BED+0.05) {
			if (speedY[z][z_2] > -0.1 && speedY[z][z_2] < 0.1 && !wallHit)  // cap the speed for the bouncing cubes
				speedY[z][z_2] -= (sin(VEL_THETA(VERTICAL_THETA)) * t * sin(VEL_THETA(VERTICAL_THETA)) - 0.5 * GRAVITY * (t*t));
			else if (wallHit) // dont cap speed
				speedY[z][z_2] -= (sin(VEL_THETA(VERTICAL_THETA)) * t * sin(VEL_THETA(VERTICAL_THETA)) - 0.5 * GRAVITY * (t*t));
		} else { // hit the floor
			if (wallHit)
				speedY[z][z_2] *= (-1 + V_Terminal(MASS_BOX_PIECE, AREA_BOX_PIECE(BOX_FRAG_SIZE), DRAG_CUBE, 100));  // resistance percentage 
			else
				speedY[z][z_2] *= -1;  // osciallate at constant height
		}

		if (OS == 1)
			scaleThread = (THREADS_BOX_COLL*THREADS_BOX_COLL);

		ballPosY[z][z_2] += speedY[z][z_2] / scaleThread;
	}
}

int timer = 0;
void *floorCollisionBOX(void *arg)
{
	if (OS == 1) pthread_mutex_lock(lock); // lock thread

	// detect for collision
	for (int n = 0; n < MAGIC_CUBES; n++) {               // rows
		for (int n_2 = 0; n_2 < MAGIC_CUBES; n_2++) {     // cols
			z = n;
			z_2 = n_2;
		
			if (!objStill[n][n_2]) {
				boxDetectBoxCollision();
				boxDetectGroundCollision();
			}
		}
	}

	// if (wallHit)
	// 	timer++;

	// Signal done scene
	if (timer > 2300) {
		for (int i = 0; i < MAGIC_CUBES; i++) {                     // rows
			for (int j = 0; j < MAGIC_CUBES; j++) {
				speedY[i][j] = 0;	
				objStill[i][j] = true;
			}
		}
	}

	if (OS == 1) pthread_mutex_unlock(lock);  // unlock thread

    return NULL;
}


void collBox(void) 
{  
	static bool _threadStop = false;

	if (!_threadStop) {
		// if thread is false trigger to stop
		for (int i = 0; i < MAGIC_CUBES; i++) {
			for (int j = 0; j < MAGIC_CUBES; j++) {
				if (!objStill[i][j]) {
					_threadStop = false;
					break;
				}
				_threadStop = true;  // all boxes are true - complete (stop and reset)
			}
		}

		if (OS == 1) {	
			// Linux
			// worker threads
			for (int i = 0; i < THREADS_BOX_COLL; i++) {
				lock = &_lock;
				pthread_create(&threads[i], NULL, floorCollisionBOX, NULL);
			}

			for (int i = 0; i < THREADS_BOX_COLL; i++) 
				pthread_join(threads[i], NULL);
		} else {
			// Windows
			floorCollisionBOX(NULL);
		}
	} 
}


void _boxCube(void)
{
	glPushMatrix();
		glScalef(0.1, 0.1, 0.1);
		glutSolidCube(1);
	glPopMatrix();
}


void _cube3D(int row, int col)
{
	double _rot = 0;

	// get random values when btn pressed - run once ( for the 'X' and 'Z' directions )
	if (!_iState) {
		// keep finding random values every clock tick unitl btn pressed
		for (int i = 0; i < MAGIC_CUBES; i++) {
			for (int j = 0; j < MAGIC_CUBES; j++) {
				posRand_X[i][j] = (float)((rand() % 200) - 100) / 10000.f;
				posRand_Z[i][j] = (float)(rand() % 50) / 5000.f;
			}
		}
		// run once when btn pressed
		_iState = true;
	}

	if (_spacePressed && wallHit) {
		if (0 == (int)(speedY[row][col]*1000.f)) {
			if ((int)(posRand_X[row][col]*10000.f) == 0)
				posRand_X[row][col] = 0;
			else if (posRand_X[row][col] > 0)
				posRand_X[row][col] -= 0.0001;
			else
				posRand_X[row][col] += 0.0001;
				
			ballPosX[row][col] += posRand_X[row][col];

			if ((int)(posRand_Z[row][col]*10000.f) == 0)
				posRand_Z[row][col] = 0;
			else if (posRand_Z[row][col] > 0)
				posRand_Z[row][col] -= 0.0001;
			else
				posRand_Z[row][col] += 0.0001;
				
			ballPosZ[row][col] += posRand_Z[row][col];
		} 
		else {
			ballPosX[row][col] += posRand_X[row][col];
			ballPosZ[row][col] += posRand_Z[row][col];
		}	
	}

	glPushMatrix();	
		glTranslatef(ballPosX[row][col], ballPosY[row][col], ballPosZ[row][col]);

		if (_spacePressed && wallHit) {
			// rotate box with respect to its position
			_rot = RAD_TO_DEG(atan(ballPosZ[row][col]/ballPosX[row][col]));
			glRotatef(_rot, 0, 1, 0);
		}
		_boxCube();
	glPopMatrix();
}


void boxCube(void)
{
	if (!initPos) {
		double _speedY[MAGIC_CUBES] = { 0.02, 0.01, 0, -0.01, 0, 0.01, 0.02, 0.01};

		// build the magic cube scene - with initial positions
		for (int i = 0; i < MAGIC_CUBES; i++) {            // row
			for (int j = 0; j < MAGIC_CUBES; j++) {        // col
				ballPosX[i][j] = statBallPosX [j];
				ballPosY[i][j] = statBallPosY[i];
				speedY[i][j] = _speedY[j];
			}
		}
		
		initPos = true;
	}

	for (int row = 0; row < MAGIC_CUBES; row++) {
		for (int col = 0; col < MAGIC_CUBES; col++) {
			if (row % 2 == 0) glColor3f(0, 1, 0);
			else glColor3f(0.6, 0, 0);
			_cube3D(row, col);
		}
	}
}


void ball(void)
{
	glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(ballBounceX, ballBounceY+BALL_RADIUS, 0);
		glutSolidSphere(_radiusBallBounce, 36, 18);
	glPopMatrix();
}


void rstBall(double *ballVelBounceY,  double *ballBounceX, double *_fri, bool *reset, int index, bool *initState) 
{
	ballBounceY = 1;
	*ballVelBounceY = 0;
	*ballBounceX = 0;
	*_fri = 0;

	_radiusBallBounce = radiusBallBounce[index%5];
	_massBallBounce = massBallBounce[index%5];

	*reset = false;
	*initState = true;
	_spacePressedBallBounce = false;
}


void ballBounce(int value) 
{  
	double area = 4 * PI * (_radiusBallBounce*_radiusBallBounce);  // area of sphere
	double _t = 0.001;
	static double _fri = 0;

	static double ballVelBounceY = 0;
	static bool reset = false;
	static bool initState = true;
	static int index = 0;

	
	// change the mass and radius of the magical - bouncing ball
	if (initState) {
		index += 1 % 5;
		initState = false;
	}
 
	if (ballBounceY+ballVelBounceY >= FLOOR_BED+_radiusBallBounce) {
		reset = false;
		ballVelBounceY -=  sin(VEL_THETA(VERTICAL_THETA)) * _t * sin(VEL_THETA(VERTICAL_THETA)) - 0.5 * GRAVITY * (_t*_t);   // Gravity acceleration movement (drag)
	} else {
		_fri += 0.0001;  // slow x-movement every time hit GND
		if (reset) rstBall(&ballVelBounceY, &ballBounceX, &_fri, &reset, index, &initState);  // if ball hits ground more then twice at a time - reset ball
		reset = true;
		ballVelBounceY *= -1 + V_Terminal(_massBallBounce, area, DRAG_SPHERE, 100) ;  // resistance percentage - relating to mass and shape
	}
	ballBounceY += ballVelBounceY;

	glutTimerFunc(15, ballBounce, 0); 
}







// ---- draw wall that the player is in
//--------------------------  wall ------------------------------------------------------
void _back(int tx)
{
	glBindTexture(GL_TEXTURE_2D, txId[tx]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  0);       glVertex3f(-0.5, -0.5, -0.5);
		glTexCoord2f(1,  0);       glVertex3f(0.5, -0.5, -0.5);
		glTexCoord2f(1, 1);        glVertex3f(0.5, 0.5, -0.5);
		glTexCoord2f(0, 1);        glVertex3f(-0.5, 0.5, -0.5); 
	glEnd();
}


void _front(int tx)
{
	glBindTexture(GL_TEXTURE_2D, txId[tx]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  0);       glVertex3f(-0.5, -0.5, 0.5);
		glTexCoord2f(1,  0);       glVertex3f(0.5, -0.5, 0.5);
		glTexCoord2f(1, 1);        glVertex3f(0.5, 0.5, 0.5);
		glTexCoord2f(0, 1);        glVertex3f(-0.5, 0.5, 0.5); 
	glEnd();
}


// inner wall
void _left(int tx)
{
	glBindTexture(GL_TEXTURE_2D, txId[tx]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);       glVertex3f(0.5, -0.5, -0.5);
		glTexCoord2f(1,  1);       glVertex3f(0.5, -0.5, 0.5);
		glTexCoord2f(1, 0);        glVertex3f(0.5, 0.5, 0.5);
		glTexCoord2f(0, 0);        glVertex3f(0.5, 0.5, -0.5); 
	glEnd();
}


// outer wall
void _right(int tx)
{
	glBindTexture(GL_TEXTURE_2D, txId[tx]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  1);       glVertex3f(-0.5, -0.5, -0.5);
		glTexCoord2f(1,  1);       glVertex3f(-0.5, -0.5, 0.5);
		glTexCoord2f(1, 0);        glVertex3f(-0.5, 0.5, 0.5);
		glTexCoord2f(0, 0);        glVertex3f(-0.5, 0.5, -0.5); 
	glEnd();
}


void _top(int tx)
{
	glBindTexture(GL_TEXTURE_2D, txId[tx]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  0);       glVertex3f(-0.5, 0.5, -0.5);
		glTexCoord2f(2,  0);       glVertex3f(0.5, 0.5, -0.5);
		glTexCoord2f(2, 1);        glVertex3f(0.5, 0.5, 0.5);
		glTexCoord2f(0, 1);        glVertex3f(-0.5, 0.5, 0.5); 
	glEnd();
}


void _bottom(int tx)
{
	glBindTexture(GL_TEXTURE_2D, txId[tx]); // use floor ID 

	glBegin(GL_QUADS);
		glTexCoord2f(0,  0);       glVertex3f(-0.5, -0.5, -0.5);
		glTexCoord2f(10,  0);       glVertex3f(0.5, -0.5, -0.5);
		glTexCoord2f(10, 10);        glVertex3f(0.5, -0.5, 0.5);
		glTexCoord2f(0, 10);        glVertex3f(-0.5, -0.5, 0.5);
	glEnd();
}



// draw and render museum cube texture
void _cubeRender(int tx)
{	
    glPushMatrix();

		glEnable(GL_TEXTURE_2D);
		// glDisable(GL_LIGHTING);

		glColor3f(1, 1, 1);

		// draw player muesum cube - texture
		_back(tx);
		_front(tx);
		_left(tx);
		_right(tx);
		_top(tx);
		_bottom(tx);

		glDisable(GL_TEXTURE_2D);
		// glEnable(GL_LIGHTING);

    glPopMatrix();
}

// -----------------------------------


// Create a single wall
void wall(void)
{
	glPushMatrix();
		// move the wall to the ground level
		glTranslatef(0, (WALL_SCALE_HEIGHT/2)-1, 0);        // y = -0.25  
		glScalef(WALL_SCALE_WIDTH, WALL_SCALE_HEIGHT, WALL_SCALE_LENGTH);
		// glutSolidCube(1);
		_cubeRender(6);
	glPopMatrix();
}


void openDoor(void)
{
	glTranslatef(0, (WALL_SCALE_HEIGHT/2)-1, 0);        // y = -0.25  
	glScalef(WALL_SCALE_WIDTH, WALL_SCALE_HEIGHT, WALL_SCALE_LENGTH/2.0f);
	// glutSolidCube(1);
	_cubeRender(9);
}


void sideWall(void)
{
	glTranslatef(0, (WALL_SCALE_HEIGHT/2)-1, 0);        // y = -0.25  
	glScalef(WALL_SCALE_WIDTH, WALL_SCALE_HEIGHT, WALL_SCALE_LENGTH+0.3);
	// glutSolidCube(1);
	_cubeRender(8);
}




// open and close the door by pressening the 'd'
void animateDoor(int value)
{
	if (_doorAction) {
		if (!doorCount) {
			// open door
			if (doorTheta < 180) {
				doorTheta += 0.5;
			} else {
				doorCount = true;
				_doorAction = false;
			}
		} else {
			// close door
			if (doorTheta > 90.f) {
				doorTheta -= 0.5;
			} else {
				doorCount = false;
				_doorAction = false;
			}
		}
	}

	glutTimerFunc(20, animateDoor, 0);
}


// front and back
void sideWalls()
{
	// back wall
	glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(0, 0, -WALL_Z-WALL_Z/2);   
		glRotatef(90, 0, 1, 0);
		glScalef(1, 1, 2.5);
		sideWall();
	glPopMatrix();
	

	// front left
	glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef((-WALL_X*2)-0.2, 0, WALL_Z/2);   
		glRotatef(90, 0, 1, 0);
		sideWall();
	glPopMatrix();
	

	// front right
	glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef((WALL_X*2)+0.2, 0, WALL_Z/2);   
		glRotatef(90, 0, 1, 0);
		sideWall();
	glPopMatrix();


	// front door - opens anti-clockwise
	glPushMatrix();
		glColor3f(0, 1, 0);

		glTranslatef(2, 0, 2 +2);
		glRotatef(doorTheta, 0, 1, 0);
		glTranslatef(-2, 0, -2 -2);
 
		glTranslatef(2, 0, 2);

		// glRotatef(90, 0, 1, 0);
		openDoor();
	glPopMatrix(); 
}



		

// side walls
// construct the outside of the meusem...
void walls(void)
{	
	// front left
	glPushMatrix();
		glColor3f(1, 0.8, 0);
		glTranslatef(-WALL_X-WIDTH_SPACE, 0, 0);        // move left
		glRotatef(WALL_ROT_THETA, 0, 1, 0);
		wall();
	glPopMatrix();

	// front right
	glPushMatrix();
		glColor3f(1, 0.8, 0);
		glTranslatef(WALL_X+WIDTH_SPACE, 0, 0);    // move right
		glRotatef(-WALL_ROT_THETA, 0, 1, 0);
		wall();
	glPopMatrix();


	// back left
	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(-WALL_X-WIDTH_SPACE, 0, -WALL_Z);
		glRotatef(-WALL_ROT_THETA, 0, 1, 0);
		wall();
	glPopMatrix();

	
	// back right
	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(WALL_X+WIDTH_SPACE, 0, -WALL_Z);
		glRotatef(WALL_ROT_THETA, 0, 1, 0);
		wall();
	glPopMatrix();

	sideWalls();
}

// -------------------------------------------------------






/// create roof structure -------------------------------------------------------
#define ROOF_ROT_THETA      45
#define ROOF_ROT_RAD        (ROOF_ROT_THETA*M_PI) / 180
#define ROOF_SCALE_WIDTH    0.3
#define ROOF_SCALE_HEIGHT   3
#define ROOF_SCALE_LENGTH   8

const int height = ROOF_SCALE_HEIGHT;
const float pointLocal[3] = { 0, -1, 0 };                     // pivot point
const float pointGlobal[3] = { -ROOF_SCALE_WIDTH/2, 0, 0 };   // diff of point



void roofTile(void)
{
	glPushMatrix();
		glScalef(ROOF_SCALE_WIDTH, ROOF_SCALE_HEIGHT, ROOF_SCALE_LENGTH);
		// glutSolidCube(1);
		_cubeRender(7);
	glPopMatrix();
}



void roof(void)
{
	// front left
	glPushMatrix();
		glColor3f(0, 1, 0);
		
		// rotate and translate into correct pos
		glTranslatef(-WALL_X-WIDTH_SPACE, -height/4.f, 0);
		glRotatef(-WALL_ROT_THETA, 0, -1, 0);

		// traslate to the pivot point of the shape
		glTranslatef(pointLocal[0], pointLocal[1] + height, pointLocal[2]); 
		glRotatef(-ROOF_ROT_THETA, 0, 0, 1);
		glTranslatef(-pointLocal[0], -pointLocal[1] - height, -pointLocal[2]); 
	
		glTranslatef(pointGlobal[0], height, 0);  
	
		roofTile();
	glPopMatrix();

	// front right
	glPushMatrix();
		glColor3f(1, 0, 0);
		
		// rotate and translate into correct pos
		glTranslatef(WALL_X+WIDTH_SPACE, -height/4.f, 0);
		glRotatef(WALL_ROT_THETA, 0, -1, 0);

		// traslate to the pivot point of the shape
		glTranslatef(pointLocal[0], pointLocal[1] + height, pointLocal[2]); 
		glRotatef(ROOF_ROT_THETA, 0, 0, 1);
		glTranslatef(-pointLocal[0], -pointLocal[1] - height, -pointLocal[2]); 
	
		glTranslatef(-pointGlobal[0], height, 0);  
	
		roofTile();
	glPopMatrix();

	// back left
	glPushMatrix();
		glColor3f(0, 1, 0);
		
		// rotate and translate into correct pos
		glTranslatef(-WALL_X-WIDTH_SPACE, -height/4.f, -WALL_Z);
		glRotatef(WALL_ROT_THETA, 0, -1, 0);

		// traslate to the pivot point of the shape
		glTranslatef(pointLocal[0], pointLocal[1] + height, pointLocal[2]); 
		glRotatef(-ROOF_ROT_THETA, 0, 0, 1);
		glTranslatef(-pointLocal[0], -pointLocal[1] - height, -pointLocal[2]); 
	
		glTranslatef(pointGlobal[0], height, 0);  
	
		roofTile();
	glPopMatrix();

	// back right
	glPushMatrix();
		glColor3f(0, 1, 0);
		
		// rotate and translate into correct pos
		glTranslatef(WALL_X+WIDTH_SPACE, -height/4.f, -WALL_Z);
		glRotatef(-WALL_ROT_THETA, 0, -1, 0);

		// traslate to the pivot point of the shape
		glTranslatef(pointLocal[0], pointLocal[1] + height, pointLocal[2]); 
		glRotatef(ROOF_ROT_THETA, 0, 0, 1);
		glTranslatef(-pointLocal[0], -pointLocal[1] - height, -pointLocal[2]); 
	
		glTranslatef(-pointGlobal[0], height, 0);  
	
		roofTile();
	glPopMatrix();
}

	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0};

// displays the Muesums External structure 
void displayMuesum()
{
	walls();
	roof();
}



#include <iostream>
#include <fstream>
#include <climits>
#include <math.h> 
#include <GL/freeglut.h>
using namespace std;


//--Globals ---------------------------------------------------------------
float *x, *y, *_z;		//vertex coordinate arrays
int *t1, *t2, *t3;		//triangles
int nvrt, ntri;			//total number of vertices and triangles
// float angle = -10.0;	//Rotation angle for viewing
// float cam_hgt = 100;

#define CANNON_BALL_RADIUS 5
double initSpeedX = 30; 
double initSpeedY = CANNON_BALL_VEL_THETA;
double cannonBallSpeedX = 0;
double cannonBallSpeedY = 0;
double cannonBallPosZ = 0;




// -- given to us
//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)  
{
	ifstream fp_in;
	int num, ne;

	fp_in.open(fname, ios::in);
	if(!fp_in.is_open())
	{
		cout << "Error opening mesh file" << endl;
		exit(1);
	}

	fp_in.ignore(INT_MAX, '\n');				//ignore first line
	fp_in >> nvrt >> ntri >> ne;			    // read number of vertices, polygons, edges

    x = new float[nvrt];                        //create arrays
    y = new float[nvrt];
    _z = new float[nvrt];

    t1 = new int[ntri];
    t2 = new int[ntri];
    t3 = new int[ntri];

	for(int i=0; i < nvrt; i++)                         //read vertex list 
		fp_in >> x[i] >> y[i] >> _z[i];

	for(int i=0; i < ntri; i++)                         //read polygon list 
	{
		fp_in >> num >> t1[i] >> t2[i] >> t3[i];
		if(num != 3)
		{
			cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //not a triangle!!
			exit(1);
		}
	}

	fp_in.close();
	cout << " File successfully read." << endl;
}

//--Function to compute the normal vector of a triangle with index tindx ----------
void normal_cannon(int tindx)
{
	float x1 = x[t1[tindx]], x2 = x[t2[tindx]], x3 = x[t3[tindx]];
	float y1 = y[t1[tindx]], y2 = y[t2[tindx]], y3 = y[t3[tindx]];
	float z1 = _z[t1[tindx]], z2 = _z[t2[tindx]], z3 = _z[t3[tindx]];
	float nx, ny, nz;

	nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
	ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
	nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);

	glNormal3f(nx, ny, nz);
}

//--------draws the mesh model of the cannon----------------------------
void _drawCannon(void)
{
	glColor3f(0.4, 0.5, 0.4);

    // Construct the object model here using triangles read from OFF file
	glBegin(GL_TRIANGLES);
		for(int tindx = 0; tindx < ntri; tindx++) {
		   normal_cannon(tindx);
		   glVertex3d(x[t1[tindx]], y[t1[tindx]], _z[t1[tindx]]);
		   glVertex3d(x[t2[tindx]], y[t2[tindx]], _z[t2[tindx]]);
		   glVertex3d(x[t3[tindx]], y[t3[tindx]], _z[t3[tindx]]);
		}
	glEnd();
}

void normal(float x1, float y1, float z1, 
            float x2, float y2, float z2,
		      float x3, float y3, float z3 )
{
	  float nx, ny, nz;
	  nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
	  ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
	  nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(nx, ny, nz);
}


// Create mount bracket object for cannon.
void mntBrack(GLfloat xT, GLfloat yT, GLfloat zT, GLfloat xS, GLfloat yS, GLfloat zS)
{
	glPushMatrix();
		glColor3f(1, 0.8, 0);
		glTranslatef(xT, yT, zT); 
		glScalef(xS, yS, zS);
		glutSolidCube(1);
	glPopMatrix();
}


// Create and fire a cannon ball as a projectile.
void _cannonBall(void)
{
	glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(CANNON_BALL_VEL_THETA-5+cannonBallSpeedX, 73+cannonBallSpeedY, cannonBallPosZ);
		glutSolidSphere(5, 36, 18);
	glPopMatrix();
}


// reset animated scene
void rstCannon(double *ballVelBounceY, double *_fri) 
{
	*ballVelBounceY = 1;
	initSpeedY = CANNON_BALL_VEL_THETA;
	cannonBallSpeedX = 0;
	cannonBallSpeedY = 0; 
	cannonBallPosZ = 0; 
	glassAlpha = 0.25f;
	*_fri = 0;
	timer = 0;
	initPos = false;

	wallHit = false;
	_iState = false;
	_spacePressed = false;

	for (int i = 0; i < MAGIC_CUBES; i++) {                     // rows
		for (int j = 0; j < MAGIC_CUBES; j++) {
			ballPosX[i][j] = 0;
			ballPosY[i][j] = 0;
			ballPosZ[i][j] = 0;
			speedY[i][j] = 0;	
			posRand_X[i][j] = 0;
			posRand_Z[i][j] = 0;
			objStill[i][j] = false;
			chkCount[i][j] = false;
		}
	}
	boxCube();

}


// Cannon ball animation
// -- spinning rotating ball
void cannonBall(int value) 
{  
	double _t = 0.1;

	double mass = 50.;
	double area = 4 * PI * (CANNON_BALL_RADIUS*CANNON_BALL_RADIUS);  // area of sphere
	static double _fri = 0;

	static double ballVelBounceY = 1;

	// animate scene when cannon ball is fired
	if (_spacePressed) {
		if (cannonBallSpeedY > -73+CANNON_BALL_RADIUS-FLOOR_BED && !wallHit) {
			if ((cannonBallSpeedX*0.02) >= 8)  // wall collision check
				wallHit = true;

			cannonBallSpeedX += initSpeedX * _t * cos(VEL_THETA(CANNON_BALL_VEL_THETA));                              // Speed X stays constant
			cannonBallSpeedY += ((initSpeedY-=AIR_FRI) * _t * sin(VEL_THETA(CANNON_BALL_VEL_THETA)) - 0.5 * GRAVITY * (_t*_t));
		} else { 
			if (cannonBallSpeedY+ballVelBounceY >= -73+CANNON_BALL_RADIUS-FLOOR_BED) 
				ballVelBounceY -=  (sin(VEL_THETA(VERTICAL_THETA)) * _t * sin(VEL_THETA(VERTICAL_THETA)) - 0.5 * GRAVITY * (_t*_t));   // Gravity acceleration movement (drag)
			else {
				if (_fri <= 0.8)
					_fri += 0.01;  // slow x-movement every time hit GND

				ballVelBounceY *= (1.8 - V_Terminal(mass, area, DRAG_SPHERE, 1)) ;  // resistance percentage - relating to mass and shape
			}

			cannonBallSpeedY += ballVelBounceY;
			cannonBallSpeedX -= 0.8 - _fri;
			cannonBallPosZ += 0.8 - _fri;

			// Let enough time go by to reset the scene
			if (timer > 1800) {
				rstCannon(&ballVelBounceY, &_fri) ;  // reset ball
			} else 
				timer++;
		}
	}

	glutTimerFunc(10, cannonBall, 0); 
}


void drawCannon(void)
{
	// rotate pivot point then draw cannon
	glPushMatrix();
		glTranslatef(-20, 30, 0);    // Pivot point coordinates    3. Cancel first Translation to get original Translation made
		glRotatef(CANNON_BALL_VEL_THETA, 0, 0, 1);      // Rotate pivot points        2. Rotate object
		glTranslatef(20, -30, 0);    // inverse of pivot points    1. Translate object to position
		_drawCannon();                // draw object                0. Draw object
	glPopMatrix();

	// create brackets
	mntBrack(-10, 5, 17, 80, 10, 6);   // first cube
	mntBrack(-20, 25, 17, 40, 30, 6);  // second cube

	// mirrored bracket
	mntBrack(-10, 5, -17, 80, 10, 6); 
	mntBrack(-20, 25, -17, 40, 30, 6);

	_cannonBall();  // cannon ball - projectile motion fire
}



void ballBoxDemo(void)
{
   	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
		glColor4f(255.f/200.f, 255.f/175.f, 1.f, 0.25f);

		// front
		glVertex3f(-1, -1, -1);
		glVertex3f(1, -1, -1);
		glVertex3f(1, 4, -1);
		glVertex3f(-1, 4, -1);

		// back
		glVertex3f(-1, -1, -2);
		glVertex3f(1, -1, -2);
		glVertex3f(1, 4, -2);
		glVertex3f(-1, 4, -2);


		// left
		glColor4f(255.f/200.f, 255.f/175.f, 1.f, 0.6f);
		glVertex3f(-1, -1, -1);
		glVertex3f(-1, -1, -2);
		glVertex3f(-1, 4, -2);
		glVertex3f(-1, 4, -1);

		// right
		glVertex3f(1, -1, -1);
		glVertex3f(1, -1, -2);
		glVertex3f(1, 4, -2);
		glVertex3f(1, 4, -1);

		// top
		glColor4f(255.f/200.f, 255.f/175.f, 1.f, 0.75f);
		glVertex3f(-1, 4, -1);
		glVertex3f(1, 4, -1);
		glVertex3f(1, 4, -2);
		glVertex3f(-1, 4, -2);

		// middle
		glColor4f(1.f, 1.f, 1.f, 0.75f);
		glVertex3f(-1, 1.5, -1);
		glVertex3f(1, 1.5, -1);
		glVertex3f(1, 1.5, -2);
		glVertex3f(-1, 1.5, -2);
	glEnd();

	glDisable(GL_BLEND);
   glDepthMask(GL_TRUE); 
}




double walkTheta = 20.;
int dir = 1;

double guardPosX = 0;
double guardPosZ = -1;
double guardTheta = 0;
float armAction = 0;
float spotDirX_2 = 0;
float spotDirZ_2 = 0;

// Adding walk animation
void guardAnimation(int value) 
{  
	static double walkDir = 0;
	static bool walkStage = false;

	walkTheta += (walkTheta <= -20 ? dir*=-1 : 20 <= walkTheta ? dir*=-1 : dir); 
	
	if (!walkStage) {
		if (guardPosX > 5.5 ) {  // Guard walking from (left->right->left)
			if (guardTheta >= 270) {
				walkDir = -0.02;
				walkStage = true;
				spotDirX_2 = -2;
			} else {
				if (guardTheta >= 180) {
					walkDir = -0.005;
					spotDirZ_2 += 0.02;  // the rate of light turn - relating to guard turn
					spotDirX_2 -= 0.02;
				} else {
					walkDir = 0.005;
					spotDirZ_2 -= 0.02;
					spotDirX_2 = 2;
				}

				guardPosZ -= 0.02;
				guardTheta += 0.5;
			}
		} 
		else {
			spotDirX_2 = 2;
			walkDir = 0.02;
			guardTheta = 90;
			walkStage = false;
		}
	} else {
		if (guardPosX < -6) {  // Guard walking from (right->left->right)
			if (guardTheta >= 270) {
				walkDir = 0.02;
				walkStage = false;
				spotDirX_2 = 2;
			} else {
				if (guardTheta >= 180) {
					walkDir = 0.005;
					spotDirZ_2 -= 0.02;
					spotDirX_2 += 0.02;
				} else {
					walkDir = -0.005;
					spotDirZ_2 += 0.02;
					spotDirX_2 = -2;
				}

				guardPosZ += 0.02;
				guardTheta+=0.5;
			}
		} else {
			spotDirX_2 = -2;
			walkDir = -0.02;
			guardTheta = 90;
			walkStage = true;
		}
	}
	guardPosX += walkDir;

	if (wallHit) {
		armAction = 3;
		glutTimerFunc(10, guardAnimation, 0); 
	} else {
		armAction = 0;
		glutTimerFunc(20, guardAnimation, 0); 
	}
}


//--Draws a character model constructed using GLUT objects ------------------
void drawGuard(bool color)
{
	if (color) glColor3f(1., 0.78, 0.06);		//Head
	else glColor3f(0.2, 0.2, 0.2);  // shadow color
	glPushMatrix();
	  glTranslatef(0, 7.7, 0);
	  glutSolidCube(1.4);
	glPopMatrix();

	if (color) glColor3f(1., 0., 0.);			//Torso
	glPushMatrix();
		glTranslatef(0, 5.5, 0);
		glScalef(3, 3, 1.4);
		glutSolidCube(1);
	glPopMatrix();

	if (color) glColor3f(0., 0., 1.);			//Right leg
	glPushMatrix();
		glTranslatef(-0.8, 4, 0);       
		glRotatef(-walkTheta, 1, 0, 0);       
		glTranslatef(0.8, -4, 0);

		glTranslatef(-0.8, 2.2, 0);
		glScalef(1, 4.4, 1);
		glutSolidCube(1);
	glPopMatrix();

	if (color) glColor3f(0., 0., 1.);			//Left leg
	glPushMatrix();
		// Rotation by walkTheta about the pivot point (0.8, 4, 0)
		glTranslatef(0.8, 4, 0);       
		glRotatef(walkTheta, 1, 0, 0);       
		glTranslatef(-0.8, -4, 0);
		
		glTranslatef(0.8, 2.2, 0);
		glScalef(1, 4.4, 1);
		glutSolidCube(1);
	glPopMatrix();

	if (color) glColor3f(0., 0., 1.);			//Right arm
	glPushMatrix();
		glTranslatef(0.8, 4, 0);       
		glRotatef(walkTheta, 1, 0, 0);       
		glTranslatef(-0.8, -4, 0);

		glTranslatef(-2, 5+armAction, 0);
		glScalef(1, 4, 1);
		glutSolidCube(1);
	glPopMatrix();

	if (color) glColor3f(0., 0., 1.);			//Left arm
	glPushMatrix();
		glTranslatef(2, 6.5, 0);       
		glRotatef(-walkTheta, 1, 0, 0);       
		glTranslatef(-2, -6.5, 0);

		glTranslatef(2, 5+armAction, 0);
		glScalef(1, 4, 1);
		glutSolidCube(1);
	glPopMatrix();
}


#define MAX_PARTICLES   100


//--------------Structure For Particle---------------------------------
typedef struct						
{
	float pos_X;
	float pos_Y;
	float pos_Z;

	float posRand_X;
	float posRand_Y;
	float posRand_Z;

	float animateWait;

	bool dir_Y;
	bool kill;
} Particle_Generator;					


Particle_Generator part[MAX_PARTICLES];

/* initialize particles */
void rstIndexPart(int i)
{
	part[i].posRand_X = (((float)(rand() % 50)-25) / 7000.f);
	part[i].posRand_Y = (float)(rand() % 50) / 5000.f;
	part[i].posRand_Z = (((float)(rand() % 50)-25) / 10000.f);

	part[i].pos_X = 0;
	part[i].pos_Y = 0;
	part[i].pos_Z = 0;

	part[i].animateWait = 0;

	part[i].dir_Y = false;
	part[i].kill = false;
}

// initial particles
void initPartical()
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		part[i].posRand_X = (((float)(rand() % 50)-25) / 7000.f);
		part[i].posRand_Y = (float)(rand() % 50) / 5000.f;
		part[i].posRand_Z = (((float)(rand() % 50)-25) / 10000.f);

		part[i].pos_X = 0;
		part[i].pos_Y = 0;
		part[i].pos_Z = 0;

		part[i].animateWait = 0;

		part[i].dir_Y = false;
		part[i].kill = false;
	}
}


GLUquadric *q =  gluNewQuadric();

bool flow[MAX_PARTICLES] = { true, false };


void rain(int i)
{
	float t = 0.0001;

	if (part[i].pos_Y > 0.02) {
		part[i].pos_X += part[i].posRand_X;
		part[i].pos_Z += part[i].posRand_Z;
		flow[i] = true;
	}

	if (part[i].pos_Y < 0.5 && !part[i].dir_Y) {
		part[i].posRand_Y += sin(VEL_THETA(VERTICAL_THETA)) * t * sin(VEL_THETA(VERTICAL_THETA)) - 0.5 * GRAVITY * (t*t);   // Gravity acceleration movement (drag)
	} else {
		part[i].dir_Y = true; // reached max point now comeing down
		if (part[i].pos_Y > -0.8){   // dont kill particle - kill before hits ground
			part[i].pos_X += part[i].posRand_X;
			part[i].posRand_Y -= sin(VEL_THETA(VERTICAL_THETA)) * t * sin(VEL_THETA(VERTICAL_THETA)) - 0.5 * GRAVITY * (t*t);   // Gravity acceleration movement (drag)
		} else {
			part[i].kill = true;
		}
	}

	if (!part[i].kill) {
		part[i].pos_Y += part[i].posRand_Y;
	} else {
		rstIndexPart(i);
	}
}


// -- render particle system
void particleFountain(void)
{   
	static bool begin = false;
	
	if (!begin) {
		initPartical();
		begin = true;
	}

	for (int i = 0; i < MAX_PARTICLES-1; i++) {
		if (flow[i]) rain(i+1);

		glColor3f(0, 1, 1);
			glPushMatrix();
				glTranslatef(part[i].pos_X, part[i].pos_Y, part[i].pos_Z);
				gluSphere(q, 0.03, 20, 4);
				// gluSphere(q, 0.1, 20, 4);
			glPopMatrix();
		} 
}



#define   N   20

float vx_init[N] = { 8, 7.5, 5.5, 5, 4, 3, 2, 1, 0.75, 0.5, 1, 2, 3, 4, 5, 6, 7.5, 8, 9.5, 11 };          
float vy_init[N] = { 0, 3, 4.5, 6.0, 6.7, 7.0, 7.5, 8.0, 8.4, 9.0, 10.0, 10.5, 11, 11.5, 12.2, 13.5, 15, 17, 20, 22 }; 
float vz_init[N] = { 0 };


void drawFountain(void)
{
	float vx[N], vy[N], vz[N];
	float wx[N], wy[N], wz[N]; 

	// init data
	for (int i = 0; i < N; i++) {
		vx[i] = vx_init[i];
		vy[i] = vy_init[i];
		vz[i] = vz_init[i];
	}

	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 11);

	for(int j = 0; j < 100; j++) {   // 100 slices in 3.6 deg steps
		glBegin(GL_TRIANGLE_STRIP);

			for (int i = 0; i < N; i++) { 
				wx[i] = vx[i] * cos((-3.6*M_PI)/180.) + vz[i] * sin((-3.6*M_PI)/180.);  // transformed points w
				wy[i] = vy[i];
				wz[i] = -vx[i] * sin((-3.6*M_PI)/180.) + vz[i] * cos((-3.6*M_PI)/180.);

				if(i > 0) normal(wx[i-1], wy[i-1], wz[i-1],
					             vx[i],   vy[i],   vz[i],
                                 wx[i],   wy[i],   wz[i]);
					glTexCoord2f((float)(j)/260.f,(float)i/(N-1));          glVertex3f(vx[i], vy[i], vz[i]);
					glTexCoord2f((float)(j+1)/260.f, (float)i/(N-1));       glVertex3f(wx[i], wy[i], wz[i]);
				}
		
		glEnd();

		//Copy W array to V for next iteration
		for (int i = 0; i<N; i++){
			vx[i] = wx[i];
			vy[i] = wy[i];
			vz[i] = wz[i];
		}
	}

	glDisable(GL_TEXTURE_2D);
}



const float orange[4]  = {1, 0.5, 0, 1};
// ----------------------------------------------------------------------------
//  				  		Display OpenGL graphics				
// ----------------------------------------------------------------------------
void display(void)  
{
	const float lposHouse[4] = { 0.f, 2.f, -3.f, 1.f };  // light pos for house

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();


	// change camera
	if (viewState) {   // top down view
		glTranslatef(x_view_2, z_view_2, _zoom_);  // since transformed y has become z
		gluLookAt(0, 30, 0,  0, 0, -1, 0, 1, 0);
	} else {
		// gluLookAt(-5, 1, 0,  0, 0, -1, 0, 1, 0);
		gluLookAt(eye_x, 0.5, eye_z,  look_x, 0.5, look_z,   0, 1, 0);	
	}
	

	// Default house lighting
	glLightfv(GL_LIGHT0, GL_POSITION, lposHouse);   // light for house


	// disable specular lighting ---------------------
	

	// move the box cube to it's spot in the scene
	glPushMatrix();	
		glTranslatef(0 , 0, -6);
		boxCube();
	glPopMatrix();




	glMaterialfv(GL_FRONT, GL_SPECULAR, black);     // ---



		
	skyBox();
	drawFloor();
	sceneWall();

	// draw objects with only ambiant and diffuse lighting
	displayMuesum();


	float gx = guardPosX-2;
	float gy = 2;
	float gz = guardPosZ;

	float shadowMat[16] = { gy,0,0,0, -gx,0,-gz,-1, 0,0,gy,0, 0,0,0,gy };


	glDisable(GL_LIGHTING);
	glPushMatrix();
		//Draw Shadow Object
		glTranslatef(0 , FLOOR_BED+0.01, 0);  // transform shadow to be on the ground
		glMultMatrixf(shadowMat);
		glTranslatef(guardPosX+2 , -1, guardPosZ);
		glRotatef(guardTheta, 0, 1, 0);

		glScalef(0.2, 0.2, 0.2);
		drawGuard(false);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	// draw the guard in the scene
	glPushMatrix();	
		//Draw Actual Object
		glTranslatef(guardPosX , FLOOR_BED, guardPosZ);
		glRotatef(guardTheta, 0, 1, 0);

		glScalef(0.2, 0.2, 0.2);
		drawGuard(true);
	glPopMatrix();


	float spot_pos_1[]= { (float)guardPosX, 1., (float)guardPosZ, 1. };
   	float spotDir_2[] = { (float)spotDirX_2, -1., (float)spotDirZ_2, 1. };

	// spotlight tourch - guard
	glLightfv(GL_LIGHT2, GL_POSITION, spot_pos_1);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir_2);



	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	glBegin(GL_QUADS);
		if (wallHit && glassAlpha > 0) glassAlpha -= 0.01f;
		glColor4f(255.f/200.f, 255.f/175.f, 1.f, glassAlpha);
		// front
		glVertex3f(-4, FLOOR_BED, -5);
		glVertex3f(4, FLOOR_BED, -5);
		glVertex3f(4, 3, -5);
		glVertex3f(-4, 3, -5);

		// back
		glVertex3f(-4, FLOOR_BED, -7);
		glVertex3f(4, FLOOR_BED, -7);
		glVertex3f(4, 3, -7);
		glVertex3f(-4, 3, -7);

		// left
		glColor4f(255.f/200.f, 255.f/175.f, 1.f, 0.6f);
		glVertex3f(-4, FLOOR_BED, -5);
		glVertex3f(-4, FLOOR_BED, -7);
		glVertex3f(-4, 3, -7);
		glVertex3f(-4, 3, -5);

		// right
		glVertex3f(4, FLOOR_BED, -5);
		glVertex3f(4, FLOOR_BED, -7);
		glVertex3f(4, 3, -7);
		glVertex3f(4, 3, -5);

		// top
		glColor4f(255.f/200.f, 255.f/175.f, 1.f, 0.75f);
		glVertex3f(-4, 3, -5);
		glVertex3f(4, 3, -5);
		glVertex3f(4, 3, -7);
		glVertex3f(-4, 3, -7);
	glEnd();

	glDisable(GL_BLEND);
   glDepthMask(GL_TRUE); 

	
	collBox();  // animate the bouncing mini boxes - thread optimised



	// reset back to white for any other objects that take light
  	glMaterialfv(GL_FRONT, GL_SPECULAR, white);         // ---
	// enable specular lighting ----------------------


	glPushMatrix();
		glTranslatef(-8, FLOOR_BED, 1);
		glRotatef(45, 0, 1, 0);
		glScalef(0.02, 0.02, 0.02);     // scale cannon to scene size ( 2.0 % of obj) 
		drawCannon();                   // draw whole cannon
	glPopMatrix();



	
	float spot_pos[]={ 1.5, 10, -3.6, 1.0 };
   	float spotDir[] = { -1, -1, 0, 1.0 };  // light1 position (directly above bouncing ball - case)

	// spotlight over case - shineing on ground
	glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);


	glPushMatrix();
		glTranslatef(-9.2 , 2.4, -3.8);
		ball();  // bouncing ball
	glPopMatrix();

	// ball - vase screen box
	glPushMatrix();	
		glTranslatef(-8 , 0, -3);
		glRotatef(60, 0, 1, 0); 
		ballBoxDemo();  // get to experiment with different values (e.g mass)
	glPopMatrix();


	// draw and move the fountain in the scene
	glPushMatrix();	
		glTranslatef(8 , FLOOR_BED, -3);
		glScalef(0.05, 0.05, 0.05);
		drawFountain();
	glPopMatrix();

	glPushMatrix();	
		glTranslatef(8 , 0, -3);
		particleFountain();
	glPopMatrix();

	
	glutSwapBuffers();	

	showFPS();
}
