#include <stdio.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "imgio.h"
#include "list.h"
#include "object.h"
#include "transform.h"

void RenderObject(Texture *texture, Object* object);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    Texture *texture = CreateImage(1080,720);
    
    Object* cube = LoadObjFile("cube.obj");
    cube->transform = (Transform){0, 0, 0, 0.5, 1, 0};

    RenderObject(texture, cube);
    SaveBMP(texture);

    FreeImage(texture);
    FreeObject(cube);
    
    return 0;
}

void RenderObject(Texture* texture, Object* object) {
    for (int i = 0; i < object->triCount; i++) {
        Pixel TriCol = {randInt(0,255),randInt(0,255),randInt(0,255)};
        float2 a = WorldToScreen(NULL, LocalToWorld(object->transform, object->tris[i].vertex[0]), texture->width, texture->height);
        float2 b = WorldToScreen(NULL, LocalToWorld(object->transform, object->tris[i].vertex[1]), texture->width, texture->height);
        float2 c = WorldToScreen(NULL, LocalToWorld(object->transform, object->tris[i].vertex[2]), texture->width, texture->height);

        float minX = clamp(fminf(fminf(a.x, b.x), c.x), 0, texture->width);
        float minY = clamp(fminf(fminf(a.y, b.y), c.y), 0, texture->height);
        float maxX = clamp(fmaxf(fmaxf(a.x, b.x), c.x), 0, texture->width);
        float maxY = clamp(fmaxf(fmaxf(a.y, b.y), c.y), 0, texture->height);
        
        for (int x = floor(minX); x < ceil(maxX); x++)
        for (int y = floor(minY); y < ceil(maxY); y++) {
            float2 p = { (float)x, (float)y };
            if (!PointInTriangle(p, a, b, c)) continue;
            texture->image[x][y] = TriCol;
        }
    }
}
