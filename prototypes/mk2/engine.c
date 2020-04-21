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

// References to hardwared
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
            frames[f][p] = new pixel({0,0,0,0.0});
        }
    }

    // Init output frame to transparent black
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        gridFrame[p] = new pixel({0,0,0,1.0});
    }
}

/*
* Combine the led configuration in the 3 virtual
* frames into one output frame and send it to
* the grid hardware.
*/
void renderGrid(void) {
    int px = 0;
    pixel * blendPix;
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            for(int f = 0; f < 3; ++f) {
                blendPix = blendPixelColor(gridFrame[px], frames[f][px]);
                gridFrame[px] = blendPix;
                // delete blendPix;
            }
            // printf("Setting pix %d to (%d,%d,%d,%f)\n",px,gridFrame[px]->r,gridFrame[px]->g,gridFrame[px]->b,gridFrame[px]->a);
            strips[j]->SetPixel(i, pixelToRgb(gridFrame[px]));
            px++;
        }
    }
    spi->SendBuffers();
    return;
}

void clearGrid(void) {
    for(int f=0; f < 3; ++f) {
        for(int p=0; p < GRIDW*GRIDL; ++p) {
            frames[f][p]->r = 0;
            frames[f][p]->g = 0;
            frames[f][p]->b = 0;
        }
    }
    return;
}

/*
* Interpolate color of laying a pixel on top of another
*/
pixel * blendPixelColor(pixel * bottom, pixel * top) {
    // printf("Blend pixel from: %d to %d\n",bottom->b, top->b);
    // Save ourself some compute cycles
    // If bottom is v transparent
    int r = ceil(bottom->r+((top->r-bottom->r)*top->a)); // red
    int g = ceil(bottom->g+((top->g-bottom->g)*top->a)); // green
    int b = ceil(bottom->b+((top->b-bottom->b)*top->a)); // blue
    float a = bottom->a; // preserve first layer alpha...
    return new pixel({r,g,b,a});
}

int pixelToRgb(pixel * pix) {
    return ceil(pix->r*(pow(256,2))*pix->a)
        + ceil((pix->g*256)*pix->a)
        + ceil(pix->b*pix->a);
}
