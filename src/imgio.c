#include "imgio.h"

void SaveBMP(Texture *texture, char* fileName) {
    FILE *fptr = fopen(fileName, "w+");
    
    char tag[] = { 'B', 'M' };
    uint32_t header[] = {
        14 + 40 + texture->width * texture->height * 4,
        0,
        14 + 40,
        40, 
        texture->width,
        texture->height,
        1 | 8*4 << 16,
        0,
        texture->width * texture->height * 4,
        0, 0, 0, 0
    };

    fwrite(&tag, sizeof(tag), 1, fptr);
    fwrite(&header, sizeof(header), 1, fptr);

    for (int y = 0; y < texture->height; y++)
        for (int x = 0; x < texture->width; x++) {
            uint8_t pixel[] = {
                texture->image[x][y].blue,
                texture->image[x][y].green,
                texture->image[x][y].red,
                0
            };
            fwrite(&pixel, sizeof(pixel), 1, fptr);
        }

    fclose(fptr);
}

void TestImage(Texture *texture) {
    for (int x = 0; x < texture->width; x++)
        for (int y = 0; y < texture->height; y++) {
            texture->image[x][y].red = x * 256 / texture->width;
            texture->image[x][y].green = y * 256 / texture->height;
            texture->image[x][y].blue = 0;
        }
}

Texture* CreateImage(int width, int height) {
    Texture *texture = malloc(sizeof(Texture));
    texture->width = width;
    texture->height = height;
    texture->image = malloc(sizeof(Pixel*) * width);
    for (int i = 0; i < width; i++)
        texture->image[i] = malloc(sizeof(Pixel) * height);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            texture->image[x][y].red = 0;
            texture->image[x][y].green = 0;
            texture->image[x][y].blue = 0;
        }
    return texture;
}

void FreeImage(Texture *texture) {
    for (int i = 0; i < texture->width; i++)
        free(texture->image[i]);
    free(texture->image);
    free(texture);
}
