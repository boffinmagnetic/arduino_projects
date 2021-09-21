#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

#define BLOCK_SIZE 8
Tetromino part;

#define MAP_WIDTH 8
#define MAP_HEIGHT 16
bool map_data[MAP_WIDTH][MAP_HEIGHT] = {0};

uint16_t score;
void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        PrintCenter(score, 0, 1);
        for (uint8_t i = 0; i < SHAPE_SIZE; i++)
            for (uint8_t j = 0; j < SHAPE_SIZE; j++)
                if (part.shape[i][j])
                    display.fillRect((part.x + i) * BLOCK_SIZE, (part.y + j) * BLOCK_SIZE + display.height() - BLOCK_SIZE * MAP_HEIGHT, BLOCK_SIZE, BLOCK_SIZE, 1);
        for (uint8_t x = 0; x < MAP_WIDTH; x++)
            for (uint8_t y = 0; y < MAP_HEIGHT; y++)
                if (map_data[x][y])
                    display.fillRect(x * BLOCK_SIZE, y * BLOCK_SIZE + display.height() - BLOCK_SIZE * MAP_HEIGHT, BLOCK_SIZE, BLOCK_SIZE, 1);
        break;
    case STATE_GAMEOVER:
        DrawGameover(score);
        break;
    }
}

void swap(bool first, bool second)
{
    bool tmp = first;
    first = second;
    second = tmp;
}

void Transpose(bool mat[SHAPE_SIZE][SHAPE_SIZE])
{
    for (uint8_t i = 0; i < SHAPE_SIZE; i++)
        for (uint8_t j = 0; j < SHAPE_SIZE; j++)
            swap(mat[i][j], mat[j][i]);
}
void ReverseRow(bool mat[SHAPE_SIZE][SHAPE_SIZE])
{
    uint8_t tmp;
    for (uint8_t i = 0; i < SHAPE_SIZE; i++)
    {
        tmp = SHAPE_SIZE - 1;
        for (uint8_t j = 0; j < tmp; j++)
        {
            swap(mat[i][j], mat[i][tmp]);
            tmp--;
        }
    }
}

void RotateMatrixCW(bool mat[SHAPE_SIZE][SHAPE_SIZE], bool cw)
{

    uint8_t size = (part.type < 5 ? SHAPE_SIZE - 1 : SHAPE_SIZE);
    for (int8_t x = 0; x < size / 2; x++)
        for (int8_t y = x; y < size - x - 1; y++)
        {
            int temp = mat[x][y];
            if (cw)
            {
                mat[x][y] = mat[y][size - 1 - x];
                mat[y][size - 1 - x] = mat[size - 1 - x][size - 1 - y];
                mat[size - 1 - x][size - 1 - y] = mat[size - 1 - y][x];
                mat[size - 1 - y][x] = temp;
            }
            else
            {
                mat[x][y] = mat[size - 1 - y][x];
                mat[size - 1 - y][x] = mat[size - 1 - x][size - 1 - y];
                mat[size - 1 - x][size - 1 - y] = mat[y][size - 1 - x];
                mat[y][size - 1 - x] = temp;
            }
        }
}
void ControlRotation(bool mat[SHAPE_SIZE][SHAPE_SIZE], bool cw)
{
    for (uint8_t i = 0; i < SHAPE_SIZE; i++)
        for (uint8_t j = 0; j < SHAPE_SIZE; j++)
            if (part.shape[i][j])
                if (map_data[part.x + i][part.y + j] || part.x + i < 0 || part.x + i >= MAP_WIDTH || part.y + j < 0 || part.y + j >= MAP_HEIGHT)
                    RotateMatrixCW(mat, !cw);
}
void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (key_code & (KEY_J | KEY_A | KEY_B))
            StartNewGame();
        break;
    case STATE_PLAY:
        static int8_t side_move_timer = 0, down_move_timer = 0;

        if (key_code & KEY_A)
        {
            RotateMatrixCW(part.shape, false);
            ControlRotation(part.shape, false);
        }
        if (key_code & KEY_B)
        {
            RotateMatrixCW(part.shape, true);
            ControlRotation(part.shape, true);
        }

        if (++side_move_timer == 3)
        {
            side_move_timer = 0;
            if (dir & RIGHT)
            {
                bool flag = false;
                for (uint8_t i = 0; i < SHAPE_SIZE; i++)
                    for (uint8_t j = 0; j < SHAPE_SIZE; j++)
                        if (part.shape[i][j])
                            if (map_data[part.x + i + 1][part.y + j] || part.x + i + 1 >= MAP_WIDTH)
                                flag = true;
                if (!flag)
                    part.x++;
            }
            else if (dir & LEFT)
            {
                bool flag = false;
                for (uint8_t i = 0; i < SHAPE_SIZE; i++)
                    for (uint8_t j = 0; j < SHAPE_SIZE; j++)
                        if (part.shape[i][j])
                            if (map_data[part.x + i - 1][part.y + j] || part.x + i - 1 < 0)
                                flag = true;
                if (!flag)
                    part.x--;
            }
        }

        if (dir & DOWN || ++down_move_timer == 10)
        {
            down_move_timer = 0;
            bool flag = false;
            for (uint8_t i = 0; i < SHAPE_SIZE; i++)
                for (uint8_t j = 0; j < SHAPE_SIZE; j++)
                    if (part.shape[i][j])
                        if (map_data[part.x + i][part.y + j + 1] || part.y + j > MAP_HEIGHT - 2)
                            flag = true;
            if (flag)
                CheckRows();
            else
                part.y++;
        }
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    randomSeed(millis());
    score = 0;
    for (uint8_t x = 0; x < MAP_WIDTH; x++)
        for (uint8_t y = 0; y < MAP_HEIGHT; y++)
            map_data[x][y] = false;
    ResetPart();
}

void ResetPart()
{
    part.x = (MAP_WIDTH - SHAPE_SIZE) / 2;
    part.y = 0;
    part.type = random(7);
    switch (part.type)
    {
    case 0:
        part.copy(shapeT);
        break;
    case 1:
        part.copy(shapeL);
        break;
    case 2:
        part.copy(shapeJ);
        break;
    case 3:
        part.copy(shapeS);
        break;
    case 4:
        part.copy(shapeZ);
        break;
    case 5:
        part.copy(shapeI);
        break;
    case 6:
        part.copy(shapeO);
        break;
    }
}

void CheckRows()
{
    for (uint8_t i = 0; i < SHAPE_SIZE; i++)
        for (uint8_t j = 0; j < SHAPE_SIZE; j++)
            if (part.shape[i][j])
            {
                map_data[part.x + i][part.y + j] = true;
                if (part.y <= 0)
                    game_state = STATE_GAMEOVER;
            }

    uint8_t row_num = 0;
    for (int8_t j = part.y; j < part.y + SHAPE_SIZE; j++)
    {
        bool flag = false;
        for (uint8_t i = 0; i < MAP_WIDTH; i++)
            if (!map_data[i][j])
                flag = true;
        if (!flag)
        {
            row_num++;
            for (int8_t y = j; y >= 0; y--)
                for (uint8_t x = 0; x < MAP_WIDTH; x++)
                    map_data[x][y] = (y < row_num ? false : map_data[x][y - 1]);
        }
    }
    switch (row_num)
    {
    case 1:
        score += 40;
        break;
    case 2:
        score += 100;
        break;
    case 3:
        score += 300;
        break;
    case 4:
        score += 1200;
        break;
    }
    ResetPart();
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Tetris"), 2000);
    display.setRotation(3);
    StartNewGame();
}

void loop()
{
    ControlStep(5, 0);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}