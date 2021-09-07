#ifndef DISPLAY_H
#define DISPLAY_H

// FPS details  
#define FPS            60
#define FPS_SEC        1000
#define TIMER_DELAY    FPS_SEC / FPS
#define QUATER_SEC     250
#define TITLE_SPACE    100


void display(void);
int getY(int pos);
int getX(int pos);


#endif // DISPLAY_H