#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "common.h"
#include "imgio.h"

typedef struct {
    float3 pos;
    float3 rot;
} Transform;

typedef struct {
    Texture *target;
    Transform transform;
    float fov;
} Camera;

float3 LocalToWorld(Transform transform, float3 point);
float2 WorldToScreen(Camera* camera, float3 worldPoint);

float3 Transform3(float3 vec, M4x4 a);
M4x4 MatMultiply(M4x4, M4x4);

#endif