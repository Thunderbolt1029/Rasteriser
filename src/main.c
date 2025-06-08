#include <stdio.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "imgio.h"
#include "list.h"
#include "object.h"
#include "transform.h"

void RenderObject(Camera *camera, Object* object);
void DepthVisualisation(Camera *cam);

int main(int argc, char *argv[]) {
    // srand(time(NULL));
    Camera *cam = CreateCamera(1080, 720, 90, 1000);

    Object* cube = LoadObjFile("cube.obj");
    cube->transform = (Transform){0, 0, 3, .5, -.5, 0};

    Object* monkey = LoadObjFile("monkey.obj");
    monkey->transform = (Transform){0, 0, 2, .3, PI + .3, 0};

    RenderObject(cam, monkey);
    SaveBMP(cam->target, "out.bmp");

    DepthVisualisation(cam);

    DestroyCamera(cam);
    FreeObject(cube);
    FreeObject(monkey);
    
    return 0;
}

void RenderObject(Camera* camera, Object* object) {
    for (int i = 0; i < object->triCount; i++) {
        Pixel TriCol = {randInt(0,255),randInt(0,255),randInt(0,255)};
        float2 a = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[0]));
        float2 b = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[1]));
        float2 c = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[2]));

        float minX = clamp(fminf(fminf(a.x, b.x), c.x), 0, camera->target->width);
        float minY = clamp(fminf(fminf(a.y, b.y), c.y), 0, camera->target->height);
        float maxX = clamp(fmaxf(fmaxf(a.x, b.x), c.x), 0, camera->target->width);
        float maxY = clamp(fmaxf(fmaxf(a.y, b.y), c.y), 0, camera->target->height);
        
        for (int x = floor(minX); x < ceil(maxX); x++)
        for (int y = floor(minY); y < ceil(maxY); y++) {
            float3 weights;
            float2 p = { (float)x, (float)y };
            if (!PointInTriangle(p, a, b, c, &weights)) continue;

            float aDepth = WorldToLocal(camera->transform, LocalToWorld(object->transform, object->tris[i].vertex[0])).z;
            float bDepth = WorldToLocal(camera->transform, LocalToWorld(object->transform, object->tris[i].vertex[1])).z;
            float cDepth = WorldToLocal(camera->transform, LocalToWorld(object->transform, object->tris[i].vertex[2])).z;

            float depth = aDepth * weights.x + bDepth * weights.y + cDepth * weights.z;
            if (depth < 0 || camera->depth[x][y] < depth) continue;

            camera->target->image[x][y] = TriCol;
            camera->depth[x][y] = depth;
        }
    }
}

void DepthVisualisation(Camera *cam) {
    float min = cam->maxDistance, max = 0;
    for (int x = 0; x < 1080; x++)
    for (int y = 0; y < 720; y++) {
        if (cam->depth[x][y] < min) min = cam->depth[x][y];
        if (cam->depth[x][y] > max && cam->depth[x][y] < cam->maxDistance) max = cam->depth[x][y];
    }
    Texture* texture = CreateImage(1080, 720);
    for (int x = 0; x < 1080; x++)
    for (int y = 0; y < 720; y++) {
        float col;
        if (cam->depth[x][y] == cam->maxDistance) col = 0.2;
        else col = (cam->depth[x][y] - min) / (max - min);
        texture->image[x][y] = (Pixel){ col * 255, col * 255, col * 255 };
    }
    SaveBMP(texture, "depth.bmp");
    FreeImage(texture);
}
