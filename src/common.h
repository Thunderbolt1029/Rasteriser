#ifndef COMMON_H
#define COMMON_H

#include "imgio.h"

#define PI 3.14159265359

#define ZERO2 (float2){0,0}
#define ZERO3 (float3){0,0,0}

typedef struct {
    float x;
    float y;
} float2;

typedef struct {
    float x;
    float y;
    float z;
} float3;

typedef struct {
    float _00;
    float _01;
    float _02;
    float _03;
    float _10;
    float _11;
    float _12;
    float _13;
    float _20;
    float _21;
    float _22;
    float _23;
    float _30;
    float _31;
    float _32;
    float _33;
} M4x4;

float2 Add2(float2 a, float2 b);
float2 Sub2(float2 a, float2 b);
float2 Scale2(float2 vec, float scale);
int Equal2(float2 a, float2 b);
float LengthSquared2(float2 vec);
float Dot2(float2, float2);
float2 Perpendicular(float2);

float3 Add3(float3 a, float3 b);
float3 Sub3(float3 a, float3 b);
float3 Scale3(float3 vec, float scale);
int Equal3(float3 a, float3 b);
float LengthSquared3(float3 vec);
float Dot3(float3, float3);
float3 Inverse3(float3);
float3 Normalise(float3);
Pixel Vec3ToColour(float3);
float3 ColourToVec3(Pixel col);

float2 IgnoreZ(float3 vec);

float SignedTriangleArea(float2, float2, float2);
int PointInTriangle(float2 p, float2 a, float2 b, float2 c, float3 *weights);

int randInt(int min, int max);
float clamp(float d, float min, float max);
float lerp(float x, float y, float t);

#endif
