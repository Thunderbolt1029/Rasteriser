#include "common.h"

#include <stdlib.h>
#include <math.h>

inline float2 Add2(float2 a, float2 b) { return (float2){ a.x + b.x, a.y + b.y }; }
inline float2 Sub2(float2 a, float2 b) { return (float2){ a.x - b.x, a.y - b.y }; }
inline float2 Scale2(float2 vec, float scale) { return (float2){ vec.x * scale, vec.y * scale }; }
inline int Equal2(float2 a, float2 b) { return a.x == b.x && a.y == b.y; }
inline float LengthSquared2(float2 vec) { return vec.x * vec.x + vec.y * vec.y; }
inline float Dot2(float2 a, float2 b) { return a.x * b.x + a.y * b.y; }
inline float2 Perpendicular(float2 vec) { return (float2){ vec.y, -vec.x }; }
inline float2 Lerp2(float2 a, float2 b, float t) {
    return (float2){lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
}

inline float3 Add3(float3 a, float3 b) { return (float3){ a.x + b.x, a.y + b.y, a.z + b.z }; }
inline float3 Sub3(float3 a, float3 b) { return (float3){ a.x - b.x, a.y - b.y, a.z - b.z }; }
inline float3 Scale3(float3 vec, float scale) { return (float3){ vec.x * scale, vec.y * scale, vec.z * scale }; }
inline int Equal3(float3 a, float3 b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline float LengthSquared3(float3 vec) { return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z; }
inline float Length3(float3 vec) { return sqrtf(LengthSquared3(vec)); }
inline float Dot3(float3 a, float3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float3 Inverse3(float3 vec) { return (float3){1/vec.x, 1/vec.y, 1/vec.z}; }
inline float3 Normalise3(float3 vec) {  return Scale3(vec, Length3(vec)); }
inline Pixel Vec3ToColour(float3 vec) {
    return (Pixel){ 
        clamp(vec.x, 0, 1) * 255,
        clamp(vec.y, 0, 1) * 255,
        clamp(vec.z, 0, 1) * 255,
    };
}
inline float3 ColourToVec3(Pixel col) {
    return (float3){
        clamp((float)col.red / 255.f, 0, 1),
        clamp((float)col.green / 255.f, 0, 1),
        clamp((float)col.blue / 255.f, 0, 1)
    };
}
inline float3 Lerp3(float3 a, float3 b, float t) {
    return (float3){lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t)};
}

inline float2 IgnoreZ(float3 vec) { return (float2){ vec.x, vec.y }; }

/*
float SignedTriangleArea(float2 a, float2 b, float2 c) {
    float2 ac = Sub2(c, a);
    float2 abPerp = Perpendicular(Sub2(b, a));
    return Dot2(ac, abPerp) / 2.f;
}
*/
inline float SignedTriangleArea(float2 a, float2 b, float2 c) {
    return ((c.x - a.x) * (b.y - a.y) + (c.y - a.y) * (a.x - b.x)) / 2.f;
}

int PointInTriangle(float2 p, float2 a, float2 b, float2 c, float3 *weights) {
    float areaABP = SignedTriangleArea(a, b, p);
    float areaBCP = SignedTriangleArea(b, c, p);
    float areaCAP = SignedTriangleArea(c, a, p);
    int inTri = areaABP >= 0 && areaBCP >= 0 && areaCAP >= 0 || areaABP <= 0 && areaBCP <= 0 && areaCAP <= 0;

    if (weights != NULL) {
        float invAreaSum = 1 / (areaABP + areaBCP + areaCAP);
        *weights = (float3){ areaBCP*invAreaSum, areaCAP*invAreaSum, areaABP*invAreaSum };
    }

    return inTri;
}

inline int randInt(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
inline float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}
inline float lerp(float x, float y, float t) {
    // t = clamp(t, 0, 1);
    return y * t + x * (1-t);
}