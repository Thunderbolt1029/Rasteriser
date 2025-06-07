#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "common.h"

typedef struct {
    float3 pos;
    float3 rot;
} Transform;

typedef struct {
    
} Camera;

float3 LocalToWorld(Transform transform, float3 point);
float2 WorldToScreen(Camera* camera, float3 worldPoint, int width, int height);

float3 Transform3(float3 vec, M4x4 a);
M4x4 MatMultiply(M4x4, M4x4);

#endif