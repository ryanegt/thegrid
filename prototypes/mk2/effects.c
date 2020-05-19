// effects.c
#include "effects.h"
#include <cstdio>
#include <math.h>
#include <algorithm>
using namespace std;
std::map<std::string, FnPtr> effLib; // Effect library map

/*
* Set a pixel to a color based on x, y coords. TODO rename
* Grid vs. grid to frame?
*/
void setFramePixel(int f, int x, int y, color col) {
    int p = x + y*GRIDL;
    virtualFrames[f][p] = col;
    return;
}

/*
* Call the effect. Effects return frames. Which we can
* additionally transform before adding it to the virtual frame.
*/
double lastEffProg;
void renderEffectFrame(effect e, double beatProg) {
    frame canvas = {{0,0,0,0}};
    double progress = beatProg;
    // manipulate the progress of the effect, this allows
    // the effect to play over multiple beats. beat is global.
    if(e.duration > 1) {
        progress = ((float)(beat-e.beat)/e.duration)+(beatProg/e.duration);
    }
    // play effect multiple times
    if(e.frequency > 1) {
        progress = (float)((int)(progress*1000*e.frequency)%1000)/1000;
    }

    // Draw effect onto canvas
    // printf("calling effect:%s with prog:%f\n", e.name.c_str(),progress);
    if(!e.color) {
        e.color = colorWheel(beat); // -------- FOR TESTING REMOVE THIS -------- //
    }
    effLib[e.name](&canvas, e, {0,beatProg,progress,(progress < lastEffProg)});

    // Put canvas onto correct virtual frame
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        int q = p;
        if(e.reverse) {
            q = (GRIDW*GRIDL)-p;
        }
        virtualFrames[e.frame][q] = canvas[p];
    }
    // Track progress
    lastEffProg = progress;
}

/*
* Set the pixels of a frame to the same color
*/
void solidColor(frame * fr, effect e, player p) {
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        (*fr)[p] = hexToColor(e.color);
    }
}

/*
* Gradient frame
*/
void gradientFrame(frame * fr, effect e, player p) {
    color col = hexToColor(e.color);
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            // Modulate alpha along the length
            col.a = (float)i/GRIDL;
            (*fr)[(i+j*GRIDL)] = col;
        }
    }
    return;
}

/*
* Solid lengthwise wipe. Overlay a solid block
* color that moves with progress
*/
void solidLengthWipe(frame * fr, effect e, player p) {
    int barLen = 20; // how many pixels long is the wipe
    int fromLed = (GRIDL+barLen)*p.effProg - 1;
    int toLed = fromLed-barLen;
    color col = hexToColor(e.color);
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            if(i <= fromLed && i > toLed && i >= 0 && i < GRIDL) {
                // Transparency/dither
                // col.a = ((float)(i-toLed)/(float)barLen);
                // printf("bar alpha i:%d a:%f\n", i, color->a);
                (*fr)[(i+j*GRIDL)] = col;
            }
        }
    }
    return;
}

/*
* Gradient lengthwise wipe. Overlay a gradient bar
* color to this frame that moves with progress.
*/
void gradientLengthWipe(frame * fr, effect e, player p) {
    int barLen = 100; // how many pixels long is the wipe
    int fromLed = (GRIDL+barLen)*p.effProg - 1;
    int toLed = fromLed-barLen;
    color col = hexToColor(e.color);
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            if(i <= fromLed && i > toLed && i >= 0 && i < GRIDL) {
                // Transparency/dither
                col.a = ((float)(i-toLed)/(float)barLen);
                // printf("bar alpha i:%d a:%f\n", i, color->a);
                (*fr)[(i+j*GRIDL)] = col;
            }
        }
    }
    return;
}

/*
* Gradient wipe from center
*/
void gradientCenterWipe(frame * fr, effect e, player p) {
    int length = 60;
    double prog = p.effProg;
    int first = (GRIDL/2)-(prog*(GRIDL/2+length));
    int last = (GRIDL/2)+(prog*(GRIDL/2+length));
    color col = hexToColor(e.color);
    for(int j=0; j < GRIDW; ++j) {
        for(int i=(length*-1); i < (GRIDL+length); ++i) {
            if(i > first && i < (first+length) && i<(GRIDL/2)+1 && i>=0) {
                col.a = 1-((float)(1-(first-i))/(float)length);
                (*fr)[(i+j*GRIDL)] = col;
            }
            if(i < last && i>(last-length) && i>(GRIDL/2)-1 && i < GRIDL) {
                col.a = 1-((float)(last-i)/(float)length);
                (*fr)[(i+j*GRIDL)] = col;
            }
        }
    }
    return;
}

/*
* Gradient wipe from center
*/
void gradientConcentricWipe(int f, color col, player p) {
    int length = 50;
    int firstL = (GRIDL/2)-(p.effProg*(GRIDL/2+length));
    int lastL = (GRIDL/2)+(p.effProg*(GRIDL/2+length));
    for(int j=0; j < GRIDW; ++j) {
        for(int i=(length*-1); i < (GRIDL+length); ++i) {
            if(i > firstL && i < (firstL+length) && i<(GRIDL/2)+1 && i>=0) {
                col.a = 1-((float)(1-(firstL-i))/(float)length);
                setFramePixel(f, i, j, col);
            }
            if(i < lastL && i>(lastL-length) && i>(GRIDL/2)-1 && i < GRIDL) {
                col.a = 1-((float)(lastL-i)/(float)length);
                setFramePixel(f, i, j, col);
            }
        }
    }
    return;
}

/*
* Fade Color/fade out
*/
void solidFade(frame * fr, effect e, player p) {
    color col = hexToColor(e.color);
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            col.a = 1-p.effProg;
            (*fr)[p] = col;
        }
    }
    return;
}

/*
* Pulse Color, pulse out
*/
void solidPulse(frame * fr, effect e, player p) {
    color col = hexToColor(e.color);
    double prog = p.effProg;
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        col.a = 1-pow(prog,3);
        (*fr)[p] = col;
    }
    return;
}

/*
* Wipe blocks. TODO: gradientBlockWipe
*/
void blockLenghtWipe(frame * fr, effect e, player p) {
    int blockSize = 30;
    double prog = p.effProg;
    color col = hexToColor(e.color);
    int blockCount = floor(GRIDL/blockSize);
    int currentBlock = ceil((prog*100)/(100/blockCount));
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            if(i < currentBlock*blockSize && i > currentBlock*blockSize-blockSize) {
                (*fr)[(i+j*GRIDL)] = col;
            }
        }
    }
    return;
}

/*
* Coming soon. Todo implement random colors and brightness wipes
*/
void gradientWidthWipe() {}


/*
* Coming soon. Todo implement random selection of
*/
int * lastBlocks;
void chooseBlocks(int f, color col, float progress) {}
void chooseSquares() {}
void chooseRows() {}

/*
* Choose random strips
*/
int lastStrips[VFRAMES][GRIDW];
void chooseStrips(frame * fr, effect e, player p) {
    int density = 20;
    color col = hexToColor(e.color);
    if(p.pulse) {
        for(int x=0; x<GRIDW; ++x) {
            lastStrips[e.frame][x] = 0;
            if(rand() % 100 < density) {
                // This is the progress that each one starts at
                lastStrips[e.frame][x] = 1;
            }
        }
    }
    for(int j = 0; j < GRIDW; ++j) {
        for(int i = 0; i < GRIDL; ++i) {
            if(lastStrips[e.frame][j]) {
                (*fr)[(i+j*GRIDL)] = col;
            }
        }
    }
}

/*
* Choose random pixels
*/
frame lastPixels[VFRAMES];
void choosePixels(frame * fr, effect e, player p) {
    int density = 30;
    color col = hexToColor(e.color);
    if(p.pulse) {
        for(int p=0; p<GRIDW*GRIDL; ++p) {
            lastPixels[e.frame][p] = {0,0,0,0};
            if(rand() % 100 < density) {
                // This is the progress that each one starts at
                lastPixels[e.frame][p] = col;
            }
        }
    }
    for(int p = 0; p < GRIDW*GRIDL; ++p) {
        (*fr)[p] = lastPixels[e.frame][p];
    }
}


/*
* Pulse stars/twinkle, pulse out
* TODO: make this a pick-some pattern
*/
float twinkleInit[GRIDW*GRIDL];
float lastTwinkleInit[GRIDW*GRIDL];
void twinklePulse(frame * fr, effect e, player p) {
    int twinklePerBeat = 1;
    float lastProg;
    float phase = twinklePerBeat*2*PI;
    color col = hexToColor(e.color);
    double prog = p.effProg;
    // Each beat will get a twinkle init
    if(beatPulse) {
        copy(begin(twinkleInit),end(twinkleInit),begin(lastTwinkleInit));
        for(int p=0; p<GRIDW*GRIDL; ++p) {
            twinkleInit[p] = 0;
            if(rand() % 100 < 40 && lastTwinkleInit[p] == 0) {
                // This is the progress that each one starts at
                twinkleInit[p] = ((double)rand()/(RAND_MAX));
            }
        }
    }
    for(int p=0; p<GRIDW*GRIDL; ++p) {
        // Shifts a sine function to modulate bw 0-1
        if(twinkleInit[p] > 0 && twinkleInit[p] < prog) {
            col.a = (float)(sin((phase*prog)-(PI/2+(phase*twinkleInit[p])))/2)+0.5;
            (*fr)[p] = col;
        }
        // Allow twinkles from the last frame to finish
        lastProg = lastTwinkleInit[p]-prog;
        if(lastProg > 0) {
            col.a = (float)(sin((phase*prog)-(PI/2+(phase*lastTwinkleInit[p])))/2)+0.5;
            (*fr)[p] = col;
        }
    }
    return;
}

/*
* Map function calls to effect library so they can be
* used as a reflection/factory pattern later.
*/
void buildEffectMap(void) {
    effLib["solidColor"] = solidColor;
    effLib["gradientFrame"] = gradientFrame;
    effLib["solidLengthWipe"] = solidLengthWipe;
    effLib["gradientLengthWipe"] = gradientLengthWipe;
    effLib["gradientCenterWipe"] = gradientCenterWipe;
    effLib["blockLenghtWipe"] = blockLenghtWipe;
    // effLib["solidFade"] = solidFade;
    effLib["solidPulse"] = solidPulse;
    effLib["twinklePulse"] = twinklePulse;
    effLib["choosePixels"] = choosePixels;
    effLib["chooseStrips"] = chooseStrips;
    // effLib["gradientFade"] = gradientFade;
    // effLib["gradientPulse"] = gradientPulse;
    return;
}

int colorWheel(int i) {
    int index;
    int cols[10] = {
        0xEA2027,0xEE5A24,0xF79F1F,
        0xFFC312,0xC4E538,0xA3CB38,
        0x009432,0x0652DD,0x1B1464,
        0x6F1E51
    };
    index = i % (sizeof(cols)/sizeof(cols[0]));
    return cols[index];
}
