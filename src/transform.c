#include <math.h>

#include "transform.h"

float3 LocalToWorld(Transform transform, float3 point) {
    return Add3(Rotate3(Scale3(point, transform.scale), transform.rot), transform.pos);
}

float3 WorldToLocal(Transform transform, float3 point) {
    M4x4 xRot = { 1, 0, 0, 0, 0, cosf(transform.rot.x), sinf(transform.rot.x), 0, 0, -sinf(transform.rot.x), cosf(transform.rot.x), 0, 0, 0, 0, 1 };
    M4x4 yRot = { cosf(transform.rot.y), 0, -sinf(transform.rot.y), 0, 0, 1, 0, 0, sinf(transform.rot.y), 0, cosf(transform.rot.y), 0, 0, 0, 0, 1 };
    M4x4 zRot = { cosf(transform.rot.z), -sinf(transform.rot.z), 0, 0, sinf(transform.rot.z), cosf(transform.rot.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    M4x4 combRot = MatMultiply(MatMultiply(zRot, yRot), xRot);

    return Scale3(Transform3(Sub3(point, transform.pos), combRot), 1/transform.scale);
}

float3 WorldToScreen(Camera* camera, float3 worldPoint) {
    float3 localPoint = WorldToLocal(camera->transform, worldPoint);
    return ViewToScreen(camera, localPoint);
}

float3 ViewToScreen(Camera* camera, float3 localPoint) {
    float screenWidth_world = tanf(camera->fov * PI / 360) * 2;
    float pixelsPerWorldUnit = (float)camera->target->width / screenWidth_world / localPoint.z;
    
    float2 pixelOffset = Scale2(IgnoreZ(localPoint), pixelsPerWorldUnit);
    float3 depthOffset = { pixelOffset.x, pixelOffset.y, localPoint.z };
    return Add3(Scale3((float3){(float)camera->target->width, (float)camera->target->height, 0}, 0.5f), depthOffset);
}

M4x4 MatMultiply(M4x4 a, M4x4 b) {
    return (M4x4){
        b._00*a._00 + b._10*a._01 + b._20*a._02 + b._30*a._03,
        b._01*a._00 + b._11*a._01 + b._21*a._02 + b._31*a._03,
        b._02*a._00 + b._12*a._01 + b._22*a._02 + b._32*a._03,
        b._03*a._00 + b._13*a._01 + b._23*a._02 + b._33*a._03,

        b._00*a._10 + b._10*a._11 + b._20*a._12 + b._30*a._13,
        b._01*a._10 + b._11*a._11 + b._21*a._12 + b._31*a._13,
        b._02*a._10 + b._12*a._11 + b._22*a._12 + b._32*a._13,
        b._03*a._10 + b._13*a._11 + b._23*a._12 + b._33*a._13,

        b._00*a._20 + b._10*a._21 + b._20*a._22 + b._30*a._23,
        b._01*a._20 + b._11*a._21 + b._21*a._22 + b._31*a._23,
        b._02*a._20 + b._12*a._21 + b._22*a._22 + b._32*a._23,
        b._03*a._20 + b._13*a._21 + b._23*a._22 + b._33*a._23,

        b._00*a._30 + b._10*a._31 + b._20*a._32 + b._30*a._33,
        b._01*a._30 + b._11*a._31 + b._21*a._32 + b._31*a._33,
        b._02*a._30 + b._12*a._31 + b._22*a._32 + b._32*a._33,
        b._03*a._30 + b._13*a._31 + b._23*a._32 + b._33*a._33
    };
}

float3 Rotate3(float3 vec, float3 rot) {
    M4x4 xRot = { 1, 0, 0, 0, 0, cosf(rot.x), -sinf(rot.x), 0, 0, sinf(rot.x), cosf(rot.x), 0, 0, 0, 0, 1 };
    M4x4 yRot = { cosf(rot.y), 0, sinf(rot.y), 0, 0, 1, 0, 0, -sinf(rot.y), 0, cosf(rot.y), 0, 0, 0, 0, 1 };
    M4x4 zRot = { cosf(rot.z), sinf(rot.z), 0, 0, -sinf(rot.z), cosf(rot.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    M4x4 combRot = MatMultiply(MatMultiply(xRot, yRot), zRot);

    return Transform3(vec, combRot);
}
float3 Transform3(float3 vec, M4x4 a) {
    return (float3){
        vec.x * a._00 + vec.y * a._10 + vec.z * a._20 + a._30,
        vec.x * a._01 + vec.y * a._11 + vec.z * a._21 + a._31,
        vec.x * a._02 + vec.y * a._12 + vec.z * a._22 + a._32,
    };
}

Camera *CreateCamera(int width, int height, float fov, float maxDistance) {
    Camera *cam = malloc(sizeof(Camera));

    cam->target = CreateImage(width, height);

    cam->depth = malloc(sizeof(float*) * width);
    for (int i = 0; i < width; i++)
        cam->depth[i] = malloc(sizeof(float) * height);    
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            cam->depth[x][y] = maxDistance;

    cam->transform = (Transform){ 0, 0, 0, 0, 0, 0, 1 };
    cam->fov = fov;
    cam->maxDistance = maxDistance;

    cam->pixMutex = malloc(sizeof(pthread_mutex_t*) * width);
    for (int i = 0; i < width; i++)
        cam->pixMutex[i] = malloc(sizeof(pthread_mutex_t) * height);    
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            pthread_mutex_init(&(cam->pixMutex[x][y]), NULL);

    return cam;
}
void DestroyCamera(Camera *cam) {
    for (int x = 0; x < cam->target->width; x++)
        for (int y = 0; y < cam->target->height; y++)
            pthread_mutex_destroy(&(cam->pixMutex[x][y]));
    for (int i = 0; i < cam->target->width; i++)
        free(cam->pixMutex[i]); 
    free(cam->pixMutex);

    for (int i = 0; i < cam->target->width; i++)
        free(cam->depth[i]);
    free(cam->depth);

    FreeImage(cam->target);
    free(cam);
}

void ClearCamera(Camera *camera) {
    for (int x = 0; x < camera->target->width; x++)
    for (int y = 0; y < camera->target->height; y++) {
        camera->target->image[x][y] = (Pixel) { 0, 0, 0 };
        camera->depth[x][y] = camera->maxDistance;
    }
}
