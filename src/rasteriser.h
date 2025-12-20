#ifndef RASTERISER_H
#define RASTERISER_H

#include "object.h"
#include "threadpool.h"

typedef void (*UpdateCallBack)(float);
typedef struct {
    Camera *camera;
    Object **objects;
    int noObjects;
    UpdateCallBack Update;
} Scene;

void RenderScene(Scene* scene, tpool_t *tpool);

#endif