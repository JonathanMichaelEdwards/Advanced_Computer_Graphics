#ifndef DISPLAY_H
#define DISPLAY_H

// FPS details
#define FPS            60
#define FPS_SEC        1000
#define TIMER_DELAY    FPS_SEC / FPS
#define QUATER_SEC     250
#define TITLE_SPACE    100

// box col
#define THREADS_BOX_COLL      8
#define THREADS_BOX_BOX_COLL  8


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


#endif // DISPLAY_H