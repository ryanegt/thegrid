// engine.c
#include "engine.h"
#include <math.h>
#include <cstdio>
#include <memory>

/*
* Virtual frames. We draw to these.
* Background frame[0]
* Primary effect frame[1]
* Secondary effect frame[2]
*/
frame virtualFrames[VFRAMES];
frame masterFrame;
show master;

// References to hardwared
LEDStrip * strips[GRIDW];
MultiSPI * spi;

// Init variables for timing
volatile int beat = 0;  // current beat
volatile bool beatPulse = false;
uint64_t startTime; // show start time
int measure = 0; // current measure
bool measurePulse = false;

/*
* Attach LED strips to address variables, init
* all our virtual frames
*/
void gridInit(void) {
    spi = CreateDirectMultiSPI();
    strips[0] = CreateAPA102Strip(spi, MultiSPI::SPI_P2, GRIDL);
    strips[1] = CreateAPA102Strip(spi, MultiSPI::SPI_P3, GRIDL);
    strips[2] = CreateAPA102Strip(spi, MultiSPI::SPI_P4, GRIDL);
    strips[3] = CreateAPA102Strip(spi, MultiSPI::SPI_P9, GRIDL);
    strips[4] = CreateAPA102Strip(spi, MultiSPI::SPI_P6, GRIDL);
    strips[5] = CreateAPA102Strip(spi, MultiSPI::SPI_P1, GRIDL);
    strips[6] = CreateAPA102Strip(spi, MultiSPI::SPI_P11, GRIDL);
    strips[7] = CreateAPA102Strip(spi, MultiSPI::SPI_P13, GRIDL);

    // Init virtual frames to transparent black
    for(int f = 0; f < VFRAMES; ++f) {
        for(int p = 0; p < GRIDW*GRIDL; ++p) {
            virtualFrames[f][p] = color({0,0,0,0});
        }
    }

    // Init output frame to transparent black
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        masterFrame[p] = color({0,0,0,1});
    }
}

/*
* Combine the led configuration in the 3 virtual
* frames into one output frame and send it to
* the grid hardware.
*/
void renderGrid(void) {
    int px = 0;
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            for(int f = 0; f < VFRAMES; ++f) {
                blendPixelColor(&(masterFrame[px]), &(virtualFrames[f][px]));
            }
            // printf("Setting pix %d to (%d,%d,%d,%f)\n",px,gridFrame[px]->r,gridFrame[px]->g,gridFrame[px]->b,gridFrame[px]->a);
            strips[j]->SetPixel(i, colorToHex(masterFrame[px]));
            px++;
        }
    }
    spi->SendBuffers();
    return;
} 

/*
* Shut it all down
*/
void clearGrid(void) {
    for(int f=0; f < VFRAMES; ++f) {
        for(int p=0; p < GRIDW*GRIDL; ++p) {
            virtualFrames[f][p] = {0,0,0,0};
        }
    }
    for(int p=0; p < GRIDW*GRIDL; ++p) {
        masterFrame[p] = {0,0,0,1};
    }
    return;
}

/*
* Interpolate color of laying a pixel on top of another, flatten img
*/
void blendPixelColor(color * bottom, color * top) {
    int r, g, b;
    // Save ourself some compute cycles
    if(top->a < 0.01) {
        r = bottom->r;
        g = bottom->g;
        b = bottom->b;
    } else if(top->a > 0.99) {
        r = top->r;
        g = top->g;
        b = top->b;
    } else {
        // Do the pixel math
        r = ceil(bottom->r+((top->r-bottom->r)*top->a)); // red
        g = ceil(bottom->g+((top->g-bottom->g)*top->a)); // green
        b = ceil(bottom->b+((top->b-bottom->b)*top->a)); // blue
    }
    // Set the values
    bottom->r = r;
    bottom->g = g;
    bottom->b = b;
    return;
}

/*
* Return hex int of pixel RGB
*/
int colorToHex(color pix) {
    return ceil(pix.r*(pow(256,2))*pix.a)
        + ceil((pix.g*256)*pix.a)
        + ceil(pix.b*pix.a);
}

/*
* Get a hex and return a pixel
*/
color hexToColor(int col) {
    int r = (int)floor(col/pow(256,2));
    int g = (int)floor(col/pow(256,1)) % 256;
    int b = col % 256;
    return {r,g,b,1.0};
}

/*
* Sum RGB to figure out how bright a pixel is.
*/
int pixelIntensity(color * pix) {
    return (int)(pix->r + pix->g + pix->b);
}

/*
* Call command line mpd/mpc to play song
* TODO: measure start time/response
*/
void playSong(char * fileName) {
    printf("Starting music %s...\n",fileName);
    std::string addStr = "mpc add " + std::string(fileName);
    system("mpc clear");
    system(addStr.c_str());
    system("mpc play");
    return;
}

/*
* Init the effect engine. Create an in memory structure
* to map all effects the beats they will play at/during.
*/
void showInit(void) {
    return;
}

/*
* Measure time passed.
*/
double timeDiff(const struct timespec& start, const struct timespec& end)
{
    double diff = (double)(((uint64_t)end.tv_sec * 1e9 + (uint64_t)end.tv_nsec)
        -((uint64_t)start.tv_sec * 1e9 + (uint64_t)start.tv_nsec))/1e9;
    return diff;
}

// From stackoverflow
bool kbhit()
{
    termios term;
    tcgetattr(0, &term);
    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    tcsetattr(0, TCSANOW, &term);
    return byteswaiting > 0;
}
