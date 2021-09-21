#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

struct Object
{
    uint8_t x, y;
    uint8_t w, h;
    Object(uint8_t x, uint8_t y, uint8_t w, uint8_t h) : x(x), y(y), w(w), h(h) {}
};

Object light((display.width() - 33) / 2, (display.height() - 54) / 2, 33, 54);

int16_t value = 0;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        if (value < 200)
            display.drawBitmap(light.x, light.y, light_1_pic, light.w, light.h, 1);
        else if (value < 400)
            display.drawBitmap(light.x, light.y, light_2_pic, light.w, light.h, 1);
        else if (value < 600)
            display.drawBitmap(light.x, light.y, light_3_pic, light.w, light.h, 1);
        else if (value < 800)
            display.drawBitmap(light.x, light.y, light_4_pic, light.w, light.h, 1);
        else
            display.drawBitmap(light.x, light.y, light_5_pic, light.w, light.h, 1);
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_PLAY:
        if (game_state == STATE_PLAY)
            value = GetVoltage();
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    value = 0;
}

int GetVoltage()
{
    long average = 0;
    for (int i = 0; i < 16; i++)
    {
        average += analogRead(A2);
        delay(5);
    }
    average >>= 4;
    return (int)average;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F(""), 2000);
    StartNewGame();
    analogReference(DEFAULT);
}

void loop()
{
    ControlStep();
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}