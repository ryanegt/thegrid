#include "grid.h"
void randomStrips(int num) {
    int lines[num];
    int colors[num];
    for(int x=0; x < num; ++x) {
        lines[x] = rand() % stripNum;
        colors[x] = wheel(rand() % 99);
        for(int i=0; i<strips[1]->count(); ++i){
            strips[lines[x]]->SetPixel(i, colors[x]);
        }
    }
}

void renderCenterWipe(float progress, unsigned int col) {
    int length = 50;
    int first = (ledNum/2)-(progress*(ledNum/2+length));
    int last = (ledNum/2)+(progress*(ledNum/2+length));
    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=(length*-1); i < (ledNum+length); ++i) {
            if(i > first && i < (first+length) && i<(ledNum/2)+1 && i>=0)
                strips[j]->SetPixel(i, col);

            if(i < last && i>(last-length) && i>(ledNum/2)-1 && i < ledNum)
                strips[j]->SetPixel(i, col);
        }
    }
}

void renderLengthWipe(float progress, unsigned int col) {
    int length = 50;
    int first = (ledNum+length)*progress-1;
    int last = first-length;

    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[j]->count(); ++i) {
            if(i <= first && i > last && i >= 0 && i < ledNum)
                strips[j]->SetPixel(i, col);
        }
    }
}

void renderWidthWipe(float progress, unsigned int col) {
    int length = 2;
    int first = floor(stripNum*progress);

    for(int i=0; i < strips[1]->count(); ++i) {
        for(int j=0; j <= stripNum; ++j) {
            if(j <= first && j > first-length && j >= 0) {
                strips[j]->SetPixel(i, col);
                // printf("light up row  %d\n",j);
            }
        }
    }
}

void renderBlockWipe(float progress, unsigned int col) {
    int blockNum = 5;
    int blockLen = (strips[0]->count()/blockNum);
    int block = floor(progress*strips[0]->count()/blockLen);
    // printf("--- block: %d\n",block);
    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[j]->count(); ++i) {
            int first = block * blockLen;
            if(i >= first && i < first+blockLen && i >= 0 && i < strips[j]->count())
                strips[j]->SetPixel(i, col);
        }
    }
}

void renderSolid(unsigned int col) {
    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[j]->count(); ++i) {
            strips[j]->SetPixel(i, col);
        }
    }
}

void brightenStrip(float progress) {
    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[j]->count(); ++i) {
            strips[j]->SetBrightness((progress)*255+10);
        }
    }
}

void fadeStrip(float progress) {
    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[j]->count(); ++i) {
            strips[j]->SetBrightness((1-progress)*255+20);
        }
    }
}

void twinkle(float progress, unsigned int col) {
    for(unsigned int j=0; j < stripNum; ++j) {
        for(int i=0; i < strips[j]->count(); ++i) {
            if(rand() % 4 == 0 && progress > 0.5) {
                strips[j]->SetPixel(i, col);
            }
        }
    }
}

void clearGrid(Grid grid) {
    for(unsigned int j=0; j < grid.stripNum; ++j) {
        for(int i=0; i < grid.strips[1]->count(); ++i) {
            grid.strips[j]->SetPixel(i, 0x000000);
        }
    }
}
