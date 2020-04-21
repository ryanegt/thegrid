// effects.c
#include "engine.h"
#include "effects.h"

/*
* Turn a frame all one color
*/
void setFramePixel(int f, int x, int y, pixel * col) {
    int p = x + y*GRIDL;
    frames[f][p]->r = col->r;
    frames[f][p]->g = col->g;
    frames[f][p]->b = col->b;
    frames[f][p]->a = col->a;
    return;
}

/*
* Set the pixels of a virtual frame to the same color
*/
void colorFrame(int f, pixel * px) {
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            setFramePixel(f, i, j, px);
        }
    }
    return;
}

/*
* Gradient frame
*/
void gradientFrame(int f, pixel * px) {
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            px->a = (float)i/GRIDL;
            setFramePixel(f, i, j, px);
        }
    }
    return;
}
