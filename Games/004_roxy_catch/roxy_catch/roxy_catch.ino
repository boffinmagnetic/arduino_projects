#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object dog((display.width() - 20) / 2, (display.height() - 20) / 2, 20, 20),
    meat(0, 0, 10, 10),
    heart(display.width() - 9, 2, 7, 6),
    bomb(0, 0, 8, 9);

uint8_t score;
uint8_t lives;
uint8_t treat;

bool eat;
bool hurt;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        static uint8_t walk_timer = 0, hurt_timer = 0, eat_timer = 0;
        static bool last_walk_dir = false;

        display.drawRect(0, 0, display.width(), display.height(), 1);

        for (int i = 0; i < 3; i++)
            if (lives > i)
                display.drawBitmap(heart.x - i * (heart.w + 2), heart.y, heart_filled, heart.w, heart.h, 1);
            else
                display.drawBitmap(heart.x - i * (heart.w + 2), heart.y, heart_blank, heart.w, heart.h, 1);

        if (hurt)
        {
            if (hurt_timer % 6 < 3)
            {
                if (last_walk_dir)
                    display.drawBitmap(dog.x, dog.y, dog_right, dog.w, dog.h, 1);
                else
                    display.drawBitmap(dog.x, dog.y, dog_left, dog.w, dog.h, 1);
            }
            hurt_timer++;
            if (hurt_timer > 20)
            {
                hurt = false;
                hurt_timer = 0;
            }
        }
        else if (eat)
        {
            if (eat_timer % 6 < 3)
                display.drawBitmap(dog.x, dog.y, dog_eat_1, dog.w, dog.h, 1);
            else
                display.drawBitmap(dog.x, dog.y, dog_eat_2, dog.w, dog.h, 1);
            eat_timer++;
            if (eat_timer > 10)
            {
                eat = false;
                eat_timer = 0;
            }
        }
        else if (dir & RIGHT || (last_walk_dir && dir & (UP | DOWN) && !(dir & LEFT)))
        {
            if (walk_timer % 6 < 3)
                display.drawBitmap(dog.x, dog.y, dog_walk_right_1, dog.w, dog.h, 1);
            else
                display.drawBitmap(dog.x, dog.y, dog_walk_right_2, dog.w, dog.h, 1);
            last_walk_dir = true;
            walk_timer++;
        }
        else if (dir & LEFT || (!last_walk_dir && dir & (UP | DOWN) && !(dir & RIGHT)))
        {
            if (walk_timer % 6 < 3)
                display.drawBitmap(dog.x, dog.y, dog_walk_left_1, dog.w, dog.h, 1);
            else
                display.drawBitmap(dog.x, dog.y, dog_walk_left_2, dog.w, dog.h, 1);
            last_walk_dir = false;
            walk_timer++;
        }
        else
        {
            if (last_walk_dir)
                display.drawBitmap(dog.x, dog.y, dog_right, dog.w, dog.h, 1);
            else
                display.drawBitmap(dog.x, dog.y, dog_left, dog.w, dog.h, 1);
        }

        if (treat == 0)
            display.drawBitmap(meat.x, meat.y, meat_pic, meat.w, meat.h, 1);
        else if (treat == 1)
            display.drawBitmap(meat.x, meat.y, heart_filled, heart.w, heart.h, 1);
        else if (treat == 2)
            display.drawBitmap(meat.x, meat.y, bomb_pic, bomb.w, bomb.h, 1);

        display.setTextSize(1);
        display.setCursor(2, 2);
        display.println(score);
        break;

    case STATE_GAMEOVER:
        PrintCenter(F("Game Over!"), 10, 2);
        PrintCenter(F("Score"), 40, 1);
        PrintCenter(score, 50, 1);
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (key_code & KEY_J)
            StartNewGame();
        break;
    case STATE_PLAY:
        if (lives == 0)
            game_state = STATE_GAMEOVER;

        float cur_speed = score / 10.0 + 2;
        meat.y += score / 10.0 + 1;

        if (!eat && !hurt)
        {
            if (dir & LEFT)
                dog.x -= cur_speed;
            else if (dir & RIGHT)
                dog.x += cur_speed;
            if (dir & UP)
                dog.y -= cur_speed;
            else if (dir & DOWN)
                dog.y += cur_speed;
        }

        if (dog.x < 1)
            dog.x = 1;
        else if (dog.x > display.width() - dog.w - 1)
            dog.x = display.width() - dog.w - 1;
        if (dog.y < 1)
            dog.y = 1;
        else if (dog.y > display.height() - dog.h - 1)
            dog.y = display.height() - dog.h - 1;

        if (meat.y >= display.height())
            MeatReset();

        if (dog.x < meat.x + meat.w && dog.x + dog.w > meat.x &&
            dog.y < meat.y + meat.h && dog.y + dog.h > meat.y)
        {
            switch (treat)
            {
            case 0:
                score++;
                eat = true;
                break;
            case 1:
                if (lives < 3)
                    lives++;
                eat = true;
                break;
            case 2:
                if (!hurt)
                {
                    lives--;
                    hurt = true;
                }
                break;
            }
            MeatReset();
        }
    }
}
void MeatReset()
{
    randomSeed(millis());
    meat.x = random(display.width() - meat.w);
    meat.y = -meat.h;

    int val = random(6);
    if (val < 4)
        treat = 0;
    else if (val < 5)
        if (lives == 3)
            MeatReset();
        else
            treat = 1;
    else if (val < 6)
        treat = 2;
}

void StartNewGame()
{
    game_state = STATE_PLAY;

    dog.x = (display.width() - dog.w) / 2;
    dog.y = (display.height() - dog.h) / 2;

    score = 0;
    lives = 3;
    eat = false;
    hurt = false;

    MeatReset();
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("RoxyCatch"), 2000);
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