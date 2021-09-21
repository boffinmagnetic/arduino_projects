#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object background_1(0, 5, background_1_pic, display.width(), 30),
    background_2(display.width(), 5, background_2_pic, display.width(), 30),
    boffin(10, display.height() - 42, boffin_pic, 14, 42);

Object block[3] = {{0, 0, NULL, 0, 0},
                   {0, 0, NULL, 0, 0},
                   {0, 0, NULL, 0, 0}};

uint8_t score;

uint8_t gameover_timer;
bool jump;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        static uint8_t walk_timer = 0;
        display.drawLine(0, display.height() - 1, display.width(), display.height() - 1, 1);

        display.drawBitmap(background_1.x, background_1.y, background_1.pic, background_1.w, background_1.h, 1);
        display.drawBitmap(background_2.x, background_2.y, background_2.pic, background_2.w, background_2.h, 1);

        display.drawBitmap(boffin.x, boffin.y, (jump ? boffin_jump_pic : (walk_timer++ % 6 < 3 ? boffin_walk_1_pic : boffin_walk_2_pic)), boffin.w, boffin.h, 1);

        for (uint8_t i = 0; i < 3; i++)
            display.drawBitmap(block[i].x, block[i].y, block[i].pic, block[i].w, block[i].h, 1);

        display.setCursor(2, 2);
        display.setTextSize(1);
        display.println(score);
        break;

    case STATE_GAMEOVER:
        if (gameover_timer < 50)
        {
            for (int i = 0; i < 3; i++)
                display.drawBitmap(block[i].x, block[i].y, block[i].pic, block[i].w, block[i].h, 1);
            if (gameover_timer < 10)
                display.drawBitmap(boffin.x, boffin.y, boffin.pic, boffin.w, boffin.h, 1);
            else if (gameover_timer < 20)
                display.drawBitmap(boffin.x, boffin.y, boffin_head_turn, boffin.w, boffin.h, 1);
            else if (gameover_timer < 25)
                display.drawBitmap(boffin.x, boffin.y, boffin.pic, boffin.w, boffin.h, 1);
            else if (gameover_timer < 30)
                display.drawBitmap(boffin.x, boffin.y, boffin_die_1_pic, boffin.w, boffin.h, 1);
            else if (gameover_timer < 35)
                display.drawBitmap(boffin.x, boffin.y, boffin_die_2_pic, boffin.w, boffin.h, 1);
            else if (gameover_timer < 40)
                display.drawBitmap(boffin.x, boffin.y, boffin_die_3_pic, boffin.w, boffin.h, 1);
            gameover_timer++;
        }
        else
        {
            PrintCenter(F("Game Over!"), 10, 2);
            PrintCenter(F("Score"), 40, 1);
            PrintCenter(score, 50, 1);
        }
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (key_code & KEY_B && gameover_timer >= 50)
            StartNewGame();
        break;
    case STATE_PLAY:
        static int8_t y_vel = 0;

        background_1.x--;
        background_2.x--;
        if (background_1.x < -background_1.w)
            background_1.x = display.width();
        if (background_2.x < -background_2.w)
            background_2.x = display.width();

        if (key_code & KEY_A && !jump)
        {
            jump = true;
            y_vel = 8;
        }

        if (jump)
        {
            boffin.y -= y_vel--;
            if (boffin.y >= display.height() - boffin.h + 1)
            {
                y_vel = 0;
                jump = false;
                boffin.y = display.height() - boffin.h;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            block[i].x -= BLOCK_SPEED;
            if (block[i].x < -block[i].w)
            {
                BlockReset(i, false);
                score++;
            }
            if (boffin.x < block[i].x + block[i].w && boffin.x + boffin.w > block[i].x &&
                boffin.y < block[i].y + block[i].h && boffin.y + boffin.h > block[i].y)
                game_state = STATE_GAMEOVER;
        }
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    score = 0;
    gameover_timer = 0;
    jump = false;
    boffin.x = 10;
    boffin.y = display.height() - boffin.h;

    for (int i = 0; i < 3; i++)
        BlockReset(i, true);
}

void BlockReset(uint8_t i, bool start)
{
    randomSeed(millis());

    uint8_t block_type = random(6);
    uint8_t gap = random(70, 120);
    switch (block_type)
    {
    case 0:
        block[i].w = block[i].h = 10;
        block[i].pic = block_10x10_1_pic;
        break;
    case 1:
        block[i].w = block[i].h = 10;
        block[i].pic = block_10x10_2_pic;
        break;
    case 2:
        block[i].w = 10;
        block[i].h = 20;
        block[i].pic = block_10x20_1_pic;
        break;
    case 3:
        block[i].w = 20;
        block[i].h = 10;
        block[i].pic = block_20x10_1_pic;
        break;
    case 4:
        block[i].w = block[i].h = 20;
        block[i].pic = block_20x20_1_pic;
        break;
    case 5:
        block[i].w = block[i].h = 20;
        block[i].pic = block_20x20_2_pic;
        break;
    }
    if (start)
        block[i].x = display.width() + (gap * i);
    else
        block[i].x = block[(i + 2) % 3].x + gap + block[i].w;
    block[i].y = display.height() - block[i].h;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Jump"), 2000);
    StartNewGame();
}

void loop()
{
    ControlStep();
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}