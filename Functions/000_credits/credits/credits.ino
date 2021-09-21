#include <Adafruit_SH1106.h>
#include <EEPROM.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object name(0, 0, 0, 0),
    letter_select(0, 0, 9, 11);

String name_text;
uint8_t current_name;
uint8_t name_font_size;

char owner[11] = "__________";
uint8_t letter;

uint8_t cursor;

bool owner_set;

void (*resetFunc)(void) = 0;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        display.setTextSize(name_font_size);
        display.setCursor(name.x, name.y);
        display.println(name_text);
        break;
    case STATE_MENU:
        PrintCenter(F("Enter Your Name"), 0, 1);

        PrintCenter(owner, 12, 1);

        for (uint8_t i = letter; i < letter + 26; i++)
        {
            if (i < letter + 13)
                display.setCursor((i - letter) * 9 + 2, 30);
            else
                display.setCursor((i - (letter + 13)) * 9 + 2, 41);
            display.println((char)i);
        }

        if (letter_select.y < 2)
            display.drawRect(0 + (letter_select.x * letter_select.w), 28 + (letter_select.y * letter_select.h), letter_select.w, letter_select.h, 1);
        else
            display.drawLine(display.width() - 4 * 6 - 2, display.height() - 1, display.width(), display.height() - 1, 1);

        if (cursor > 0)
        {
            display.setCursor(display.width() - 4 * 6, display.height() - 10);
            display.println(F("Done"));
        }
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_PLAY:
        static bool joy_connected = false;
        static uint8_t joy_connected_timer = 0;

        if (!joy_connected)
        {
            if (!dir)
            {
                if (joy_connected_timer++ > 30)
                    joy_connected = true;
            }
            else
            {
                joy_connected_timer = 0;
            }
        }

        if (joy_connected && dir)
        {
            Achievement(true);

            if (dir & LEFT)
                name.x -= SPEED;
            else if (dir & RIGHT)
                name.x += SPEED;
            if (dir & UP)
                name.y -= SPEED;
            else if (dir & DOWN)
                if (name.y < display.height() - name.h - SPEED)
                    name.y += SPEED;

            if (name.x < 0)
            {
                name.x = display.width() - name_font_size * 6;
                name.y -= name.h + 2;
            }
            else if (name.x > display.width() - name_font_size * 6)
            {
                name.x = 0;
                name.y += name.h + 2;
            }
        }
        else
        {
            name.y--;
        }

        if (name.y < -name.h)
            NewName();

        if (key_code & KEY_A)
        {
            Achievement(false);
            name_font_size = 2;
            CalculateNewName();
        }
        else if (key_code & KEY_B)
        {
            name_font_size = 1;
            CalculateNewName();
        }
        else if (key_code & KEY_Y)
            ResetEeprom();
        break;
    case STATE_MENU:
        if (key_code & KEY_J)
            letter == 65 ? letter = 97 : letter = 65;

        if (dir & LEFT)
            letter_select.x--;
        else if (dir & RIGHT)
            letter_select.x++;
        if (dir & UP)
            letter_select.y--;
        else if (dir & DOWN)
            letter_select.y++;

        if (letter_select.x > 12)
            letter_select.x = 12;
        else if (letter_select.x < 0)
            letter_select.x = 0;

        if (letter_select.y > 2)
            letter_select.y = 2;
        else if (letter_select.y < 0)
            letter_select.y = 0;

        if (letter_select.y > 1 && cursor < 1)
            letter_select.y = 1;

        if (letter_select.y < 2)
        {
            if (key_code & KEY_A && cursor < 10)
            {
                owner[cursor] = letter + letter_select.x + (letter_select.y * 13);
                cursor++;
            }
        }
        else
        {
            if (key_code & KEY_A)
            {
                game_state = STATE_PLAY;
                owner[cursor] = '\0';
                EEPROM.put(3 * sizeof(bool), true);
                EEPROM.put(4 * sizeof(bool), owner);
                owner_set = true;
                resetFunc();
            }
        }

        if (key_code & KEY_B && cursor > 0)
            owner[--cursor] = '_';
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;

    name_font_size = 1;
    letter = 65;
    cursor = 0;
    owner_set = false;

    EEPROM.get(3 * sizeof(bool), owner_set);
    if (owner_set)
    {
        char check[6];
        EEPROM.get(4 * sizeof(bool) + sizeof(owner), check);
        if (strcmp(check, "check\0"))
            ResetEeprom();
        current_name = 0;
        EEPROM.get(4 * sizeof(bool), owner);
    }
    else
        current_name = 2;
    NewName();
}

void NewName()
{
    if (current_name == 1 && owner_set)
        name_text = owner;
    else
        name_text = names[current_name];

    CalculateNewName();

    name.x = (display.width() - name.w) / 2;
    name.y = display.height();

    if (current_name < NAMES_SIZE - 1)
        current_name++;
    else
        current_name = (owner_set ? 0 : 2);
}

void CalculateNewName()
{
    name.w = name_text.length() * 6 * name_font_size;
    if (name.w > display.width())
        name.w = display.width();
    name.h = 6 * name_font_size;
}

void Achievement(bool achivement_num)
{
    bool tmp1;
    bool tmp2;
    bool tmp3;
    EEPROM.get(1 * sizeof(bool), tmp1);
    EEPROM.get(2 * sizeof(bool), tmp2);
    EEPROM.get(3 * sizeof(bool), tmp3);
    if (!achivement_num && !tmp1)
    {
        EEPROM.put(1 * sizeof(bool), true);
        tmp1 = true;
        display.clearDisplay();
        PrintCenter(F("Found 1. easter egg"), 25, 1);
        display.display();
        delay(1000);
    }
    else if (achivement_num && !tmp2)
    {
        EEPROM.put(2 * sizeof(bool), true);
        tmp2 = true;
        display.clearDisplay();
        PrintCenter(F("Found 2. easter egg"), 25, 1);
        display.display();
        delay(1000);
    }
    if (tmp1 && tmp2 && !tmp3)
        game_state = STATE_MENU;
}

void ResetEeprom()
{
    EEPROM.put(1 * sizeof(bool), false);
    EEPROM.put(2 * sizeof(bool), false);
    EEPROM.put(3 * sizeof(bool), false);
    EEPROM.put(4 * sizeof(bool) + sizeof(owner), "check\0");

    strncpy(owner, "__________", 10);
    owner_set = false;
    letter_select.x = letter_select.y = 0;
    cursor = 0;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Credits"), 2000);
    StartNewGame();
}

void loop()
{
    if (game_state == STATE_MENU)
        ControlStep(5, 2);
    else if (game_state == STATE_PLAY)
        ControlStep(5, 0);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}