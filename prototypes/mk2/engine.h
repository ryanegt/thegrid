// engine.h
#include "led-strip.h"
#include <sys/time.h>

using namespace spixels;

// Length and width of LED grid
#define GRIDW 5
#define GRIDL 150

// A pixel is just a holder for an rgba value
typedef struct pixel {
    int r;
    int g;
    int b;
    float a;
} pixel;

// The virtual frame stack
extern pixel * frames[][GRIDW*GRIDL];

// Forward function declarations
void engineInit(void);
void renderGrid(void);
void blendPixelColor(pixel * bottom, pixel * top);
int pixelToRgb(pixel * pix);
