/*
 *  mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
 * Simple example how to use the spixels library
 */
#include "led-strip.h"
#include <sys/time.h>
#include <unistd.h>  // for usleep()
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "grid.h"
#define FRAME_RATE 3
using namespace spixels;
unsigned int stripNum = 5;
LEDStrip *strips[5];
MultiSPI *spi;
int ledNum = 150;

struct Grid
{
    int ledNum;
    unsigned int stripNum;
    LEDStrip *strips[5];
}

unsigned int wheel(int i) {
    int index;
    unsigned int cols[10] = {
        0xEA2027,0xEE5A24,0xF79F1F,
        0xFFC312,0xC4E538,0xA3CB38,
        0x009432,0x0652DD,0x1B1464,
        0x6F1E51
    };
    index = i % (sizeof(cols)/sizeof(cols[0]));
    return cols[index];
}

int main() {
    // See include/multi-spi.h
    spi = CreateDirectMultiSPI();

    // Connect LED strips with 144 LEDs to connector P1 and P2
    // strips[0] = CreateAPA102Strip(spi, MultiSPI::SPI_P1, 150);
    strips[0] = CreateAPA102Strip(spi, MultiSPI::SPI_P2, 150);
    strips[1] = CreateAPA102Strip(spi, MultiSPI::SPI_P3, 150);
    strips[2] = CreateAPA102Strip(spi, MultiSPI::SPI_P4, 150);
    strips[3] = CreateAPA102Strip(spi, MultiSPI::SPI_P9, 150);
    strips[4] = CreateAPA102Strip(spi, MultiSPI::SPI_P6, 150);

    struct Grid grid = {150, 5, strips}

    clearGrid(grid);
    spi->SendBuffers();
    usleep(1000000 * 2);

    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[1]->count(); ++i) {
            strips[j]->SetPixel(i, wheel(i));
        }
    }
    spi->SendBuffers();
    usleep(1000000);

    // Main loop
    float bpm = 60;  // bpm
    unsigned int i = 0;
    struct timeval tv;
    unsigned long start; // start time
    unsigned int beat; // current beat
    float elapsed, beatProgress, beatDuration;

    gettimeofday(&tv, NULL);
    start = 1000000*tv.tv_sec+tv.tv_usec;
    printf("Current microsec: %lu\n",start);
    printf("Starting program loop \n");

    while(true){
        // Every loop, calculate time elapsed, beat progress
        gettimeofday(&tv, NULL);
        elapsed = ((float)((1000000*tv.tv_sec+tv.tv_usec) - start)/1000000);

        // Calculate beat progress
        beatDuration = (1/(bpm/60));
        beat = (elapsed * bpm / 60);
        beatProgress = (elapsed-(beat*beatDuration))/beatDuration;

        // printf("Current beat: %d\n", beat);
        // printf("Beat progress: %f\n", beatProgress);
        // Clear pixels
        clearGrid();
        spi->SendBuffers();
        // twinkle(beatProgress, 0XFFFFFF);

        //renderLengthWipe(beatProgress, wheel(beat));
        // renderBlockWipe(beatProgress, wheel(beat));
        // renderCenterWipe(beatProgress, 0xF79F1F);
        // renderWidthWipe(beatProgress, wheel(beat));
        if(beat %  8 == 0 || !beat)
            renderLengthWipe(beatProgress, wheel(beat));
        if(beat %  8 == 1 || !beat)
            renderLengthWipe(beatProgress, wheel(beat));
        if(beat %  8 == 2)
            renderCenterWipe(beatProgress, wheel(beat));
        if(beat %  8 == 3)
            renderCenterWipe(beatProgress, wheel(beat));
        if(beat %  8 == 4)
            renderWidthWipe(beatProgress, wheel(beat));
        if(beat %  8 == 5)
            renderWidthWipe(beatProgress, wheel(beat));
        if(beat %  8 == 6)
            renderBlockWipe(beatProgress, wheel(beat));
        if(beat %  8 == 7)
            renderBlockWipe(beatProgress, wheel(beat));
        // renderSolid(wheel(beat));
        // brightenStrip(beatProgress);
        // fadeStrip(beatProgress)

        // Light up one strip at a time
        // if(beat != lastBeat) {
        //     randomStrips(3);
        //     lastBeat = beat;
        // }

        // brightenStrips(0xEE5A24, i);

        spi->SendBuffers();
        usleep(1000000 / 360);
        if(i%360==0) {
            printf("FPS: %lf\n",(i/elapsed));
        }
        i++;
    }

    for (unsigned int j = 0; j<stripNum; ++j) {
        delete strips[j];
    }
    delete spi;
}
