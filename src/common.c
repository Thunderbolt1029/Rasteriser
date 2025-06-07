#include "common.h"

#include <stdlib.h>

float2 Add2(float2 a, float2 b) { 
    return (float2){ a.x + b.x, a.y + b.y };
}
float2 Sub2(float2 a, float2 b) { 
    return (float2){ a.x - b.x, a.y - b.y };
}
float2 Scale2(float2 vec, float scale) {
    return (float2){ vec.x * scale, vec.y * scale };
}
int Equal2(float2 a, float2 b) {
    return a.x == b.x && a.y == b.y;
}
float LengthSquared2(float2 vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

float Dot2(float2 a, float2 b) {
    return a.x * b.x + a.y * b.y; 
}
float Dot3(float3 a, float3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float2 Perpendicular(float2 vec) {
    return (float2){ vec.y, -vec.x };
}

float SignedTriangleArea(float2 a, float2 b, float2 c) {
    float2 ac = Sub2(c, a);
    float2 abPerp = Perpendicular(Sub2(b, a));
    return Dot2(ac, abPerp) / 2.f;
}

int PointInTriangle(float2 p, float2 a, float2 b, float2 c) {
    float areaABP = SignedTriangleArea(a, b, p);
    float areaBCP = SignedTriangleArea(b, c, p);
    float areaCAP = SignedTriangleArea(c, a, p);
    int inTri = areaABP >= 0 && areaBCP >= 0 && areaCAP >= 0;

    return inTri;
}

int randInt(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}
