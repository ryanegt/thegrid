// I have no idea what I'm doing
#define GRIDW 5
#define GRIDL 150
#define PIXELS 750
#define FRAME_RATE 3
#include <string>
#include <cstdint>
typedef struct pixel {
    int r;
    int g;
    int b;
    float a;
} pixel;
typedef struct effect {
    std::string name;
    int id;
    int frCol;
    int toCol;
    int beat;
    int duration;
    int frame;
} effect;
typedef void (*FnPtr)(pixel *, float, long, long);
unsigned int wheel(int i);
void clearFrame(pixel *f);
void rainbowFrame(pixel *f);
void colorFrame(pixel *f, float progress, long col1, long col2);
void renderLengthWipe(pixel *frame, float progress, long col1, long col2);
void renderCenterWipe(pixel *frame, float progress, int frCol, int toCol);
void renderFadeColor(pixel *frame, float progress, int frCol, int toCol);
void renderExpFadeColor(pixel *frame, float progress, int frCol, int toCol);
void renderSparkle(pixel *frame, float progress, int frCol, int toCol);
void renderOneSparkle(pixel *frame, float progress, int frCol, int toCol);
pixel blendPixelColor(pixel bottom, pixel top);
int * getRgbaFromLong(unsigned long col);
int * getRgbFromLong(unsigned long col);
long getLongFromRgba(int *rgb);
int getIntFromRgb(int *rgb);
long getRgba(int r, int g, int b, int a);
using namespace std;
extern pixel * frames[][GRIDW*GRIDL];
int getPixelColor(unsigned long *f, int x, int y);
void setPixelColor(unsigned long *f, int x, int y, long col);
effect* getShow(void);
extern int showSize;
extern long start; // start time
extern int beat; // current beat
extern int callOrder; // effect call number per iteration
extern float bpm;
