#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object car(0, 0, 16, 13);

Object tree[3] = {{0, 0, 13, 16}, {0, 0, 13, 16}, {0, 0, 13, 16}};

uint8_t score;
uint8_t timer;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        static uint8_t line_timer = 0;

        display.drawLine(0, 0, 0, display.height(), 1);
        display.drawLine(display.width() - 1, 0, display.width() - 1, display.height() - 1, 1);

        for (uint8_t i = 8 + line_timer; i < display.height(); i += 8)
            display.drawLine(display.width() / 2, i, display.width() / 2, i + 3, 1);

        line_timer += score / 10 + 1;
        if (line_timer >= 8)
            line_timer = 0;

        display.drawBitmap(car.x, car.y, car_pic, car.w, car.h, 1);

        for (uint8_t i = 0; i < 3; i++)
            display.drawBitmap(tree[i].x, tree[i].y, tree_pic, tree[i].w, tree[i].h, 1);

        PrintCenter(score, 0, 1);
        break;
    case STATE_GAMEOVER:
        PrintCenter(F("Game"), 20, 2);
        PrintCenter(F("Over"), 38, 2);
        PrintCenter(F("Score"), 60, 1);
        PrintCenter(score, 70, 1);
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_GAMEOVER:
        if (timer++ > 50)
        {
            StartNewGame();
            timer = 0;
        }
        break;
    case STATE_PLAY:
        car.x += map(analogRead(Y_AXIS), 0, 1023, 6 + score / 2, -6 - score / 2);

        if (car.x < 0)
            car.x = 0;
        if (car.x > display.width() - car.w)
            car.x = display.width() - car.w;

        for (uint8_t i = 0; i < 3; i++)
        {
            tree[i].y += score / 10 + 1;

            if (tree[i].y >= display.height())
            {
                score++;
                TreeReset(i);
            }
            if (car.x < tree[i].x + tree[i].w && car.x + car.w > tree[i].x &&
                car.y < tree[i].y + tree[i].h && car.y + car.h > tree[i].y + tree[i].h - 1)
                game_state = STATE_GAMEOVER;
        }
    }
}

void StartNewGame()
{
    randomSeed(millis());
    game_state = STATE_PLAY;

    score = 0;
    car.x = (display.width() - car.w) / 2;
    car.y = display.height() - car.h;

    TreeReset(0);
    tree[1].y = -((display.height() + tree[1].h) / 3) - tree[1].h;
    tree[2].y = -(2 * ((display.height() + tree[2].h) / 3)) - tree[2].h;
}

void TreeReset(int i)
{

    tree[i].x = random(display.width() - tree[i].w);
    tree[i].y = -tree[i].h;
}
void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Race2"), 2000);
    display.setRotation(3);
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