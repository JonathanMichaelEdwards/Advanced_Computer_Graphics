#ifndef DISPLAY_H
#define DISPLAY_H


#include <iostream>
#include <map>
#include <fstream>
#include <math.h>
using namespace std;

// OpenGL libaries

#include <GL/freeglut.h>
#include <IL/il.h>


// Assimp & Devil animation & image libs
#include <assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp_extras.h"




#define 	MODEL_PATH_1 		"./models/Dwarf"
#define 	MODEL_1			    "dwarf.x"

#define 	MODEL_PATH_2 		"./models/Mov"
#define 	MODEL_2			    "Dance.bvh"


// FPS details
#define FPS            60
#define FPS_SEC        1000
#define TIMER_DELAY    FPS_SEC / FPS
#define QUATER_SEC     250
#define TITLE_SPACE    100


static bool wallHit = false;




void moveBack(float angle);
void moveForward(float angle);
void lookRotation(float angle, bool _view);
void animateDoor(int value);

// top camera
void topBottomForward(void);
void topBottomBack(void);
void topBottomLeft(void);
void topBottomRight(void);
void getView(bool _view);
void zoom(double _zoom);
void loadMeshFile(const char* fname);
void cannonBall(int value);

void spacePressed(bool _state);
void doorAction(bool _state);
void ballBounce(int value);
void boxCube(int value);
void collBox(int value);
void guardAnimation(int value);
void init_particle(void);

char *getPath(char *path, int _OS);
void loadTexture(void);	
void display(void);

aiScene* get_scene();
int loadModel(const char* fileName, unsigned long anim_flag);
void loadGLTextures_devil(void);

void updateNodeMatrices(int tick);


#endif // DISPLAY_H