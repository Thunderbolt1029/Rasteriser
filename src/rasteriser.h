#ifndef RASTERISER_H
#define RASTERISER_H

#include "object.h"

typedef void (*UpdateCallBack)(float);
typedef struct {
    Camera *camera;
    Object **objects;
    int NoObjects;
    UpdateCallBack Update;
} Scene;

void RenderScene(Scene* scene);

#endif