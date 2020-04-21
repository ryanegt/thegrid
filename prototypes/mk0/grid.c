/*
use ffmpeg to make a short clip:
ffmpeg -i ./odesza_loyal.mp3 -ss 30 -to 60 -ac 1 -ab 128k odesza_loyal_sample.mp3

beat analysis:
python3 ~/pyAudioAnalysis/pyAudioAnalysis/audioAnalysis.py beatExtraction -i /home/ryan/songs/odesza_loyal_mono.mp3
*/
#include "grid.h"
#include "led-strip.h"
#include "mysql_connection.h"
#include <cppconn/resultset.h>
#include <sys/time.h>
#include <unistd.h>  // for usleep()
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <cstdint>

/*
* Virtual frames. We draw to these.
* Background frame[0]
* Primary effect frame[1]
* Secondary effect frame[2]
*/
pixel * frames[3][GRIDW*GRIDL];

int showSize = 0; // num of db effects in show.
using namespace spixels;
using namespace std;
LEDStrip *strips[5];
MultiSPI *spi;
effect *show;
long start; // start time
float bpm = 0; // current bpm. can change now
int beat = 0; // current beat
int callOrder;

void init() {
    // Things we do once befoe the program loop
    // Connect LED strips with 144 LEDs to connector
    spi = CreateDirectMultiSPI();
    strips[0] = CreateAPA102Strip(spi, MultiSPI::SPI_P2, 150);
    strips[1] = CreateAPA102Strip(spi, MultiSPI::SPI_P3, 150);
    strips[2] = CreateAPA102Strip(spi, MultiSPI::SPI_P4, 150);
    strips[3] = CreateAPA102Strip(spi, MultiSPI::SPI_P9, 150);
    strips[4] = CreateAPA102Strip(spi, MultiSPI::SPI_P6, 150);

    // Init frames to transparent black
    for(int f=0; f < 3; ++f) {
        for(int p=0; p < GRIDW*GRIDL; ++p) {
            *frames[f][p] = {0,0,0,0};
        }
    }
}

/*
* Map function calls to effect library so they can be
* used as a reflection/factory pattern later.
*/
std::map<std::string, FnPtr> effLib;
void buildEffectMap() {
    effLib["colorFrame"] = colorFrame;
    effLib["renderLengthWipe"] = renderLengthWipe;
    // effLib["renderCenterWipe"] = renderCenterWipe;
    // effLib["renderFadeColor"] = renderFadeColor;
    // effLib["renderExpFadeColor"] = renderExpFadeColor;
    return;
}

/*
* Call command line mpd/mpc to play song
* TODO: measure start time/response
*/
void playSong(char * fileName) {
    printf("Starting music...\n");
    std::string addStr = "mpc add " + std::string(fileName);
    system("mpc clear");
    system(addStr.c_str());
    system("mpc play");
    return;
}

/*
* Combine the virtul frames for final output to leds
*/
void renderFrame() {
    int px;
    int col = 0;
    pixel * outFrame = new pixel[GRIDW*GRIDL];
    printf("Color of outframe[0] is: %du\n",outFrame[0].r);
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            // Interpolate pixels in our virtual frames
            for(int f=0; f < 3; ++f) {
                outFrame[px] = blendPixelColor(outFrame[px], frames[f][px]);
            }
            col = getIntFromRgb(getRgbFromLong(outFrame[px]));
            printf("New color for pixel %du is: %du\n",px, col);
            strips[j]->SetPixel(i, col);
            px++;
        }
    }
    spi->SendBuffers();
    return;
}

int main() {
    printf("Initiating show...");
    init();
    buildEffectMap();
    show = getShow();
    printf("First effects: ");
    cout << show[0].name << endl;
    printf("Show contains %d effects.\n",showSize);
    printf("Color of frames[0][0] is: %du\n",frames[0][0]);
    colorFrame(frames[0], 0, 0xFFFFFFFF, 0x000000);
    renderFrame();

    /*
    clearFrame(frame[0]);
    renderFrame();
    usleep(1000000);

    rainbowFrame(frame[0]);
    renderFrame();
    usleep(500000);
    clearFrame(frame[0]);
    renderFrame();

    // Play music
    playSong((char *)"odesza_loyal.mp3");

    // Main loop
    bpm = 140;  // bpm
    long i = 0;
    long* rgb;

    struct timeval tv;
    float elapsedTime, beatProgress;
    float beatDuration = 0;
    float nextBeatTime = 0;
    // int beatPulse;

    gettimeofday(&tv, NULL);
    start = 1000000*tv.tv_sec+tv.tv_usec;
    printf("Current microsec: %lu\n",start);
    printf("Starting program loop \n");

    // Start loop
    while(true) {
        // Calculate progress of the current beat
        // beatPulse = 0;
        gettimeofday(&tv, NULL);
        elapsedTime = ((float)((1000000*tv.tv_sec+tv.tv_usec) - start)/1000000);

        // Detect beat pulse, this method is immune to bpm changes unlike
        // the pure math model implemented at first.
        if(elapsedTime >= nextBeatTime) {
            beatDuration = (1/(bpm/60)); // in seconds. bpm could change mid track
            nextBeatTime = elapsedTime + beatDuration;
            // beatPulse = 1;
            beat++;
        }
        beatProgress = 1-((nextBeatTime-elapsedTime)/beatDuration);
        // renderCenterWipe(frame, beatProgress, wheel(beat));
        // colorFrame(frame, 15105570);
        //if(beat%2==0)
        // renderFadeColor(frame, beatProgress, wheel(beat), wheel(beat+1));
        //else
        //    renderFadeColor(frame, beatProgress, wheel(beat-1),0x000000);
        // effLib["renderCenterWipe"](frame, beatProgress, wheel(beat),  wheel(beat+1));
        colorFrame(frame, 0x500000);
        effect ef = show[beat%showSize];
        effLib["renderExpFadeColor"](frame, beatProgress, 0x000000,  0xFFFFFF);
        // effLib[ef.name](frame, beatProgress, ef.frCol, ef.toCol);
        // setPixel(frame, 2, 0, 0x110000);
        // colorFrame(frame, wheel(beat));
        // renderFrame();
        // renderOneSparkle(frame, beatProgress, 0xffffff, 0xdd8800);
        // renderOneSparkle(frame, beatProgress, 0xffffff, 0xffffff);
        // colorFrame(frame, 0x100010);
        // frame[beat%PIXELS] = 0xFF4422;
        // for(int j=0; j < GRIDW; ++j) {
        //     for(int i=0; i < GRIDL; ++i) {
        //         colorFrame(frame, 0x100010);
        //         setPixel(frame, i, j, 0xFF4422);
        //         renderFrame();
        //         usleep(100000);
        //     }
        // }
        renderFrame();

        i++;
        callOrder = 0;
        if(i%300 == 0) {
            printf("FPS: %lf",(i/elapsedTime));
            printf(", color at 1:%d",frame[0]);
            rgb = getRgbaFromInt(frame[0][0]);
            printf(", r:%ld",rgb[1]);
            printf(", g:%ld",rgb[2]);
            printf(", b:%ld",rgb[3]);
            printf(", prog:%f",beatProgress);
            printf(", beat:%d", beat);
            printf(", elap:%f\n",elapsedTime);
        }
        // So we don't saturate a processor core
        usleep(2000);
    }
    */
    return(0);
}
