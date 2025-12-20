#include "engine.h"
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "imgio.h"
#include "threadpool.h"

#define NO_THREADS 16

void Run(Scene* scene) {
    tpool_t *tpool = tpool_create(NO_THREADS);
    if (tpool == NULL) {
        fprintf(stderr, "Failed to create thread pool.\nExiting...\n");
        return;
    }

    // Render Scene to target
    RenderScene(scene, tpool);
    tpool_wait(tpool);

    // Save output to bmp
    printf("Saving rendered output...\n");
    if (SaveBMP(scene->camera->target, "output/out.bmp")) 
        fprintf(stderr, "FAILED TO SAVE RENDER\n");
    else
        printf("Saved!\n");

    tpool_destroy(tpool);
}
