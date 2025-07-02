#include "raylibWrap.h"
#include <stdlib.h>
#include <stdio.h>

#include "engine.h"
#include "rasteriser.h"

void Update(float deltaTime);

#define MOVE_SPEED 10
#define TURN_SPEED 1

Camera *camera;
Object *cube, *monkey, *dragon, *plane;

int main() 
{
    // srand(time(NULL));
    camera = CreateCamera(540, 360, 90, 1000);
    camera->transform = (Transform){ 0.00000f, 0.00000f, -7.75418f, 0.00000f, 0.00000f, 0.00000f, 1.00000f };

    cube = LoadObjFile("assets/cube.obj");
    cube->texture = ReadBMP("assets/CubeBoundsTexture.bmp");
    
    monkey = LoadObjFile("assets/monkey.obj");
    
    dragon = LoadObjFile("assets/dragon.obj");
    
    plane = LoadObjFile("assets/plane.obj");

    Scene *scene = malloc(sizeof(Scene));
    scene->Update = &Update;
    scene->camera = camera;
    scene->NoObjects = 4;
    scene->objects = malloc(sizeof(Object) * scene->NoObjects);
    scene->objects[0] = cube;
    scene->objects[1] = monkey;
    scene->objects[2] = dragon;
    scene->objects[3] = plane;

    Run(scene);

    DestroyCamera(camera);
    FreeObject(cube);
    FreeObject(monkey);
    FreeObject(dragon);
    FreeObject(plane);

	return 0;
}

void Update(float deltaTime) {
    float3 MoveDir = ZERO3;

    if (IsKeyDown(KEY_W)) MoveDir.z++;
    if (IsKeyDown(KEY_S)) MoveDir.z--;
    if (IsKeyDown(KEY_A)) MoveDir.x--;
    if (IsKeyDown(KEY_D)) MoveDir.x++;
    MoveDir = Normalise3(MoveDir);
    MoveDir = Rotate3(MoveDir, (float3){0, camera->transform.rot.y, 0});
    camera->transform.pos = Add3(camera->transform.pos, Scale3(MoveDir, MOVE_SPEED * deltaTime));

    if (IsKeyDown(KEY_LEFT)) camera->transform.rot.y += TURN_SPEED * deltaTime;
    if (IsKeyDown(KEY_RIGHT)) camera->transform.rot.y -= TURN_SPEED * deltaTime;
    if (IsKeyDown(KEY_UP)) camera->transform.rot.x += TURN_SPEED * deltaTime;
    if (IsKeyDown(KEY_DOWN)) camera->transform.rot.x -= TURN_SPEED * deltaTime;
}
