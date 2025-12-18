#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <pthread.h>

#include "common.h"
#include "imgio.h"

typedef struct {
    float3 pos;
    float3 rot;
    float scale;
} Transform;

typedef struct {
    Texture *target;
    float **depth;
    Transform transform;
    float fov;
    float maxDistance;

    pthread_mutex_t **pixMutex;
} Camera;

float3 LocalToWorld(Transform transform, float3 point);
float3 WorldToLocal(Transform transform, float3 point);
float3 WorldToScreen(Camera* camera, float3 worldPoint);
float3 ViewToScreen(Camera* camera, float3 viewPoint);

float3 Rotate3(float3 vec, float3 rot);
float3 Transform3(float3 vec, M4x4 a);
M4x4 MatMultiply(M4x4, M4x4);

Camera *CreateCamera(int width, int height, float fov, float maxDistance);
void DestroyCamera(Camera *camera);

void ClearCamera(Camera *camera);

#endif