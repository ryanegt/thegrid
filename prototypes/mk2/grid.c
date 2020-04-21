// grid.c
#include <cstdio>
#include <unistd.h>
#include "engine.h"
#include "effects.h"

int main(void) {
    printf("Hello world\n");
    engineInit();
    renderGrid();
    usleep(1000000);

    colorFrame(0, new pixel({150,0,150,1.0}));
    gradientFrame(1, new pixel({255,255,5,1.0}));
    printf("B value is %d\n", frames[0][0]->b);
    renderGrid();
    usleep(1000000);

    
}
