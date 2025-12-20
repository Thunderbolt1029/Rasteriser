#include <stdlib.h>
#include <stdio.h>

#include "engine.h"
#include "rasteriser.h"

int main() 
{
    Camera* camera = CreateCamera(1080, 720, 90, 1000);

    printf("Loading object file...\n");
    Object *cube = LoadObjFile("assets/cube.obj");
    cube->transform = (Transform){0, 0, 3, .5, -.5, 0, 1};
    cube->texture = ReadBMP("assets/CubeBoundsTexture.bmp");
    printf("Loaded object file!\n");
    
    printf("Creating and populating scene...\n");
    int noObjects = 1;
    Object *objects[noObjects];
    objects[0] = cube;

    Scene scene;
    scene.Update = NULL;
    scene.camera = camera;
    scene.NoObjects = noObjects;
    scene.objects = objects;
    
    printf("Rendering scene...\n");
    Run(&scene);

    printf("Cleaning up...\n");
    DestroyCamera(camera);
    
    FreeImage(cube->texture);
    FreeObject(cube);

    printf("Done!\n");
	return 0;
}