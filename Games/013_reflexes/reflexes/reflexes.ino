#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

bool led_on = false;
WIN winner = WIN_NONE;

uint32_t reaction_time;
bool key;
void Draw()
{
    switch (game_state)
    {
    case STATE_MENU:
        digitalWrite(LED_PIN, led_on ? HIGH : LOW);
        PrintCenter(F("Get Ready"), 5, 2);
        PrintCenter(F("and"), 23, 2);
        PrintCenter(F("Watch LED"), 41, 2);
        break;
    case STATE_PLAY:
        digitalWrite(LED_PIN, led_on ? HIGH : LOW);
        break;
    case STATE_GAMEOVER:
        digitalWrite(LED_PIN, led_on ? HIGH : LOW);
        switch (winner)
        {
        case WIN_TIE:
            PrintCenter(F("TIE"), 9, 2);
            break;
        case WIN_LEFT:
            PrintCenter(F("LEFT"), 0, 2);
            PrintCenter(F("WINS"), 18, 2);
            break;
        case WIN_RIGHT:
            PrintCenter(F("RIGHT"), 0, 2);
            PrintCenter(F("WINS"), 18, 2);
            break;
        }
        PrintCenter(F("Reaction time"), 40, 1);
        PrintCenter(String(reaction_time) + String(" ms"), 50, 1);
        break;
    }
}

void GameStep()
{
    static uint8_t start_timer = 0;
    switch (game_state)
    {
    case STATE_MENU:
        if (start_timer > 50)
        {
            start_timer = 0;
            game_state = STATE_PLAY;
        }
        start_timer++;
        break;
    case STATE_GAMEOVER:
        if (key_code & KEY_B || key)
            game_state = STATE_MENU;
        break;
    case STATE_PLAY:
        static uint32_t start_millis;
        if (!led_on)
        {
            if (random(100) == 1)
            {
                led_on = true;
                start_millis = millis();
            }
            if (key_code & KEY_B || key)
                game_state = STATE_MENU;
        }
        else
        {
            if (key_code & KEY_B && key)
                winner = WIN_TIE;
            else if (key_code & KEY_B)
                winner = WIN_LEFT;
            else if (key)
                winner = WIN_RIGHT;

            if (key_code & KEY_B || key)
            {
                reaction_time = millis() - start_millis;
                randomSeed(reaction_time);
                led_on = false;
                game_state = STATE_GAMEOVER;
            }
        }
        break;
    }
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Reaction"), 2000);
    pinMode(0, INPUT_PULLUP);
    game_state = STATE_MENU;
}

void loop()
{
    static uint8_t slow_key = 0;
    key = false;

    if (slow_key < 5)
        slow_key++;
    else if (!digitalRead(0))
    {
        key = true;
        slow_key = 0;
    }
    ControlStep(5);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}