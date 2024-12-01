#include "raylib.h"
#include "rlgl.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// gcc gol.c -o gol -lraylib -lm

void parse_rle(const char* rle_string, int* initialState, int width, int height, int start_x, int start_y)
{
    int row = start_y;
    int col = start_x;
    int repeat = 0;

    for (const char* c = rle_string; *c != '!'; ++c)
    {
        if (isdigit(*c))
            repeat = repeat * 10 + (*c - '0');

        else if (*c == 'b' || *c == 'o')
        {
            int count = (repeat == 0) ? 1 : repeat;
            repeat = 0;

            for (int i = 0; i < count; i++)
            {
               if (row < height && col < width)
                   initialState[row * width + col] = (*c == 'o') ? 1 : 0;
               col++;

               if (col >= width)
               {
                   col = start_x;
                   row++;
                   if (row >= height) return;
               }
            }
        }

        else if (*c == '$')
        {
            int count = (repeat == 0) ? 1 : repeat;
            repeat = 0;
            row += count;
            col = start_x;
        }
    }
}

uint handle_cell_flag(int frameDiff, uint* bits, uint bitCount, uint maxBitCount)
{
    if (bitCount == 0)
    {
        bits[0] = 1;
        return 1;
    }

    frameDiff = frameDiff - 46;
    uint zeroCount = frameDiff / 46;

    if (bitCount + zeroCount + 1 >= maxBitCount)
        return 0;

    for (uint i = 0; i < zeroCount; i++)
    {
        bits[bitCount + i] = 0;
    }

    bits[bitCount + zeroCount] = 1;
    return zeroCount + 1;
}

void write_bits_to_file(const char* filename, const uint* bits, uint length)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Bad file open.");
        return;
    }

    for (uint i = 0; i < length; i++)
    {
        fprintf(file, "%u\n", bits[i]);
    }

    if (fclose(file) != 0)
        perror("Bad file close.");
}

int main(int argc, char** argv)
{
    int gol_width = 320;
    int gol_height = 320;
    int scale_factor = 4;
    int start_x = 100;
    int start_y = 100;

    // const char* rle_string = "24bo$22bobo$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o$2o8bo3bob2o4bobo$10bo5bo7bo$11bo3bo$12b2o!";
    /* const char* rle_string = "23bo$21bobo$11b2o6b2o$10bo3bo4b2o12b2o$9bo5bo3b2o12b2o$9bo3bob2o4bobo$9bo5bo7bo$10bo3bo$11b2o6$11b2o$2b3o5bobo$bo3bo6bo$o5bo$o5bo2$6bo$5b2o$5b2o$3bo2b2o$4bobo$4b2o3$2b2o3b2o$2b2o3b2o2$4b3o$4b3o$5bo6$5b2o$5b2o9$100b2o$100b2o6$118b2o$118b2o2$119bo$98b2o3b2o13bobo$118bobo$99bo3bo15bo$100b3o$100b3o$116b2obob2o$116bo5bo$117bo3bo$118b3o2$99bo5bo$98b3o5bo$98b3o3b3o2$96b2o3b2o$96b2o3b2o14bo$116bo$116b3o4$101b2o$101bo17b5o$102b3o13bob3obo$104bo14bo3bo$120b3o$121bo4$102bo17b2o$101bo18b2o$101b3o2$79b2o$79bobo$80b3o$81b2o$78b2o$78b3o$91b3o$91bo$79b2o11bo$79b2o2$88bo$82b2obobobo$82bob2ob2o10b2o$93b2o4bobo$94b2o3bo$93bo4$118b2o$86bo19b3o2bo5bo3bo$86b2o14bo3bo9bo5bo$74bobo8bobo12bobo4bo8bo3bo2bo$74bo3bo19b2o16bo$64b2o12bo10b2o7b2o17bo3bo3bo$64b2o8bo4bo7bo2bo7b2o18b2o3b2obo$78bo7bo7b2o4bobo23bo$74bo3bo7bo6bo2bo5bo23b2o7b2o$74bobo9bo7b2o39bobo$87bo2bo46bo$89b2o46b2o!"; */
    const char* rle_string = "146b2o5b2o$146b2o5b2o8$147bo5bo$146b3o3b3o$145b2obo3bob2o3$148bo3bo$148bo3bo5$147bo$146bobo$33bo111b2ob2o$32b2o8b2o101bo3bo$18b2o11b3obo5bo2bo99b3ob3o$18b2o10b2o8b2ob3o99bo3bo$31b2o8b2obo100bo3bo$32bo10bo102bobo4b2o$147bo5b2o$32bo$31b2o$18b2o10b2o13b2o30b2o$18b2o11b3obo9b2o30b2o$b2o29b2o$b2o30bo3$41bo2bo$19b4o22bo19b4o$18bo3bo18bo3bo18bo3bo58b2o$22bo19b4o22bo59b2o$18bo2bo42bo2bo59bo24b2o$b3o3b3o142bobo$o2bo3bo2bo143bo$2obo3bob2o69bo73b2o$57bo5bo5b2o8b2o15b2o$21b2o19bob2o11bo5bo5b2o7b2o16b2o$20b2o14b2o3bo2b2o2b3o28b2o2b2o$22bo13b2o3bo6b2o3b2o11b2o$41b2o3b3o$43bo3bo9bo5bo52bo$57bo5bo15b2o2b2o31b2o$43bo3bo21b2o7b2o16b2o17bobo$41b2o3b3o20b2o8b2o15b2o$36b2o3bo6b2o13b2o15bo$36b2o3bo2b2o2b3o12b2o$42bob2o$33bo$32b2o58b2o$32bobo17bo38bo$b2o5b2o41b2o25b2o10bo2b2o$b2o5b2o40b3obo9b2o12b2o10bo2bo18bo$30b2o17b2o13b2o24bobo11b2o4bo2bo$32bo17b2o39b2o12b2o3b5o10b2o$17b2o10b2o2bo17bo52bo5b3ob2o9b2o$17b2o11bo2bo57b2o18b2obo$31bobo17bo38bobo19b2o$31b2o17b2o26b2o10bo2bo$49b2o13b2o12b2o10bo2b2o17b2o$31b2o17b3obo9b2o25bo19b2obo$31bobo17b2o39b2o16b3ob2o9b2o$17b2o11bo2bo18bo57b5o10b2o$17b2o10b2o2bo76bo2bo$32bo79bo$30b2o!";

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

    unsigned int cellFlag = 0;
    unsigned int ssboFlag = rlLoadShaderBuffer(sizeof(unsigned int), &cellFlag, RL_DYNAMIC_COPY);

    unsigned int ssboA = rlLoadShaderBuffer(gol_width * gol_height * sizeof(unsigned int), NULL, RL_DYNAMIC_COPY);
    unsigned int ssboB = rlLoadShaderBuffer(gol_width * gol_height * sizeof(unsigned int), NULL, RL_DYNAMIC_COPY);

    {
        unsigned int initialState[gol_width * gol_height];
        parse_rle(rle_string, initialState, gol_width, gol_height, start_x, start_y);

        rlUpdateShaderBuffer(ssboA, initialState, gol_width * gol_height * sizeof(unsigned int), 0);
    }

    Image whiteImg = GenImageColor(gol_width, gol_height, WHITE);
    Texture whiteTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    uint maxBitCount = 1000;
    uint frameCount = 0;
    uint lastFlag = 0;
    uint bits[maxBitCount];
    uint bitCount = 0;
    /* SetTargetFPS(60); */
    int gameOver = 0;
    char gameOverText[256];

    double startTime = GetTime();
    double totalTime = 0.0;

    while (!WindowShouldClose())
    {
        /* SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())); */

        if (gameOver)
        {
            BeginDrawing();
            EndDrawing();
            continue;
        }

        frameCount++;

        rlEnableShader(computeProgram);
        rlBindShaderBuffer(ssboA, 1);
        rlBindShaderBuffer(ssboB, 2);
        rlBindShaderBuffer(ssboFlag, 3);
        rlComputeShaderDispatch(gol_num_grp_x, gol_num_grp_y, 1);
        rlDisableShader();

        rlReadShaderBuffer(ssboFlag, &cellFlag, sizeof(unsigned int), 0);
        if (cellFlag)
        {
            uint incr = handle_cell_flag(frameCount - lastFlag, bits, bitCount, maxBitCount);
            lastFlag = frameCount;
            bitCount += incr;

            if (incr == 0)
            {
                gameOver = 1;
                write_bits_to_file("bits_2.csv", bits, bitCount);
                totalTime = GetTime() - startTime;
            }

            printf("%d\n", frameCount);
            cellFlag = 0;
            rlUpdateShaderBuffer(ssboFlag, &cellFlag, sizeof(unsigned int), 0);
        }

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
            if (gameOver)
            {
                sprintf(gameOverText, "Game Over. Time: %f s", totalTime);
                DrawText(gameOverText, GetScreenWidth() / 4, GetScreenHeight() / 2, 20, RED);
            }

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
