// grid.c
#include <cstdio>
#include "engine.h"
#include "effects.h";

int main(void) {
    printf("Hello world\n");
    engineInit();
    printf("B value is %d\n", frames[0][0]->b);
}
