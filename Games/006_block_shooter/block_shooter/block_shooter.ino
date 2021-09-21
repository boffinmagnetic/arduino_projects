#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object paddle(5, (display.height() - 24) / 2, 24, 6),
    ball(0, 0, 5, 5);

#define BLOCK_SIZE 8

#define BLOCKS_LENGTH 20
#define LEVEL_NUM 5
Block blocks[LEVEL_NUM][BLOCKS_LENGTH] = {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {0, 1}, {4, 1}, {0, 2}, {4, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {0, 4}, {4, 4}, {0, 5}, {4, 5}, {0, 6}, {1, 6}, {2, 6}, {3, 6}},
                                          {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {4, 1}, {0, 2}, {4, 2}, {0, 3}, {4, 3}, {0, 4}, {4, 4}, {0, 5}, {4, 5}, {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}},
                                          {{0, 0}, {0, 0}, {1, 0}, {1, 0}, {2, 0}, {2, 0}, {3, 0}, {3, 0}, {4, 0}, {4, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {0, 4}, {0, 5}, {0, 6}},
                                          {{0, 0}, {0, 0}, {1, 0}, {1, 0}, {2, 0}, {2, 0}, {3, 0}, {3, 0}, {4, 0}, {4, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}},
                                          {{0, 0}, {0, 0}, {4, 0}, {4, 0}, {0, 1}, {0, 1}, {4, 1}, {0, 2}, {1, 2}, {4, 2}, {0, 3}, {2, 3}, {4, 3}, {0, 4}, {3, 4}, {4, 4}, {0, 5}, {4, 5}, {0, 6}, {4, 6}}};

uint8_t level = 0;
uint16_t total_score = 0;
uint8_t score;

float ball_dir_x;
float ball_dir_y;

int8_t ball_speed;

bool starting;

uint32_t start_time;

void Draw()
{
    switch (game_state)
    {
    case STATE_MENU:
        PrintCenter(F("LEVEL"), 20, 2);
        PrintCenter(level + 1, 50, 2);
        break;
    case STATE_PLAY:
        display.drawRect(0, 0, display.width(), display.height(), 1);

        display.fillRoundRect(ball.x, ball.y, ball.w, ball.h, 90, 1);
        display.drawBitmap(paddle.x, paddle.y, paddle_pic, paddle.w, paddle.h, 1);

        for (int i = 0; i < BLOCKS_LENGTH; i++)
            if (blocks[level][i].visible)
                display.fillRect(blocks[level][i].x * BLOCK_SIZE + 12, blocks[level][i].y * BLOCK_SIZE + 15, BLOCK_SIZE, BLOCK_SIZE, 1);

        PrintCenter(score, 2, 1);
        break;
    case STATE_WIN:
        if (level < 5)
        {
            PrintCenter(F("Level"), 20, 2);
            PrintCenter(F("Complete"), 38, 1);
            PrintCenter(F("Score"), 60, 1);
            PrintCenter(score, 69, 1);
            PrintCenter(F("Total"), 85, 1);
            PrintCenter(total_score, 94, 1);
        }
        else
        {
            PrintCenter(F("You"), 20, 2);
            PrintCenter(F("Win"), 38, 2);
            PrintCenter(F("Score"), 85, 1);
            PrintCenter(total_score, 94, 1);
        }
        break;
    case STATE_GAMEOVER:
        PrintCenter(F("Game"), 20, 2);
        PrintCenter(F("Over"), 38, 2);
        PrintCenter(F("Score"), 85, 1);
        PrintCenter(total_score, 94, 1);
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_MENU:
        if (key_code & (KEY_A | KEY_B))
            StartNewGame();
        break;
    case STATE_WIN:
        if (key_code & (KEY_A | KEY_B) && level < 5)
            game_state = STATE_MENU;
        break;
    case STATE_GAMEOVER:
        if (key_code & (KEY_A | KEY_B))
        {
            level = 0;
            total_score = 0;
            game_state = STATE_MENU;
        }
        break;
    case STATE_PLAY:
        paddle.x = map(analogRead(Y_AXIS), 0, 1023, display.width() - paddle.w, 0);
        if (starting)
        {
            if (key_code & (KEY_A | KEY_B))
            {
                ball_dir_y = -1.0f;
                starting = false;
                start_time = millis();
            }
            ball.x = paddle.x + 10;
        }
        else
        {
            bool tmp = false;
            for (int i = 0; i < BLOCKS_LENGTH; i++)
                if (blocks[level][i].visible)
                    tmp = true;

            if (!tmp)
            {
                game_state = STATE_WIN;
                if (level == 4)
                    display.invertDisplay(false);
                level++;
                total_score += score;
                return;
            }
            if (score != 0)
            {
                if (100 - (millis() - start_time) / 1000 == 0)
                    score = 0;
                else
                    score = 100 - (millis() - start_time) / 1000;
            }
        }

        if (ball.y < paddle.y + paddle.h &&
            ball.y + ball.h > paddle.y &&
            ball.x < paddle.x + paddle.w &&
            ball.x + ball.w > paddle.x)
        {
            ball.y = paddle.y - ball.h;
            float t = ((ball.x + ball.w / 2 - paddle.x) / paddle.w) - 0.5f;
            ball_dir_y = -abs(ball_dir_y);
            ball_dir_x = t;
        }

        for (int i = 0; i < BLOCKS_LENGTH; i++)
        {
            if (blocks[level][i].visible)
                if (ball.y < blocks[level][i].y * BLOCK_SIZE + 15 + BLOCK_SIZE &&
                    ball.y + ball.h > blocks[level][i].y * BLOCK_SIZE + 15 &&
                    ball.x < blocks[level][i].x * BLOCK_SIZE + 12 + BLOCK_SIZE &&
                    ball.x + ball.w > blocks[level][i].x * BLOCK_SIZE + 12)
                {
                    if (abs((ball.x - ball_dir_x * ball_speed + (ball.w / 2) - (blocks[level][i].x * BLOCK_SIZE + 12 + BLOCK_SIZE / 2))) > abs((ball.y - ball_dir_y * ball_speed + (ball.h / 2) - (blocks[level][i].y * BLOCK_SIZE + 15 + BLOCK_SIZE / 2))))
                        ball_dir_x = -ball_dir_x;
                    else
                        ball_dir_y = -ball_dir_y;
                    blocks[level][i].visible = false;
                    break;
                }
        }

        if (ball.y < 1)
            ball_dir_y = -ball_dir_y;
        else if (ball.y > display.height())
        {
            game_state = STATE_GAMEOVER;
            if (level == 4)
                display.invertDisplay(false);
        }

        if (ball.x < 1)
        {
            ball.x = 1;
            ball_dir_x = abs(ball_dir_x);
        }
        else if (ball.x > display.width() - ball.w - 1)
        {
            ball.x = display.width() - ball.w - 1;
            ball_dir_x = -abs(ball_dir_x);
        }

        ball.x += ball_dir_x * ball_speed;
        ball.y += ball_dir_y * ball_speed;
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;

    starting = true;
    score = 100;
    paddle.x = (display.width() - paddle.w) / 2;
    paddle.y = display.height() - paddle.h - 1;

    if (level == 4)
        display.invertDisplay(true);

    for (int i = 0; i < BLOCKS_LENGTH; i++)
        blocks[level][i].visible = true;

    ResetBall();
}

void ResetBall()
{
    ball.x = (display.width() - paddle.w) / 2;
    ball.y = display.height() - paddle.h - ball.h - 2;
    ball_dir_x = 0;
    ball_dir_y = 0;
    ball_speed = 2;
}
void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo("Block Shooter", 2000);
    display.setRotation(3);
    game_state = STATE_MENU;
}

void loop()
{
    ControlStep(5, 0);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}