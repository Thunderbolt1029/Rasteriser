#include <math.h>
#include <time.h>

#include "rasteriser.h"
#include "common.h"
#include "imgio.h"
#include "list.h"
#include "object.h"
#include "transform.h"

void RenderObject(Camera *camera, Object* object);
void DepthVisualisation(Camera *cam, char* fileName);
Pixel PixelColour(Object* object, float2 texture, float3 normal);

void RenderScene(Scene* scene) {
    for (int i = 0; i < scene->NoObjects; i++)
        RenderObject(scene->camera, scene->objects[i]);
}

void RenderObject(Camera* camera, Object* object) {
    for (int i = 0; i < object->triCount; i++) {
        float3 a = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[0]));
        float3 b = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[1]));
        float3 c = WorldToScreen(camera, LocalToWorld(object->transform, object->tris[i].vertex[2]));

        if (a.z <= 0 || b.z <= 0 || c.z <= 0) continue;

        float minX = clamp(fminf(fminf(a.x, b.x), c.x), 0, camera->target->width);
        float minY = clamp(fminf(fminf(a.y, b.y), c.y), 0, camera->target->height);
        float maxX = clamp(fmaxf(fmaxf(a.x, b.x), c.x), 0, camera->target->width);
        float maxY = clamp(fmaxf(fmaxf(a.y, b.y), c.y), 0, camera->target->height);
        
        for (int x = floor(minX); x < ceil(maxX); x++)
        for (int y = floor(minY); y < ceil(maxY); y++) {
            float3 weights;
            
            if (!PointInTriangle((float2){(float)x, (float)y}, IgnoreZ(a), IgnoreZ(b), IgnoreZ(c), &weights)) continue;
            
            float depth = 1/Dot3(Inverse3((float3){ a.z, b.z, c.z }), weights);
            if (camera->depth[x][y] < depth) continue;
            
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
            normal = Rotate3(normal, object->transform.rot);
            
            camera->target->image[x][y] = PixelColour(object, texture, normal);
            camera->depth[x][y] = depth;
        }
    }
}

void DepthVisualisation(Camera *cam, char* fileName) {
    float min = cam->maxDistance, max = 0;
    for (int x = 0; x < cam->target->width; x++)
    for (int y = 0; y < cam->target->height; y++) {
        if (cam->depth[x][y] < min) min = cam->depth[x][y];
        if (cam->depth[x][y] > max && cam->depth[x][y] < cam->maxDistance) max = cam->depth[x][y];
    }
    Texture* texture = CreateImage(cam->target->width, cam->target->height);
    for (int x = 0; x < cam->target->width; x++)
    for (int y = 0; y < cam->target->height; y++) {
        float col;
        if (cam->depth[x][y] == cam->maxDistance) col = 0.2;
        else col = (cam->depth[x][y] - min) / (max - min);
        texture->image[x][y] = (Pixel){ col * 255, col * 255, col * 255 };
    }
    SaveBMP(texture, fileName);
    FreeImage(texture);
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
