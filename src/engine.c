#include "engine.h"
#include <math.h>
#include "raylibWrap.h"

#include "imgio.h"

void Flatten(Pixel** TD, Color* OD, int width, int height);
void ClearTexture(Texture* texture);

void Run(Scene* scene) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(scene->camera->target->width, scene->camera->target->height, "Test");

    Texture2D texture = LoadTextureFromImage(GenImageColor(scene->camera->target->width, scene->camera->target->height, BLACK));
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    Color *texColBuffer = MemAlloc(sizeof(Color) * scene->camera->target->width * scene->camera->target->height);

    int pos[2] = {100, 100};

	while (!WindowShouldClose())
	{
        // Update Scene
        (*scene->Update)(GetFrameTime());

        // Render Scene to target
        ClearTexture(scene->camera->target);
        RenderScene(scene);

        // for (int x = 0; x < scene->camera->target->width; x++)
        // for (int y = 0; y < scene->camera->target->height; y++) {
        //     target[x][y] = (float3){(float)x/scene->camera->target->width, (float)y/scene->camera->target->height, 0};
        // }
        // target[pos[0]][pos[1]] = (float3){1, 1, 1};

        Flatten(scene->camera->target->image, texColBuffer, scene->camera->target->width, scene->camera->target->height);
        UpdateTexture(texture, texColBuffer);

        Rectangle src = (Rectangle){0, scene->camera->target->height, scene->camera->target->width, -scene->camera->target->height};
        Rectangle dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
        Vector2 origin = (Vector2){0, 0};

        // Draw
        BeginDrawing();
        DrawTexturePro(texture, src, dest, origin, 0.0f, WHITE);
        DrawFPS(10, 10);
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

void ClearTexture(Texture* texture) {
    for (int x = 0; x < texture->width; x++)
    for (int y = 0; y < texture->height; y++)
        texture->image[x][y] = (Pixel) { 0, 0, 0 };
}
