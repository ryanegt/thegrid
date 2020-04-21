#include "grid.h"
#include <cstdio>
#include <cstring>
#include <math.h>
#include <map>

pixel* tmpFrame = new pixel[GRIDW*GRIDL];
float oldProgress;
int oldBeat;

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

void setPixelColor(pixel *f, int x, int y, pixel col) {
    int pix = x + y*GRIDL;
    f[pix] = col;
}

int getPixelColor(pixel *f, int x, int y) {
    int pix = x + y*GRIDL;
    return f[pix];
}

pixel getRgba(int r, int g, int b, int a) {
    unsigned ret = (unsigned long)r*(pow(256,3)) + (long)g*(pow(256,2)) + ((long)b*256) + (long)a;
    printf("getRgba returns %lu...\n", ret);
    return ret;
}

// Custom for our blending purposes
int * getRgbaFromLong(long col) {
    printf("In getRgbaFromLong for %ld...\n", col);
    int* rgba = new int[4];
    rgba[0] = ceil(col/pow(256,3));
    printf("rgba[0] is %d\n",rgba[0]);
    rgba[1] = (int)ceil(col/pow(256,2)) % 256;
    rgba[2] = (int)ceil(col/pow(256,1)) % 256;
    rgba[3] = col % 256;
    return rgba;
}

// Custom for our blending purposes
long getLongFromRgba(int *rgba) {
    return rgba[0]*(pow(256,3)) + rgba[1]*(pow(256,2)) + (rgba[2]*256) + rgba[3];
}

// To pass to spixels
int getIntFromRgb(int *rgb) {
    printf("In getIntFromRgb...\n");
    return rgb[0]*(pow(256,2)) + (rgb[1]*256) + rgb[2];
}

void rainbowFrame(pixel *f) {
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            setPixelColor(f, i, j, (long)wheel(i));
        }
    }
    return;
}

void colorFrame(pixel *f, float progress, pixel px) {
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            setPixelColor(f, i, j, col1);
        }
    }
    return;
}

void clearFrame(pixel *f) {
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            setPixelColor(f, i, j, 0x000000);
        }
    }
    return;
}

void clearGrid(void) {
    for(int f=0; f < 3; ++f) {
        memset(frames[f], 0x00000000, sizeof(frames[f]));
    }
    return;
}

void renderLengthWipe(pixel *f, float progress, long frCol, long toCol) {
    int barLen = 50; // how many pixels long is the wipe
    int fromLed = (GRIDL+barLen)*progress - 1;
    int toLed = fromLed-barLen;
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            if(i <= fromLed && i > toLed && i >= 0 && i < GRIDL) {
                // Transparency/dither

            }
        }
    }
    // delete fromRGB;
    // delete toRGB;
    // callOrder++;
}

/*
* Interpolate color of laying on on top of nother
*/
pixel blendPixelColor(pixel bottom, pixel top) {
    printf("Blend pixel from: %ld to %ld\n",bottom, top);
    // Save ourself some compute cycles
    // When no bottom is passed or its v transparent
    if(bottom == 0 || bottom % 256 < 3)
        return top;
    // When no top is passed or v transparetn
    if(top == 0 || top % 256 < 3)
        return bottom;
    // Or when top is passed with 100% transparent
    if(top % 256 > 253)
        return top;

    int* rgba = new int[4];
    int* rgbaTop = getRgbaFromLong(top);
    int* rgbaBottom = getRgbaFromLong(bottom);
    float fadeTop = (255/rgbaTop[3]);

    rgba[0] = ceil(rgbaBottom[0]+((rgbaTop[0]-rgbaBottom[0])*fadeTop)); // red
    rgba[1] = ceil(rgbaBottom[1]+((rgbaTop[1]-rgbaBottom[1])*fadeTop)); // green
    rgba[2] = ceil(rgbaBottom[2]+((rgbaTop[2]-rgbaBottom[2])*fadeTop)); // blue
    rgba[3] = rgbaBottom[3]; // preserve first layer alpha... lets explore this

    long ret = getLongFromRgba(rgba);
    delete(rgbaTop);
    delete(rgbaBottom);
    delete(rgba);
    return ret;
}

/*
* rgba to rgb for LED rendering
*/
int * getRgbFromLong(long color) {
    // A long color has an alpha, translate that to an rgb
    printf("In getRgbFromLong for %ld...\n", color);
    int * rgb = new int[3];
    int * rgba = getRgbaFromLong(color);
    float fade = (255/rgba[3]);
    printf("rgba[0] is %d\n",rgba[0]);
    printf("Fade is %f\n",fade);

    rgb[0] = ceil(rgba[0] * fade); // red
    rgb[1] = ceil(rgba[1] * fade); // green
    rgb[2] = ceil(rgba[2] * fade); // blue

    delete(rgba);
    return rgb;
}
/*
void renderCenterWipe(int *frame, float progress, int frCol, int toCol) {
    int length = 50;
    int first = (GRIDL/2)-(progress*(GRIDL/2+length));
    int last = (GRIDL/2)+(progress*(GRIDL/2+length));
    int* rgb = new int[3];
    int* fromRGB = getRGB(frCol);
    int* toRGB = getRGB(toCol);
    rgb[0] = fromRGB[0]+((toRGB[0]-fromRGB[0])*progress);
    rgb[1] = fromRGB[1]+((toRGB[1]-fromRGB[1])*progress);
    rgb[2] = fromRGB[2]+((toRGB[2]-fromRGB[2])*progress);
    for(int j=0; j < GRIDW; ++j) {
        for(int i=(length*-1); i < (GRIDL+length); ++i) {
            if(i > first && i < (first+length) && i<(GRIDL/2)+1 && i>=0)
                setPixel(frame, i, j, getHex(rgb));

            if(i < last && i>(last-length) && i>(GRIDL/2)-1 && i < GRIDL)
                setPixel(frame, i, j, getHex(rgb));

        }
    }
    delete fromRGB;
    delete toRGB;
    callOrder++;
}

// Linear fade
void renderFadeColor(int *frame, float progress, int frCol, int toCol) {
    int* rgb = new int[3];
    int* fromRGB = getRGB(frCol);
    int* toRGB = getRGB(toCol);
    rgb[0] = fromRGB[0]+((toRGB[0]-fromRGB[0])*progress);
    rgb[1] = fromRGB[1]+((toRGB[1]-fromRGB[1])*progress);
    rgb[2] = fromRGB[2]+((toRGB[2]-fromRGB[2])*progress);
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            setPixel(frame, i, j, getHex(rgb));
        }
    }
    delete fromRGB;
    delete toRGB;
    callOrder++;
}

// Exponential fade
void renderExpFadeColor(int *frame, float progress, int frCol, int toCol) {
    int* rgb = new int[3];
    int* fromRgb = getRGB(frCol);
    int* toRgb = getRGB(toCol);
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            // Transparency/dither
            if(frCol == 0) {
                frCol = getPixel(frame, i, j);
            }
            rgb[0] = fromRgb[0]+((toRgb[0]-fromRgb[0])*pow(progress,4));
            rgb[1] = fromRgb[1]+((toRgb[1]-fromRgb[1])*pow(progress,4));
            rgb[2] = fromRgb[2]+((toRgb[2]-fromRgb[2])*pow(progress,4));
            setPixel(frame, i, j, getHex(rgb));
        }
    }
    delete fromRgb;
    delete toRgb;
    callOrder++;
}
*/
/*
void renderSparkle(int *frame, float progress, int frCol, int toCol) {
    int density = 10;
    int pixelCol = 0;
    int pix = 0;
    int* rgb = new int[3];
    int* fromRGB;
    int* toRGB;
    // int toRGB[3];
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            // pix = j + i*GRIDL;
            if(i % density == 0) {
                setPixel(frame, i, j, toCol);
            } else {
                setPixel(frame, i, j, frCol);
            }

            pix = j + i*GRIDL;
            if((pix*2+beat) % density == 0) {
                pixelCol = getPixel(frame, j, i);
                fromRGB = getRGB(frCol);
                toRGB = getRGB(toCol);
                if(progress < 0.5) {
                    rgb[0] = fromRGB[0]+((toRGB[0]-fromRGB[0])*pow(progress*2,2));
                    rgb[1] = fromRGB[1]+((toRGB[1]-fromRGB[1])*pow(progress*2,2));
                    rgb[2] = fromRGB[2]+((toRGB[2]-fromRGB[2])*pow(progress*2,2));
                } else {
                    rgb[0] = fromRGB[0]-((toRGB[0]-fromRGB[0])*pow(progress*2,2));
                    rgb[1] = fromRGB[1]-((toRGB[1]-fromRGB[1])*pow(progress*2,2));
                    rgb[2] = fromRGB[2]-((toRGB[2]-fromRGB[2])*pow(progress*2,2));
                }
                setPixel(frame, i, j, getHex(rgb));
                delete fromRgb;
                delete toRgb;
            } else {
                setPixel(frame, i, j, frCol);
            }

        }
    }
    callOrder++;
}
void renderOneSparkle(int *frame, float progress, int frCol, int toCol) {
    int col = 0;
    int* rgb = new int[3];
    int* fromRgb = getRGB(frCol);
    int* toRgb = getRGB(toCol);

    if(beat != oldBeat) {
        for(int i=0; i < PIXELS; i++) {
            if(rand() % 20 == 0) {
                tmpFrame[i] = toCol;
            } else {
                tmpFrame[i] = frame[i];
            }
        }
    }

    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            setPixel(frame, i, j, getPixel(tmpFrame, i, j));
        }
    }
    // int toRGB[3];
    // pix = j + i*GRIDL;

    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            if(rand() % (20 * PIXELS) == 0) {
                rgb[0] = toCol;
                rgb[1] = toCol;
                rgb[2] = toCol;
                setPixel(frame, i, j, getHex(rgb));
            }

            col = getPixel(frame, i, j);
            rgb = getRGB(col);

            // If it isn't b
            if(col > 0) {
                // Brighten this LED
                rgb[0] = fromRgb[0]+((toRgb[0]-fromRgb[0])*pow(progress,2));
                rgb[1] = fromRgb[1]+((toRgb[1]-fromRgb[1])*pow(progress,2));
                rgb[2] = fromRgb[2]+((toRgb[2]-fromRgb[2])*pow(progress,2));
            } else {
                // Turn random ones on every beat change
                if(rand() % (20 * PIXELS) == 0) {
                    printf("turn on led: %d\n",(i + j*GRIDL));
                    rgb[0] = 1;
                    rgb[2] = 1;
                    rgb[3] = 1;
                }
            }

            // Kill it if its reached full
            if(col >= toCol) {
                rgb[0] = 0;
                rgb[2] = 0;
                rgb[3] = 0;;
            }
            setPixel(frame, i, j, getHex(rgb));

        }
    }

    if(progress < 0.5) {

    } else {
        rgb[0] = toRgb[0]-((fromRgb[0]-toRgb[0])*(0.5-progress)*2);
        rgb[1] = toRgb[1]-((fromRgb[1]-toRgb[1])*(0.5-progress)*2);
        rgb[2] = toRgb[2]-((fromRgb[2]-toRgb[2])*(0.5-progress)*2);
    }
    // setPixel(frame, 0, 0, getHex(rgb));

    if(beat > oldBeat) {
        oldBeat = beat;
    }
    callOrder++;
    delete fromRgb;
    delete toRgb;
    delete rgb;
}
*/
