// effects.c

void colorFrame(pixel *f, float progress, pixel px) {
    for(int j=0; j < GRIDW; ++j) {
        for(int i=0; i < GRIDL; ++i) {
            setPixelColor(f, i, j, col1);
        }
    }
    return;
}
