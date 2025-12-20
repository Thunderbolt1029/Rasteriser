#ifndef OBJECT_H
#define OBJECT_H

#include "transform.h"

typedef struct {
    float3 vertex[3];
    float3 normal[3];
    float2 texture[3];
} Tri;
typedef struct {
    const char *fileName;
    Transform transform;
    int triCount;
    Tri *tris;
    Texture *texture;
    Pixel colour;
} Object;

Object* LoadObjFile(const char* objFileName);
void FreeObject(Object* object);

#endif