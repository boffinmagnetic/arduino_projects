#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Question question;
Object bar(10, 35, 108, 6);

#define MAX_NUM 10

String text;

uint8_t score;
uint8_t game_timer;
uint8_t gameover_timer;
Wrong wrong;

void DrawScreen()
{
    display.fillRect(bar.x, bar.y, game_timer, bar.h, 1);

    display.setTextSize(2);
    display.setCursor(0, 49);
    display.println(F("YES"));
    display.setCursor(104, 49);
    display.println(F("NO"));

    PrintCenter(score, 49, 2);
}
void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        display.drawRect(bar.x, bar.y, bar.w, bar.h, 1);
        PrintCenter(text, 6, 2);
        DrawScreen();
        break;
    case STATE_GAMEOVER:
        if (gameover_timer < 50)
        {
            gameover_timer++;
            if (gameover_timer % 10 < 5)
            {
                switch (wrong)
                {
                case WRONG_QUESTION:
                    PrintCenter(text, 6, 2);
                    break;
                case WRONG_TIME:
                    display.drawRect(bar.x, bar.y, bar.w, bar.h, 1);
                    break;
                }
            }
            switch (wrong)
            {
            case WRONG_QUESTION:
                display.drawRect(bar.x, bar.y, bar.w, bar.h, 1);
                break;
            case WRONG_TIME:
                PrintCenter(text, 6, 2);
                break;
            }
            DrawScreen();
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
        if (key_code & (KEY_A | KEY_B) && gameover_timer >= 50)
            StartNewGame();
        break;
    case STATE_PLAY:
        if (--game_timer == 0)
        {
            game_state = STATE_GAMEOVER;
            wrong = WRONG_TIME;
        }
        if ((key_code & KEY_A && question.answer) || (key_code & KEY_B && !question.answer))
        {
            score++;
            GenerateQuestion();
        }
        else if ((key_code & KEY_A && !question.answer) || (key_code & KEY_B && question.answer))
        {
            game_state = STATE_GAMEOVER;
            wrong = WRONG_QUESTION;
        }
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    gameover_timer = 0;
    score = 0;
    GenerateQuestion();
}

void GenerateQuestion()
{
    randomSeed(millis());
    game_timer = 108;
    question.first = random(MAX_NUM);
    question.second = random(MAX_NUM);
    question.answer = random(2);

    switch (random(3))
    {
    case 0:
        question.operation = '+';
        question.result = question.first + question.second;
        break;
    case 1:
        question.operation = '-';
        question.result = question.first - question.second;
        break;
    case 2:
        question.operation = '*';
        question.result = question.first * question.second;
        break;
    }

    if (!question.answer)
    {
        int8_t tmp = question.result;
        while (tmp == question.result)
            question.result = random(MAX_NUM * MAX_NUM);
    }

    text = String(question.first) + String(question.operation) + String(question.second) + " = " + String(question.result);
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Equasions"), 2000);
    StartNewGame();
}

void loop()
{
    ControlStep(10);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}