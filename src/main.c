#include "raylibWrap.h"
#include <stdlib.h>
#include <stdio.h>

#include "engine.h"
#include "rasteriser.h"

void Update(float deltaTime);

#define MOVE_SPEED 10
#define TURN_SPEED 1

Scene scene;

int main() 
{
    scene.Update = &Update;

    scene.camera = CreateCamera(540, 360, 90, 1000);
    scene.camera->transform = (Transform){ 0.00000f, 0.00000f, -7.75418f, 0.00000f, 0.00000f, 0.00000f, 1.00000f };

    scene.noObjects = 4;
    Object *objects[scene.noObjects];
    scene.objects = objects;

    objects[0] = LoadObjFile("assets/cube.obj");
    objects[0]->texture = ReadBMP("assets/CubeBoundsTexture.bmp");
    objects[0]->transform.pos.y += 0.01f;

    objects[1] = LoadObjFile("assets/monkey.obj");
    objects[2] = LoadObjFile("assets/dragon.obj");
    objects[3] = LoadObjFile("assets/plane.obj");

    Run(&scene);

    DestroyCamera(scene.camera);
    
    FreeImage(objects[0]->texture);
    for (int i = 0; i < scene.noObjects; i++)
        FreeObject(objects[i]);

	return 0;
}

void Update(float deltaTime) {
    float3 MoveDir = ZERO3;

    if (IsKeyDown(KEY_W)) MoveDir.z++;
    if (IsKeyDown(KEY_S)) MoveDir.z--;
    if (IsKeyDown(KEY_A)) MoveDir.x--;
    if (IsKeyDown(KEY_D)) MoveDir.x++;

    MoveDir = Normalise3(MoveDir);
    MoveDir = Rotate3(MoveDir, (float3){0, scene.camera->transform.rot.y, 0});
    scene.camera->transform.pos = Add3(scene.camera->transform.pos, Scale3(MoveDir, MOVE_SPEED * deltaTime));

    if (IsKeyDown(KEY_LEFT)) scene.camera->transform.rot.y += TURN_SPEED * deltaTime;
    if (IsKeyDown(KEY_RIGHT)) scene.camera->transform.rot.y -= TURN_SPEED * deltaTime;
    if (IsKeyDown(KEY_UP)) scene.camera->transform.rot.x += TURN_SPEED * deltaTime;
    if (IsKeyDown(KEY_DOWN)) scene.camera->transform.rot.x -= TURN_SPEED * deltaTime;
}
