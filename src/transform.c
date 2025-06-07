#include <math.h>

#include "transform.h"

float3 LocalToWorld(Transform transform, float3 point) {
    M4x4 xRot = { 1, 0, 0, 0, 0, cosf(transform.rot.x), -sinf(transform.rot.x), 0, 0, sinf(transform.rot.x), cosf(transform.rot.x), 0, 0, 0, 0, 1 };
    M4x4 yRot = { cosf(transform.rot.y), 0, sinf(transform.rot.y), 0, 0, 1, 0, 0, -sinf(transform.rot.y), 0, cosf(transform.rot.y), 0, 0, 0, 0, 1 };
    M4x4 zRot = { cosf(transform.rot.z), sinf(transform.rot.z), 0, 0, -sinf(transform.rot.z), cosf(transform.rot.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    M4x4 trans = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, transform.pos.x, transform.pos.y, transform.pos.z, 1 };
    return Transform3(point, MatMultiply(MatMultiply(trans, xRot), MatMultiply(yRot, zRot)));
}

float2 WorldToScreen(Camera* camera, float3 worldPoint, int width, int height) {
    float screenHeight_world = 5;
    float pixelsPerWorldUnit = (float)height / screenHeight_world;
    
    float2 pixelOffset = Scale2((float2){worldPoint.x, worldPoint.y}, pixelsPerWorldUnit);
    return Add2(Scale2((float2){(float)width, (float)height}, 0.5f), pixelOffset);
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

float3 Transform3(float3 vec, M4x4 a) {
    return (float3){
        vec.x * a._00 + vec.y * a._10 + vec.z * a._20 + a._30,
        vec.x * a._01 + vec.y * a._11 + vec.z * a._21 + a._31,
        vec.x * a._02 + vec.y * a._12 + vec.z * a._22 + a._32,
    };
}
