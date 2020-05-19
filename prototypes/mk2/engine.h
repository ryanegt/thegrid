// engine.h
#include "led-strip.h"
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>
#include <unistd.h>

using namespace spixels;
using namespace std;

// Length and width of LED grid
#define GRIDW 8
#define GRIDL 150
#define VFRAMES 3
const float PI = 3.1415927;
//#define TIME(a,b) ((a*1000000ull) + b)

// A pixel is just a holder for an rgba value
typedef struct color {
    int r;
    int g;
    int b;
    float a;
} color;

// Details of the song choice
typedef struct show {
    int id;
    float bpm;
    string name;
    string audioFile;
    vector<float> beatTimes;
    int signature;
} show;

// The virtual frame stack
typedef color frame[GRIDW*GRIDL];
extern frame virtualFrames[];
extern show master;

// Timing variables
extern uint64_t startTime;
extern volatile bool beatPulse;
extern volatile int beat;

// Forward function declarations
void gridInit(void);
void showInit(void);
void renderGrid(void);
void clearGrid(void);
void blendPixelColor(color *, color *);
int colorToHex(color);
color hexToColor(int);
int pixelIntensity(color *);
void buildEffectMap(void);
double timeDiff(const struct timespec&, const struct timespec&);
void playSong(char *);
bool kbhit(void);
