#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

SnakePart head(display.width() / 2, display.height() / 2), food(0, 0);
SnakePart tail[128];

uint8_t score;
uint8_t snake_dir;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        display.drawRect(0, 0, display.width(), display.height(), 1);

        display.fillRect(head.x, head.y, BLOCK_SIZE, BLOCK_SIZE, 1);
        display.drawRoundRect(food.x + 1, food.y + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2, 90, 1);
        display.drawRoundRect(food.x + 2, food.y + 2, BLOCK_SIZE - 4, BLOCK_SIZE - 4, 90, 1);

        for (int i = 0; i < score; i++)
            display.fillRect(tail[i].x, tail[i].y, BLOCK_SIZE, BLOCK_SIZE, 1);

        PrintCenter(score, 2, 1);
        break;

    case STATE_GAMEOVER:
        PrintCenter(F("Game Over!"), 10, 2);

        PrintCenter(F("Score"), 40, 1);
        PrintCenter(score, 50, 1);
        break;
    case STATE_WIN:
        PrintCenter(F("You Win!"), 10, 2);

        PrintCenter(F("score"), 40, 1);
        PrintCenter(score, 50, 1);
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (key_code & (KEY_A | KEY_B))
            StartNewGame();
        break;
    case STATE_WIN:
        if (key_code & (KEY_A | KEY_B))
            StartNewGame();
        break;
    case STATE_PLAY:
        static uint8_t move_timer = 0;
        if (key_code & KEY_A)
            if (snake_dir <= 1)
                snake_dir = 4;
            else
                snake_dir--;
        else if (key_code & KEY_B)
            if (snake_dir >= 4)
                snake_dir = 1;
            else
                snake_dir++;

        move_timer++;
        if (move_timer >= 10)
        {
            move_timer = 0;
            for (int8_t i = score - 1; i > 0; i--)
            {
                tail[i].x = tail[i - 1].x;
                tail[i].y = tail[i - 1].y;
            }
            tail[0].x = head.x;
            tail[0].y = head.y;

            switch (snake_dir)
            {
            case 1:
                head.x += BLOCK_SIZE;
                break;
            case 2:
                head.y += BLOCK_SIZE;
                break;
            case 3:
                head.x -= BLOCK_SIZE;
                break;
            case 4:
                head.y -= BLOCK_SIZE;
                break;
            }

            if (head.x > display.width() - BLOCK_SIZE || head.y > display.height() - BLOCK_SIZE)
                game_state = STATE_GAMEOVER;

            for (int i = 0; i < score; i++)
                if (head.x == tail[i].x && head.y == tail[i].y)
                    game_state = STATE_GAMEOVER;

            if (head.x == food.x && head.y == food.y)
            {
                tail[score].x = head.x;
                tail[score].y = head.y;
                score++;
                if (score > 125)
                    game_state = STATE_WIN;
                ResetFood();
            }
        }
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    score = 0;
    snake_dir = 0;
    head.x = display.width() / 2;
    head.y = display.height() / 2;
    ResetFood();
}

void ResetFood()
{
    randomSeed(millis());
    food.x = random(display.width() / BLOCK_SIZE) * BLOCK_SIZE;
    food.y = random(display.height() / BLOCK_SIZE) * BLOCK_SIZE;
    for (int i = 0; i < score; i++)
        if (food.x == tail[i].x && food.y == tail[i].y)
        {
            ResetFood();
            return;
        }
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Snake"), 2000);
    StartNewGame();
}

void loop()
{
    ControlStep(8, 8);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}