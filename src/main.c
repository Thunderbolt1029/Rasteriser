#include "imgio.h"

int main(int argc, char *argv[]) {
    Texture *texture = CreateImage(1280,720);

    TestImage(texture);
    SaveBMP(texture);

    FreeImage(texture);
    
    return 0;
}