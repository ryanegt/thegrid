// effects.h
// Effect Construct
#include "engine.h"
#include <string>
#include <map>

// Pass player and timing attribs to render funcs
typedef struct player {
    int beat;
    double beatProg;
    double effProg;
    bool pulse;
} player;

// Pass effect attribs to display funcs
typedef struct effect {
    std::string name;
    int id;
    int beat;
    int measure;
    double elapsed;
    int duration;  // how many beats, measures or seconds
    int frequency; // howtimes to repeat within a beat or measure
    int frame;
    int color;
    int reverse;
} effect;

// Forward declarations
int colorWheel(int);
void renderEffectFrame(effect, double);
void setFramePixel(int, int, int, color);
typedef void (*FnPtr)(frame *, effect, player);
extern std::map<std::string, FnPtr> effLib;

void solidColor(frame *, effect, player);
void solidPulse(frame *, effect, player);
void solidFade(frame *, effect, player);
void twinklePulse(frame *, effect, player);
void gradientFrame(frame *, effect, player);
void solidLengthWipe(frame *, effect, player);
void blockLenghtWipe(frame *, effect, player);
void gradientLengthWipe(frame *, effect, player);
void gradientCenterWipe(frame *, effect, player);
void choosePixels(frame *, effect, player);
void chooseStrips(frame *, effect, player);
// effect * getShow(int);
