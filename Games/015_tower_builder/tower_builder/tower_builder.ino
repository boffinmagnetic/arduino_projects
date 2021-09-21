#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

#define PLANKS_SIZE 40
#define PLANK_HEIGHT 3
#define PLANK_STARTING_WIDTH 40
Object planks[PLANKS_SIZE];

uint8_t current_plank;
uint8_t win_timer;
uint16_t score;
Dir going_from;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        for (uint8_t i = 0; i <= current_plank; i++)
            display.fillRect(planks[i].x, planks[i].y, planks[i].w, planks[i].h, 1);

        PrintCenter(score, 2, 1);
        break;
    case STATE_GAMEOVER:
        PrintCenter(F("Game"), 10, 2);
        PrintCenter(F("Over"), 30, 2);
        PrintCenter(F("score"), 60, 1);
        PrintCenter(score, 70, 1);
        break;
    case STATE_WIN:
        if (win_timer < 60)
        {
            win_timer++;
            for (uint8_t i = 0; i <= current_plank; i++)
                display.fillRect(planks[i].x, planks[i].y, planks[i].w, planks[i].h, 1);
        }
        else
        {
            PrintCenter(F("You"), 10, 2);
            PrintCenter(F("Win"), 30, 2);
            PrintCenter(F("score"), 60, 1);
            PrintCenter(score, 70, 1);
        }
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (key_code & (KEY_A | KEY_J))
            StartNewGame();
        break;
    case STATE_WIN:
        for (uint8_t i = 0; i <= current_plank; i++)
            i % 2 ? planks[i].x++ : planks[i].x--;

        if (key_code & (KEY_A | KEY_J))
            StartNewGame();
        break;
    case STATE_PLAY:
        if (going_from == RIGHT)
        {
            planks[current_plank].x--;
            if (planks[current_plank].x + planks[current_plank].w < planks[current_plank - 1].x)
                game_state = STATE_GAMEOVER;
        }
        else if (going_from == LEFT)
        {
            planks[current_plank].x++;
            if (planks[current_plank].x > planks[current_plank - 1].x + planks[current_plank - 1].w)
                game_state = STATE_GAMEOVER;
        }

        if (key_code & (KEY_A | KEY_J))
        {
            if (planks[current_plank].x < planks[current_plank - 1].x)
            {
                planks[current_plank].w -= planks[current_plank - 1].x - planks[current_plank].x;
                planks[current_plank].x = planks[current_plank - 1].x;
            }
            else if (planks[current_plank].x > planks[current_plank - 1].x)
            {
                planks[current_plank].w -= planks[current_plank].x + planks[current_plank].w - planks[current_plank - 1].x - planks[current_plank - 1].w;
            }

            if (planks[current_plank].w < 1)
                game_state = STATE_GAMEOVER;

            score += (planks[current_plank].w > 0 ? planks[current_plank].w : 0);
            if (current_plank + 1 >= PLANKS_SIZE)
                game_state = STATE_WIN;

            if (game_state != STATE_PLAY)
                return;

            SpawnPlank();
        }
        break;
    }
}

void SpawnPlank()
{
    randomSeed(millis());
    going_from = random() % 2 ? LEFT : RIGHT;
    current_plank++;
    if (going_from == RIGHT)
        planks[current_plank] = {display.width(), planks[current_plank - 1].y - PLANK_HEIGHT, planks[current_plank - 1].w, planks[current_plank - 1].h};
    else if (going_from == LEFT)
        planks[current_plank] = {-planks[current_plank - 1].w, planks[current_plank - 1].y - PLANK_HEIGHT, planks[current_plank - 1].w, planks[current_plank - 1].h};
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    win_timer = 0;
    score = 0;
    for (int i = 1; i < PLANKS_SIZE; i++)
        planks[i] = {0, 0, 0, 0};
    current_plank = 0;
    planks[current_plank] = {(display.width() - PLANK_STARTING_WIDTH) / 2, display.height() - PLANK_HEIGHT, PLANK_STARTING_WIDTH, PLANK_HEIGHT};
    SpawnPlank();
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo("Tower Builder", 2000);
    display.setRotation(3);
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