#include <stdio.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "imgio.h"
#include "list.h"
#include "object.h"
#include "transform.h"

void RenderObject(Camera *camera, Object* object);

int main(int argc, char *argv[]) {
    // srand(time(NULL));

    Texture *texture = CreateImage(1080,720);

    Camera cam = { texture, (Transform){ 0, 0, 0, 0, 0, 0 }, 90 };
    
    Object* cube = LoadObjFile("cube.obj");
    cube->transform = (Transform){0, 0, 3, .5, -.5, 0};

    RenderObject(&cam, cube);
    SaveBMP(texture);

    FreeImage(texture);
    FreeObject(cube);
    
    return 0;
}

void RenderObject(Camera* camera, Object* object) {
    for (int i = 0; i < object->triCount; i++) {
        Pixel TriCol = {randInt(0,255),randInt(0,255),randInt(0,255)};
        float2 b = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[1]));
        float2 a = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[0]));
        float2 c = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[2]));

        float minX = clamp(fminf(fminf(a.x, b.x), c.x), 0, camera->target->width);
        float minY = clamp(fminf(fminf(a.y, b.y), c.y), 0, camera->target->height);
        float maxX = clamp(fmaxf(fmaxf(a.x, b.x), c.x), 0, camera->target->width);
        float maxY = clamp(fmaxf(fmaxf(a.y, b.y), c.y), 0, camera->target->height);
        
        for (int x = floor(minX); x < ceil(maxX); x++)
        for (int y = floor(minY); y < ceil(maxY); y++) {
            float2 p = { (float)x, (float)y };
            if (!PointInTriangle(p, a, b, c)) continue;
            camera->target->image[x][y] = TriCol;
        }
    }
}
