#include <math.h>
#include <time.h>
#include <stdio.h>

#include "rasteriser.h"
#include "common.h"
#include "imgio.h"
#include "list.h"
#include "object.h"
#include "transform.h"

void RenderObject(Camera *camera, Object* object);
void RenderTri(Tri screenTri, Camera *camera, Object *object, int sum);
void RenderPixel(int x, int y, Camera *camera, Object *object, Tri screenTri);
Pixel PixelColour(Object* object, float2 texture, float3 normal);
Tri triToScreen(Camera *camera, Transform transform, Tri tri);

#define NEAR_CLIP_DIST 0.01f

void RenderScene(Scene* scene) {
    // clock_t t = clock(); 
    for (int i = 0; i < scene->NoObjects; i++) {
        RenderObject(scene->camera, scene->objects[i]);
        // t = clock() - t; 
        // printf("%s %f\n", scene->objects[i]->fileName, 1/(((double)t)/CLOCKS_PER_SEC)); 
    }
    // printf("\n");
}

void RenderObject(Camera* camera, Object* object) {
    for (int i = 0; i < object->triCount; i++) {
        Tri screenTri = triToScreen(camera, object->transform, object->tris[i]);

        int clip0 = screenTri.vertex[0].z <= 0;
        int clip1 = screenTri.vertex[1].z <= 0;
        int clip2 = screenTri.vertex[2].z <= 0;

        switch (clip0 + clip1 + clip2) {
        case 0:
            RenderTri(screenTri, camera, object, clip0 + clip1 + clip2);
            break;
        case 1:
            // Figure out which point is to be clipped, and the two that will remain
            int indexClip = clip0 ? 0 : clip1 ? 1 : 2;
            int indexNext = (indexClip + 1) % 3;
            int indexPrev = (indexClip - 1 + 3) % 3;
            float3 pointClipped = screenTri.vertex[indexClip];
            float3 pointA = screenTri.vertex[indexNext];
            float3 pointB = screenTri.vertex[indexPrev];

            // Fraction along triangle edge at which the depth is equal to the clip distance
            float fracA = (NEAR_CLIP_DIST - pointClipped.z) / (pointA.z - pointClipped.z);
            float fracB = (NEAR_CLIP_DIST - pointClipped.z) / (pointB.z - pointClipped.z);

            // New triangle points (in view space)
            float3 clipPointAlongEdgeA = Lerp3(pointClipped, pointA, fracA);
            float3 clipPointAlongEdgeB = Lerp3(pointClipped, pointB, fracB);

            // Create new triangles
            Tri rastTri;
            rastTri.vertex[0] = clipPointAlongEdgeB;
            rastTri.vertex[1] = clipPointAlongEdgeA;
            rastTri.vertex[2] = pointB;
            AddRasterizerPoint(model, clipPointAlongEdgeB, i + indexClip, i + indexPrev, fracB);
            AddRasterizerPoint(model, clipPointAlongEdgeA, i + indexClip, i + indexNext, fracA);
            AddRasterizerPoint(model, pointB, i + indexPrev);

            AddRasterizerPoint(model, clipPointAlongEdgeA, i + indexClip, i + indexNext, fracA);
            AddRasterizerPoint(model, pointA, i + indexNext);
            AddRasterizerPoint(model, pointB, i + indexPrev);
            break;
        case 2:
            // Figure out which point will not be clipped, and the two that will be
            int indexNonClip = !clip0 ? 0 : !clip1 ? 1 : 2;
            int indexNext = (indexNonClip + 1) % 3;
            int indexPrev = (indexNonClip - 1 + 3) % 3;

            float3 pointNotClipped = screenTri.vertex[indexNonClip];
            float3 pointA = screenTri.vertex[indexNext];
            float3 pointB = screenTri.vertex[indexPrev];

            // Fraction along triangle edge at which the depth is equal to the clip distance
            float fracA = (NEAR_CLIP_DIST - pointNotClipped.z) / (pointA.z - pointNotClipped.z);
            float fracB = (NEAR_CLIP_DIST - pointNotClipped.z) / (pointB.z - pointNotClipped.z);

            // New triangle points (in view space)
            float3 clipPointAlongEdgeA = Lerp3(pointNotClipped, pointA, fracA);
            float3 clipPointAlongEdgeB = Lerp3(pointNotClipped, pointB, fracB);

            // Create new triangle
            Tri rastTri;
            rastTri.vertex[0] = clipPointAlongEdgeB;
            rastTri.vertex[1] = pointNotClipped;
            rastTri.vertex[2] = clipPointAlongEdgeA;
            rastTri.texture[0] = Lerp2(screenTri.texture[i + indexNonClip], screenTri.texture[i + indexPrev], fracB);
            rastTri.texture[1] = screenTri.texture[i + indexNonClip];
            rastTri.texture[2] = Lerp2(screenTri.texture[i + indexNonClip], screenTri.texture[i + indexPrev], fracA);
            rastTri.normal[0] = Lerp3(screenTri.normal[i + indexNonClip], screenTri.normal[i + indexPrev], fracB);
            rastTri.normal[1] = screenTri.normal[i + indexNonClip];
            rastTri.normal[2] = Lerp3(screenTri.normal[i + indexNonClip], screenTri.normal[i + indexPrev], fracA);
            RenderTri(rastTri, camera, object, clip0 + clip1 + clip2);
        }
    }
}

void RenderTri(Tri screenTri, Camera *camera, Object *object, int sum) {
    float minX = fminf(fminf(screenTri.vertex[0].x, screenTri.vertex[1].x), screenTri.vertex[2].x);
    float minY = fminf(fminf(screenTri.vertex[0].y, screenTri.vertex[1].y), screenTri.vertex[2].y);
    float maxX = fmaxf(fmaxf(screenTri.vertex[0].x, screenTri.vertex[1].x), screenTri.vertex[2].x);
    float maxY = fmaxf(fmaxf(screenTri.vertex[0].y, screenTri.vertex[1].y), screenTri.vertex[2].y);

    float2 clampMin = (float2){clamp(minX, 0, camera->target->width-1), clamp(minY, 0, camera->target->height-1)};
    float2 clampMax = (float2){clamp(maxX, 0, camera->target->width-1), clamp(maxY, 0, camera->target->height-1)};

    for (int x = floor(clampMin.x); x < ceil(clampMax.x); x++)
    for (int y = floor(clampMin.y); y < ceil(clampMax.y); y++) {
        object->colour = (sum == 0) ? (Pixel){255,0,0} : (sum == 1) ? (Pixel){0,255,0} : (Pixel){0,0,255};
        RenderPixel(x, y, camera, object, screenTri);
    }
}

void RenderPixel(int x, int y, Camera *camera, Object *object, Tri screenTri) {
    float3 weights;
    
    if (!PointInTriangle((float2){(float)x, (float)y}, IgnoreZ(screenTri.vertex[0]), IgnoreZ(screenTri.vertex[1]), IgnoreZ(screenTri.vertex[2]), &weights)) 
        return;
    
    float depth = 1/Dot3(Inverse3((float3){ screenTri.vertex[0].z, screenTri.vertex[1].z, screenTri.vertex[2].z }), weights);
    if (camera->depth[x][y] < depth) 
        return;
    
    float2 texture = ZERO2;
    texture = Add2(texture, Scale2(screenTri.texture[0], weights.x / screenTri.vertex[0].z));
    texture = Add2(texture, Scale2(screenTri.texture[1], weights.y / screenTri.vertex[1].z));
    texture = Add2(texture, Scale2(screenTri.texture[2], weights.z / screenTri.vertex[2].z));
    texture = Scale2(texture, depth);
    
    float3 normal = ZERO3;
    normal = Add3(normal, Scale3(screenTri.normal[0], weights.x / screenTri.vertex[0].z));
    normal = Add3(normal, Scale3(screenTri.normal[1], weights.y / screenTri.vertex[1].z));
    normal = Add3(normal, Scale3(screenTri.normal[2], weights.z / screenTri.vertex[2].z));
    normal = Scale3(normal, depth);
    normal = Normalise3(normal);
    normal = Rotate3(normal, object->transform.rot);
    
    camera->target->image[x][y] = PixelColour(object, texture, normal);
    camera->depth[x][y] = depth;
}

Pixel PixelColour(Object* object, float2 texture, float3 normal) {
    float lightIntensity = (Dot3(normal, (float3){0, 1, 0}) + 1) * 0.5;
    lightIntensity = lerp(0, 1, lightIntensity);
    
    Pixel colour;
    if (object->texture == NULL) 
        colour = object->colour;
    else {
        float u = 1-texture.x; // scale by texture scale in future
        float v = texture.y; // * object->textureScale ?

        int x = (int)roundf((u - floorf(u)) * (object->texture->width-1));
        int y = (int)roundf((v - floorf(v)) * (object->texture->height-1));

        x = clamp(x, 0, object->texture->width-1);
        y = clamp(y, 0, object->texture->height-1);

        colour = object->texture->image[x][y];
    }

    return Vec3ToColour(Scale3(ColourToVec3(colour), lightIntensity));
}

Tri triToScreen(Camera *camera, Transform transform, Tri tri) {
    Tri screenTri = tri;
    screenTri.vertex[0] = WorldToScreen(camera, LocalToWorld(transform, tri.vertex[0]));
    screenTri.vertex[1] = WorldToScreen(camera, LocalToWorld(transform, tri.vertex[1]));
    screenTri.vertex[2] = WorldToScreen(camera, LocalToWorld(transform, tri.vertex[2]));
    return screenTri;
}

