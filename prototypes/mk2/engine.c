// engine.c
#include "engine.h"
#include <math.h>
#include <cstdio>

/*
* Virtual frames. We draw to these.
* Background frame[0]
* Primary effect frame[1]
* Secondary effect frame[2]
*/
pixel * frames[3][GRIDW*GRIDL];
pixel * gridFrame[GRIDW*GRIDL];
LEDStrip *strips[5];
MultiSPI *spi;

/*
* Attach LED strips to address variables, init
* all our virtual frames
*/
void engineInit(void) {
    spi = CreateDirectMultiSPI();
    strips[0] = CreateAPA102Strip(spi, MultiSPI::SPI_P2, 150);
    strips[1] = CreateAPA102Strip(spi, MultiSPI::SPI_P3, 150);
    strips[2] = CreateAPA102Strip(spi, MultiSPI::SPI_P4, 150);
    strips[3] = CreateAPA102Strip(spi, MultiSPI::SPI_P9, 150);
    strips[4] = CreateAPA102Strip(spi, MultiSPI::SPI_P6, 150);

    // Init virtual frames to transparent black
    for(int f = 0; f < 3; ++f) {
        for(int p = 0; p < GRIDW*GRIDL; ++p) {
            frames[f][p] = new pixel({0,0,0,0});
        }
    }

    // Init output frame to transparent black
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        gridFrame[p] = new pixel({0,0,0,0.0});
    }
}

/*
* Combine the led configuration in the 3 virtual
* frames into one output frame and send it to
* the grid hardware.
*/
void renderGrid() {
    int px = 0;
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            for(int f = 0; f < 3; ++f) {
                blendPixelColor(gridFrame[px], frames[f][px]);
            }
            strips[j]->SetPixel(i, pixelToRgb(gridFrame[px]));
            px++;
        }
    }
}

/*
* Interpolate color of laying a pixel on top of another
*/
void blendPixelColor(pixel * bottom, pixel * top) {
    printf("Blend pixel from: %d to %d\n",bottom->b, top->b);
    // Save ourself some compute cycles
    // If bottom is v transparent
    if(bottom->a < 3 || top->a > 253) {
        bottom = top;
        return;
    }
    // When no top is passed or v transparetn
    if(top->a < 3)
        return;

    bottom->r = ceil(bottom->r+((top->r-bottom->r)*top->a)); // red
    bottom->g = ceil(bottom->g+((top->g-bottom->g)*top->a)); // green
    bottom->g = ceil(bottom->b+((top->b-bottom->b)*top->a)); // blue
    bottom->a = bottom->a; // preserve first layer alpha...
    return;
}

int pixelToRgb(pixel * pix) {
    return 0;
}
