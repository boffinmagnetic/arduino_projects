#include <Adafruit_SH1106.h>
#include <stdio.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object boffin(0, 0, 12, 20);

#define COIN_WIDTH 10
#define COIN_HEIGHT 11

#define LEVEL_SIZE 3
Level level;

#define X_SPEED 2
#define MAX_JUMP_SPEED 9
#define MAX_SCORE 3

bool jump;
int8_t y_vel;
uint8_t level_num;
uint8_t score;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        static uint8_t walk_timer = 0;
        static uint8_t last_walk = RIGHT;

        display.setCursor(0, 0);
        display.setTextSize(1);
        display.println(score);

        for (uint8_t i = 0; i < level.platform_size; i++)
            display.drawRect(level.platform[i].x, level.platform[i].y, level.platform[i].w, level.platform[i].h, 1);
        for (uint8_t i = 0; i < level.coin_size; i++)
            if (level.coin[i].vizible)
                display.drawBitmap(level.coin[i].x, level.coin[i].y, coin_pic, COIN_WIDTH, COIN_HEIGHT, 1);

        if (dir & RIGHT)
        {
            if (jump)
                display.drawBitmap(boffin.x, boffin.y, boffin_jump_right, boffin.w, boffin.h, 1);
            else if (walk_timer % 6 < 3)
                display.drawBitmap(boffin.x, boffin.y, boffin_walk_right_1, boffin.w, boffin.h, 1);
            else
                display.drawBitmap(boffin.x, boffin.y, boffin_walk_right_2, boffin.w, boffin.h, 1);
            walk_timer++;
            last_walk = RIGHT;
        }
        else if (dir & LEFT)
        {
            if (jump)
                display.drawBitmap(boffin.x, boffin.y, boffin_jump_left, boffin.w, boffin.h, 1);
            else if (walk_timer % 6 < 3)
                display.drawBitmap(boffin.x, boffin.y, boffin_walk_left_1, boffin.w, boffin.h, 1);
            else
                display.drawBitmap(boffin.x, boffin.y, boffin_walk_left_2, boffin.w, boffin.h, 1);
            walk_timer++;
            last_walk = LEFT;
        }
        else
        {
            if (last_walk == RIGHT)
            {
                if (jump)
                    display.drawBitmap(boffin.x, boffin.y, boffin_jump_right, boffin.w, boffin.h, 1);
                else
                    display.drawBitmap(boffin.x, boffin.y, boffin_right_pic, boffin.w, boffin.h, 1);
            }
            else
            {
                if (jump)
                    display.drawBitmap(boffin.x, boffin.y, boffin_jump_left, boffin.w, boffin.h, 1);
                else
                    display.drawBitmap(boffin.x, boffin.y, boffin_left_pic, boffin.w, boffin.h, 1);
            }
        }
        break;
    case STATE_GAMEOVER:
        PrintCenter(F("Game Over"), 15, 2);
        PrintCenter(String(score) + String("/") + String(MAX_SCORE), 40, 1);
        display.drawBitmap(75, 38, coin_pic, COIN_WIDTH, COIN_HEIGHT, 1);
        break;
    case STATE_WIN:
        if (level_num <= LEVEL_SIZE)
        {
            PrintCenter(String("Level ") + String(level_num), 5, 2);
            PrintCenter(F("Complete"), 25, 2);
            PrintCenter(String(score) + String("/") + String(MAX_SCORE), 50, 1);
            display.drawBitmap(75, 48, coin_pic, COIN_WIDTH, COIN_HEIGHT, 1);
        }
        else
        {
            PrintCenter(F("You Win!"), 25, 2);
        }
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (key_code & (KEY_A | KEY_B | KEY_J))
            StartNewLevel(false);
        break;
    case STATE_WIN:
        if (key_code & (KEY_A | KEY_B | KEY_J))
        {
            if (level_num < LEVEL_SIZE)
                StartNewLevel(true);
            else if (level_num == LEVEL_SIZE)
                level_num++;
            else if (level_num > LEVEL_SIZE)
                StartNewGame();
        }
        break;
    case STATE_PLAY:
        if (score >= MAX_SCORE)
            game_state = STATE_WIN;
        if (key_code & (KEY_A | KEY_J) && !jump)
        {
            jump = true;
            y_vel = MAX_JUMP_SPEED;
        }

        if (y_vel > -MAX_JUMP_SPEED)
            y_vel--;
        boffin.y -= y_vel;

        if (boffin.y < 0)
            boffin.y = 0;
        else if (boffin.y > display.height())
            game_state = STATE_GAMEOVER;

        if (dir & RIGHT)
            boffin.x += X_SPEED;
        else if (dir & LEFT)
            boffin.x -= X_SPEED;

        for (uint8_t i = 0; i < level.platform_size; i++)
        {
            if (boffin.Collided(level.platform[i]))
            {
                if (boffin.x + boffin.w <= level.platform[i].x + X_SPEED)
                {
                    boffin.x = level.platform[i].x - boffin.w;
                    dir = 0;
                }
                else if (boffin.x >= level.platform[i].x + level.platform[i].w - X_SPEED)
                {
                    boffin.x = level.platform[i].x + level.platform[i].w;
                    dir = 0;
                }
                else if (boffin.y >= level.platform[i].y + level.platform[i].h - y_vel)
                {
                    boffin.y = level.platform[i].y + level.platform[i].h;
                    y_vel = 0;
                }
                else if (boffin.y <= level.platform[i].y + y_vel)
                {
                    boffin.y = level.platform[i].y - boffin.h;
                    jump = false;
                    y_vel = 0;
                }
            }
        }
        for (uint8_t i = 0; i < level.coin_size; i++)
            if (boffin.Collided(Object(level.coin[i].x, level.coin[i].y, COIN_WIDTH, COIN_HEIGHT)) && level.coin[i].vizible)
            {
                level.coin[i].vizible = false;
                score++;
            }
        for (uint8_t i = 0; i < level.platform_size; i++)
            level.platform[i].x += (display.width() - boffin.w) / 2 - boffin.x;
        for (uint8_t i = 0; i < level.coin_size; i++)
            level.coin[i].x += (display.width() - boffin.w) / 2 - boffin.x;

        boffin.x = (display.width() - boffin.w) / 2;
    }
}

void StartNewGame()
{
    level_num = 0;
    StartNewLevel(true);
}

void StartNewLevel(bool iterate)
{
    game_state = STATE_PLAY;
    score = 0;
    y_vel = 0;
    jump = false;

    if (iterate)
    {
        if (level_num >= LEVEL_SIZE)
            game_state = STATE_WIN;
        level.Fill(level_num++);
    }
    else
    {
        level.Reset();
    }

    boffin.x = (display.width() - boffin.w) / 2;
    boffin.y = level.platform[2].y - boffin.w;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Platforms"), 2000);
    StartNewGame();
}

void loop()
{
    ControlStep(5, 0, 0, 0);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}