#include <math.h>

#include "raylib.h"

typedef struct float3 {
    float x;
    float y;
    float z;
} float3;

#define WIDTH 1280
#define HEIGHT 720

void Flatten(float3** TD, Color* OD);
void Clear(float3** A);

int main()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(WIDTH, HEIGHT, "Test");

    Texture texture = LoadTextureFromImage(GenImageColor(WIDTH, HEIGHT, BLACK));
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    Color *texColBuffer = MemAlloc(sizeof(Color) * WIDTH * HEIGHT);

	float3 **target = MemAlloc(sizeof(float3*) * WIDTH);
    for (int x = 0; x < WIDTH; x++)
        target[x] = MemAlloc(sizeof(float3) * HEIGHT);
    Clear(target);

    int pos[2] = {100, 100};

	while (!WindowShouldClose())
	{

        // Update Scene
        if (IsKeyDown(KEY_LEFT)) pos[0]--;
        if (IsKeyDown(KEY_RIGHT)) pos[0]++;
        if (IsKeyDown(KEY_UP)) pos[1]++;
        if (IsKeyDown(KEY_DOWN)) pos[1]--;

        // Render Scene to target
        for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++) {
            target[x][y] = (float3){(float)x/WIDTH, (float)y/HEIGHT, 0};
        }
        target[pos[0]][pos[1]] = (float3){1, 1, 1};

        Flatten(target, texColBuffer);
        UpdateTexture(texture, texColBuffer);
        Clear(target);

        Rectangle src = (Rectangle){0, HEIGHT, WIDTH, -HEIGHT};
        Rectangle dest = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
        Vector2 origin = (Vector2){0, 0};

        // Draw
        BeginDrawing();
        DrawTexturePro(texture, src, dest, origin, 0.0f, WHITE);
        DrawFPS(10, 10);
		EndDrawing();
	}

    for (int x = 0; x < WIDTH; x++)
        MemFree(target[x]);
    MemFree(target);

    MemFree(texColBuffer);
    UnloadTexture(texture);

	CloseWindow();
	return 0;
}

void Flatten(float3** TD, Color* OD) {
    for (int x = 0; x < WIDTH; x++)
    for (int y = 0; y < HEIGHT; y++) {
        OD[WIDTH*y + x] = (Color){
            roundf(TD[x][y].x * 255),
            roundf(TD[x][y].y * 255),
            roundf(TD[x][y].z * 255),
            255
        };
    }
}

void Clear(float3** A) {
    for (int x = 0; x < WIDTH; x++)
    for (int y = 0; y < HEIGHT; y++)
        A[x][y] = (float3){0, 0, 0};    
}
