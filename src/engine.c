#include "engine.h"
#include <math.h>
#include "raylibWrap.h"
#include <time.h>
#include <stdio.h>

#include "imgio.h"

void Flatten(Pixel** TD, Color* OD, int width, int height);

void Run(Scene* scene) {
    SetConfigFlags(/*FLAG_VSYNC_HINT | */FLAG_WINDOW_HIGHDPI);
	InitWindow(1620, 1080, "Rasteriser");
	// InitWindow(scene->camera->target->width, scene->camera->target->height, "Rasteriser");

    Texture2D texture = LoadTextureFromImage(GenImageColor(scene->camera->target->width, scene->camera->target->height, BLACK));
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    Color *texColBuffer = MemAlloc(sizeof(Color) * scene->camera->target->width * scene->camera->target->height);

	while (!WindowShouldClose())
	{
        // Update Scene
        (*scene->Update)(GetFrameTime());
                
        // Render Scene to target
        ClearCamera(scene->camera);
        RenderScene(scene);

        Flatten(scene->camera->target->image, texColBuffer, scene->camera->target->width, scene->camera->target->height);
        UpdateTexture(texture, texColBuffer);

        Rectangle src = (Rectangle){0, scene->camera->target->height, scene->camera->target->width, -scene->camera->target->height};
        Rectangle dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
        Vector2 origin = (Vector2){0, 0};

        // Draw
        BeginDrawing();
        DrawTexturePro(texture, src, dest, origin, 0.0f, WHITE);
        DrawFPS(10, 10);
        DrawText(TextFormat("pos: %f %f %f\nrot: %f %f %f", 
                scene->camera->transform.pos.x,
                scene->camera->transform.pos.y,
                scene->camera->transform.pos.z,
                scene->camera->transform.rot.x,
                scene->camera->transform.rot.y,
                scene->camera->transform.rot.z
            ), 10, 40, 20, LIME
        );
		EndDrawing();
	}

    MemFree(texColBuffer);
    UnloadTexture(texture);

	CloseWindow();
}

void Flatten(Pixel** TD, Color* OD, int width, int height) {
    for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++) {
        OD[width*y + x] = (Color){
            TD[x][y].red,
            TD[x][y].green,
            TD[x][y].blue,
            255
        };
    }
}
