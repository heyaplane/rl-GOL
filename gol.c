#include "raylib.h"
#include "rlgl.h"
#include "stdlib.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    SetTraceLogLevel(LOG_WARNING);

    int gol_width = 800;
    int gol_height = 400;
    int scale_factor = 1;

    if (argc > 1) gol_width = atoi(argv[1]);
    if (argc > 2) gol_height = atoi(argv[2]);
    if (argc > 3) scale_factor = atoi(argv[3]);

    if (gol_width <= 0 || gol_width % 16 > 0 || gol_height <=0 || gol_height % 16 > 0 || scale_factor < 1)
    {
        printf("Bad command line args.");
        return -1;
    }

    int gol_num_grp_x = gol_width / 16;
    int gol_num_grp_y = gol_height / 16;

    InitWindow(gol_width * scale_factor, gol_height * scale_factor, "GOL");

    char* computeCode = LoadFileText("shaders/gol.glsl");
    unsigned int computeShader = rlCompileShader(computeCode, RL_COMPUTE_SHADER);
    unsigned int computeProgram = rlLoadComputeShaderProgram(computeShader);
    UnloadFileText(computeCode);

    int wVar = rlGetLocationUniform(computeProgram, "gol_width");
    int hVar = rlGetLocationUniform(computeProgram, "gol_height");
    rlEnableShader(computeProgram);
    rlSetUniform(wVar, &gol_width, RL_SHADER_UNIFORM_UINT, 1);
    rlSetUniform(hVar, &gol_height, RL_SHADER_UNIFORM_UINT, 1);
    rlDisableShader();

    const Vector2 resolution = { gol_width, gol_height};
    Shader renderShader = LoadShader(NULL, "shaders/gol_render.glsl");
    int resVar = GetShaderLocation(renderShader, "resolution");
    SetShaderValue(renderShader, resVar, &resolution, SHADER_UNIFORM_VEC2);

    unsigned int ssboA = rlLoadShaderBuffer(gol_width * gol_height * sizeof(unsigned int), NULL, RL_DYNAMIC_COPY);
    unsigned int ssboB = rlLoadShaderBuffer(gol_width * gol_height * sizeof(unsigned int), NULL, RL_DYNAMIC_COPY);

    {
        unsigned int initialState[gol_width * gol_height];
        for (int i = 0; i < gol_width * gol_height; i++)
        {
            initialState[i] = GetRandomValue(0, 1);
        }

        rlUpdateShaderBuffer(ssboA, initialState, gol_width * gol_height * sizeof(unsigned int), 0);
    }

    Image whiteImg = GenImageColor(gol_width, gol_height, WHITE);
    Texture whiteTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    uint frameCount = 0;

    while (!WindowShouldClose())
    {
        SetTargetFPS(60);
        frameCount++;

        rlEnableShader(computeProgram);
        rlBindShaderBuffer(ssboA, 1);
        rlBindShaderBuffer(ssboB, 2);
        rlComputeShaderDispatch(gol_num_grp_x, gol_num_grp_y, 1);
        rlDisableShader();

        int temp = ssboA;
        ssboA = ssboB;
        ssboB = temp;

        rlBindShaderBuffer(ssboA, 1);

        BeginDrawing();

            ClearBackground(BLANK);

            BeginShaderMode(renderShader);
            DrawTextureEx(whiteTex, (Vector2){0, 0}, 0.0, (float)scale_factor, WHITE);
            EndShaderMode();

            DrawFPS(GetScreenWidth() - 150, 10);
            DrawText(TextFormat("Count: %d", frameCount), GetScreenWidth() - 150, 30, 20, GREEN);

        EndDrawing();
    }

    rlUnloadShaderBuffer(ssboA);
    rlUnloadShaderBuffer(ssboB);

    rlUnloadShaderProgram(computeProgram);
    UnloadShader(renderShader);

    UnloadTexture(whiteTex);

    CloseWindow();
    return 0;
}
