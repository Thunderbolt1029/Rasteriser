#ifndef IMGIO_H
#define IMGIO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;
typedef struct {
    int width;
    int height;
    Pixel** image;
} Texture;

int SaveBMP(Texture *texture, char* fileName);
void TestImage(Texture *texture);
Texture* CreateImage(int width, int height);
void FreeImage(Texture *texture);

#endif
