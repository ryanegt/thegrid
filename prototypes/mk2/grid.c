// grid.c
#include "mysql.h"
#include "grid.h"
#include <fstream>

effect greenEffect = {.name="solidPulse",.beat=1,.duration=1,.frame=0,.color=65280};
effect blueEffect = {.name="solidLengthWipe",.beat=1,.duration=1,.frame=0,.color=120};
effect yellowEffect = {.name="twinklePulse",.beat=1,.duration=1,.frame=1,.color=150};
effect whiteEffect = {.name="choosePixels",.beat=1,.duration=32,.frequency=64,.frame=2,.color=16777215};

int main(int argc, char *argv[]) {
    // Stop everything
    printf("Grid init. Welcome...\n");
    gridInit();
    buildEffectMap();

    // Blackout grid for two seconds
    renderGrid();
    usleep(2000000);
    renderEffectFrame(greenEffect, 0);
    // Intro frame
    // solidColor(0,{150,0,150,1.0},0);
    // gradientFrame(1,{255,255,5,1.0},0);
    renderGrid();

    // Map effects and load show effects from DB

    printf("calling show %s\n",argv[1]);
    int showId = std::stoi(argv[1]);
    show master = getShow(showId);
    vector<vector<effect>> effects = getEffects(master);

    usleep(1000000);

    // Show timing. TODO move to engine or show
    int i = 0; // iterator
    effect e;
    struct timespec start, current;
    double elapsedTime, beatProgress;
    float beatDuration = (1/(master.bpm/60));
    float nextBeatTime = 0;

    // Play song
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("calling playSong at 0.000 for %s\n",master.audioFile.c_str());
    playSong((char *)master.audioFile.c_str());
    clock_gettime(CLOCK_MONOTONIC, &current);
    printf("done with playSong at %f\n",timeDiff(start, current));

    // Delay to first beat
    // float songFirstBeat = 0.24381;
    float lastBeatTime = 0;
    float startModifier = 0.02;
    usleep(startModifier * 1000000);
    printf("calling program loop at 0.00\n");
    clock_gettime(CLOCK_MONOTONIC, &start);
    beat = 0;
    // master.bpm = 50;

    // Main loop
    while(true) {
        // Calculate progress of the current beat
        clock_gettime(CLOCK_MONOTONIC, &current);
        elapsedTime = timeDiff(start, current);
        // printf("elapsed time:%f\n",elapsedTime);
        // Detect beat pulse, this method is immune to bpm changes unlike
        // the pure math model implemented at first.
        // if(elapsedTime >= nextBeatTime) {
        //     printf("beat:%d",beat);
        //     printf(", file:%f",beatTimes[beat]);
        //     printf(", elap:%f",elapsedTime);
        //     printf(", diff:%f\n",beatTimes[beat]-elapsedTime);
        // }

        // Bet can be timestamp based or deterministic
        if(master.beatTimes.size() > 0)
            nextBeatTime = master.beatTimes[0];

        // We are at the next beat. Do something.
        if(elapsedTime >= nextBeatTime) {
             // in seconds.
            printf("beat:%d, mod:%d, sig:%f, diff:%f\n",beat,(beat%4+1),lastBeatTime,(nextBeatTime-lastBeatTime));
            if((beat%4) == 3) {
                printf("measure %d\n", (beat/4)+1);
            }
            beatDuration = (1/(master.bpm/60)); // in case beat changes mid track
            lastBeatTime = nextBeatTime;
            beatPulse=true;

            if(master.beatTimes.size() > 0)
                master.beatTimes.erase(master.beatTimes.begin());
            else
                nextBeatTime = elapsedTime + beatDuration;
        }
        // At this moment what % of the beat has gone by
        beatProgress = 1-((nextBeatTime-elapsedTime)/beatDuration);
        // printf("beatProgress:%f\n", beatProgress);

        // Remove effects
        clearGrid();

        // Get effects for this beat
        if((int)effects.size() > 0) {
            // printf("somehow we got effects?\n");
            // printf("beatMod:%d allbeats:%d\n", beatMod, (int)effects[beatMod].size());
            for(int i=0; i < (int)effects[beat].size(); ++i) {
                // Call the effect by its name from our effect library.
                e = effects[beat][i];
                renderEffectFrame(e, beatProgress);
            }
        }
        // pulseBlue.reverse = 1;
        // if(beat%2==0 && beat>0) renderEffectFrame(greenEffect, beatProgress);
        // else renderEffectFrame(blueEffect, beatProgress);
        // renderEffectFrame(yellowEffect, beatProgress);
        // renderEffectFrame(redEffect, beatProgress);
        // solidLengthWipe(&(virtualFrames[2]),redEffect,beatProgress);
        // if(beat==0) effLib["solidColor"](0,{255,10,10,1.0},0);
        // effLib["gradientFrame"](0,{0,10,50,1.0},beatProgress);
        // effLib["twinklePulse"](2,{0,0,1,0.5},beatProgress);
        // effLib["solidPulse"](1,{255,255,5,0.2},beatProgress);
        // effLib["solidPulse"](2,{100,0,128,0.2},beatProgress);
        // printf("gradientLengthWipe succeeded %d\n", i);
        // gradientFrame(1, new pixel({255,255,5,1.0}),0);
        // gradientLengthWipe(2, new pixel({0,0,255,1.0}),beatProgress);
        // renderEffectFrame(whiteEffect, beatProgress);
        // twinklePulse(&(virtualFrames[1]),yellowEffect,beatProgress);

        for(int p=0; p<GRIDW*GRIDL; ++p) {
            if(p <= beat) {
                if(p%4==0) virtualFrames[2][p] = {255,0,0,0.5};
                else virtualFrames[2][p] = {155,155,155,0.5};
            }
        }
        renderGrid();

        if(!(i % 1000)) {
            printf("FPS: %lf",(i/elapsedTime));
            printf(", elapsed:%f",elapsedTime);
            printf(", nxtbeat:%f",nextBeatTime);
            printf(", prog:%f",beatProgress);
            printf(", meas:%d", (beat/4));
            printf(", elap:%f\n",elapsedTime);
        }

        // Detect keypress
        char ch;
        if(kbhit()) {
            ch = getch();
            printf("pressed: %d",ch);
            clearGrid();
            renderGrid();
            system("mpc stop");
            break;
        }
        if(beatPulse) {
            beatPulse=false;
            beat++;
        }

        // So we don't saturate a processor core
        usleep(1000);
        i++;
    }
}
