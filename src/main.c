#include "raylibWrap.h"
#include <stdlib.h>
#include <stdio.h>

#include "engine.h"
#include "rasteriser.h"

void Update(float deltaTime);

Camera *cam;
Object *cube, *monkey, *dragon;

int main() 
{
    // srand(time(NULL));
    cam = CreateCamera(1080, 720, 90, 1000);
    
    cube = LoadObjFile("assets/cube.obj");
    cube->transform = (Transform){0, 1.5, 5, .5, -.5, 0};
    cube->texture = ReadBMP("assets/CubeBoundsTexture.bmp");
    
    monkey = LoadObjFile("assets/monkey.obj");
    monkey->transform = (Transform){-3, 0, 5, .3, PI + .3, 0};
    
    dragon = LoadObjFile("assets/dragon.obj");
    dragon->transform = (Transform){4, 0, 5, 0, PI / 2, 0};
    
    Scene *scene = malloc(sizeof(Scene));
    scene->Update = &Update;
    scene->camera = cam;
    scene->NoObjects = 3;
    scene->objects = malloc(sizeof(Object) * scene->NoObjects);
    scene->objects[0] = cube;
    scene->objects[1] = monkey;
    scene->objects[2] = dragon;
    
    Run(scene);

    DestroyCamera(cam);
    FreeObject(cube);
    FreeObject(monkey);
    FreeObject(dragon);

	return 0;
}

void Update(float deltaTime) {
    if (IsKeyDown(KEY_W)) cam->transform.pos.z++;
    if (IsKeyDown(KEY_S)) cam->transform.pos.z--;
    if (IsKeyDown(KEY_A)) cam->transform.pos.x--;
    if (IsKeyDown(KEY_D)) cam->transform.pos.x++;
}
