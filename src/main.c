#include <stdio.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "imgio.h"
#include "list.h"
#include "object.h"
#include "transform.h"

void RenderObject(Camera *camera, Object* object);
void DepthVisualisation(Camera *cam, char* fileName);
Pixel PixelColour(Object* object, float2 texture, float3 normal);

int main(int argc, char *argv[]) {
    // srand(time(NULL));
    Camera* cam = CreateCamera(1080, 720, 90, 1000);

    printf("Loading object files...\n");

    Object* cube = LoadObjFile("assets/cube.obj");
    cube->transform = (Transform){0, 0, 3, .5, -.5, 0};
    cube->texture = CreateImage(64, 64);
    for (int x = 0; x < 64; x++)
    for (int y = 0; y < 64; y++)
        cube->texture->image[x][y] = (x + y) % 2 ? (Pixel){255,0,255} : (Pixel){50,50,50};

    Object* monkey = LoadObjFile("assets/monkey.obj");
    monkey->transform = (Transform){0, 0, 2, .3, PI + .3, 0};
    
    Object* dragon = LoadObjFile("assets/dragon.obj");
    dragon->transform = (Transform){0, 0, .6, 0, PI / 2, 0};

    printf("Loaded object files!\n");

    printf("Rendering scene...\n");
    RenderObject(cam, cube);

    printf("Saving rendered output...\n");
    int ret = SaveBMP(cam->target, "output/out.bmp");
    if (ret == -1) printf("FAILED TO SAVE RENDER\n");

    printf("Cleaning up...\n");
    DestroyCamera(cam);
    FreeObject(cube);
    FreeObject(monkey);
    FreeObject(dragon);

    printf("Done!\n");
    return 0;
}

void RenderObject(Camera* camera, Object* object) {
    for (int i = 0; i < object->triCount; i++) {
        float3 a = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[0]));
        float3 b = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[1]));
        float3 c = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[2]));

        float minX = clamp(fminf(fminf(a.x, b.x), c.x), 0, camera->target->width);
        float minY = clamp(fminf(fminf(a.y, b.y), c.y), 0, camera->target->height);
        float maxX = clamp(fmaxf(fmaxf(a.x, b.x), c.x), 0, camera->target->width);
        float maxY = clamp(fmaxf(fmaxf(a.y, b.y), c.y), 0, camera->target->height);
        
        for (int x = floor(minX); x < ceil(maxX); x++)
        for (int y = floor(minY); y < ceil(maxY); y++) {
            float3 weights;
            if (!PointInTriangle((float2){(float)x, (float)y}, IgnoreZ(a), IgnoreZ(b), IgnoreZ(c), &weights)) continue;

            float depth = 1/Dot3(Inverse3((float3){ a.z, b.z, c.z }), weights);
            if (depth < 0 || camera->depth[x][y] < depth) continue;

            float2 texture = ZERO2;
            texture = Add2(texture, Scale2(object->tris[i].texture[0], weights.x / a.z));
            texture = Add2(texture, Scale2(object->tris[i].texture[1], weights.y / b.z));
            texture = Add2(texture, Scale2(object->tris[i].texture[2], weights.z / c.z));
            texture = Scale2(texture, depth);

            float3 normal = ZERO3;
            normal = Add3(normal, Scale3(object->tris[i].normal[0], weights.x / a.z));
            normal = Add3(normal, Scale3(object->tris[i].normal[1], weights.y / b.z));
            normal = Add3(normal, Scale3(object->tris[i].normal[2], weights.z / c.z));
            normal = Scale3(normal, depth);
            normal = Normalise(normal);

            camera->target->image[x][y] = PixelColour(object, texture, normal);
            camera->depth[x][y] = depth;
        }
    }
}

void DepthVisualisation(Camera *cam, char* fileName) {
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
    SaveBMP(texture, fileName);
    FreeImage(texture);
}

Pixel PixelColour(Object* object, float2 texture, float3 normal) {
    // Half lambert
    float lightIntensity = (Dot3(normal, (float3){0, 1, 0}) + 1) * 0.5;

    Pixel colour;
    if (object->texture == NULL) 
        colour = object->colour;
    else {
        int x, y;
        x = (int)clamp(texture.x * (float)object->texture->width, 0, object->texture->width);
        y = (int)clamp(texture.y * (float)object->texture->height, 0, object->texture->height);
        colour = object->texture->image[x][y];
    }

    return Vec3ToColour(Scale3(ColourToVec3(colour), lightIntensity));
}
