#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);
uint16_t score;

#include "config.h"
#include "display.h"
#include "buttons.h"

#define BLINK_CATCH 10

int8_t player_pos;
int8_t lives;
bool treasure;
bool tentacle1_dir;
int8_t tentacle1_pos;
int8_t tentacle2_pos;
int8_t tentacle3_pos;
int8_t tentacle4_pos;
uint8_t death_timer;
void Draw()
{
    switch (game_state)
    {
    case STATE_MENU:
        break;
    case STATE_PLAY:
        /* Background */
        display.drawBitmap(0, 0, background_pic, display.width(), display.height(), 1);

        /* Lives */
        if (lives > 2)
            display.drawBitmap(19, 2, lives_pic, 7, 9, 1);
        if (lives > 1)
            display.drawBitmap(27, 2, lives_pic, 7, 9, 1);

        /* Player */
        if (death_timer == 0)
        {
            switch (player_pos)
            {
            case 0:
                if (treasure)
                    display.drawBitmap(4, 1, player_pos1_treasure_pic, 10, 14, 1);
                else
                    display.drawBitmap(4, 1, player_pos1_pic, 8, 14, 1);
                break;
            case 1:
                if (treasure)
                    display.drawBitmap(7, 21, player_pos2_treasure_pic, 9, 14, 1);
                else
                    display.drawBitmap(7, 21, player_pos2_pic, 8, 14, 1);
                break;
            case 2:
                if (treasure)
                    display.drawBitmap(19, 36, player_pos3_treasure_pic, 10, 14, 1);
                else
                    display.drawBitmap(19, 36, player_pos3_pic, 8, 14, 1);
                break;
            case 3:
                if (treasure)
                    display.drawBitmap(36, 44, player_pos4_treasure_pic, 9, 14, 1);
                else
                    display.drawBitmap(36, 44, player_pos4_pic, 7, 14, 1);
                break;
            case 4:
                if (treasure)
                    display.drawBitmap(59, 46, player_pos5_treasure_pic, 9, 14, 1);
                else
                    display.drawBitmap(59, 46, player_pos5_pic, 7, 14, 1);
                break;
            case 5:
                display.drawBitmap(83, 45, player_pos6_treasure_pic, 10, 14, 1);
                break;
            }
        }
        /* Tentacle 1 */
        if (tentacle1_pos > 0)
            display.drawBitmap(62, 19, tentacle1_pos1_pic, 13, 7, 1);
        if (tentacle1_pos > 1)
            display.drawBitmap(47, 20, tentacle1_pos2_pic, 14, 7, 1);
        if (tentacle1_pos > 2)
            if (tentacle1_dir)
                display.drawBitmap(32, 24, tentacle1_pos3_up_pic, 16, 8, 1);
            else
                display.drawBitmap(40, 25, tentacle1_pos3_down_pic, 8, 9, 1);
        if (tentacle1_pos > 3)
            if (tentacle1_dir)
                display.drawBitmap(20, 25, tentacle1_pos4_up_pic, 12, 7, 1);
            else
                display.drawBitmap(31, 34, tentacle1_pos4_down_pic, 12, 7, 1);
        if (tentacle1_pos > 4)
        {
            if (tentacle1_dir)
            {
                if (player_pos == 1)
                {
                    if (death_timer % BLINK_CATCH < BLINK_CATCH / 2)
                        display.drawBitmap(4, 20, tentacle1_pos5_up_catch_pic, 15, 17, 1);
                }
                else
                {
                    display.drawBitmap(2, 21, tentacle1_pos5_up_pic, 18, 12, 1);
                }
            }
            else
            {
                if (player_pos == 2)
                {
                    if (death_timer % BLINK_CATCH < BLINK_CATCH / 2)
                        display.drawBitmap(20, 35, tentacle1_pos5_down_catch_pic, 15, 16, 1);
                }
                else
                {
                    display.drawBitmap(19, 40, tentacle1_pos5_down_pic, 13, 13, 1);
                }
            }
        }

        /* Tentacle 2 */
        if (tentacle2_pos > 0)
            display.drawBitmap(72, 29, tentacle2_pos1_pic, 11, 6, 1);
        if (tentacle2_pos > 1)
            display.drawBitmap(61, 30, tentacle2_pos2_pic, 10, 7, 1);
        if (tentacle2_pos > 2)
            display.drawBitmap(51, 33, tentacle2_pos3_pic, 9, 10, 1);
        if (tentacle2_pos > 3)
            display.drawBitmap(43, 40, tentacle2_pos4_pic, 9, 7, 1);
        if (tentacle2_pos > 4)
        {
            if (player_pos == 3)
            {
                if (death_timer % BLINK_CATCH < BLINK_CATCH / 2)
                    display.drawBitmap(35, 43, tentacle2_pos5_catch_pic, 14, 15, 1);
            }
            else
            {
                display.drawBitmap(37, 44, tentacle2_pos5_pic, 7, 14, 1);
            }
        }

        /* Tentacle 3 */
        if (tentacle3_pos > 0)
            display.drawBitmap(88, 33, tentacle3_pos1_pic, 10, 7, 1);
        if (tentacle3_pos > 1)
            display.drawBitmap(81, 37, tentacle3_pos2_pic, 8, 6, 1);
        if (tentacle3_pos > 2)
            display.drawBitmap(73, 39, tentacle3_pos3_pic, 7, 5, 1);
        if (tentacle3_pos > 3)
            display.drawBitmap(65, 41, tentacle3_pos4_pic, 8, 6, 1);
        if (tentacle3_pos > 4)
        {
            if (player_pos == 4)
            {
                if (death_timer % BLINK_CATCH < BLINK_CATCH / 2)
                    display.drawBitmap(58, 45, tentacle3_pos5_catch_pic, 16, 14, 1);
            }
            else
            {
                display.drawBitmap(51, 45, tentacle3_pos5_pic, 14, 13, 1);
            }
        }

        /* Tentacle 4 */
        if (tentacle4_pos > 0)
            display.drawBitmap(108, 33, tentacle4_pos1_pic, 8, 5, 1);
        if (tentacle4_pos > 1)
            display.drawBitmap(100, 36, tentacle4_pos2_pic, 12, 6, 1);
        if (tentacle4_pos > 2)
            display.drawBitmap(92, 39, tentacle4_pos3_pic, 10, 9, 1);
        if (tentacle4_pos > 3)
        {
            if (player_pos == 5)
            {
                if (death_timer % BLINK_CATCH < BLINK_CATCH / 2)
                    display.drawBitmap(80, 44, tentacle4_pos4_catch_pic, 14, 13, 1);
            }
            else
            {
                display.drawBitmap(73, 46, tentacle4_pos4_pic, 20, 10, 1);
            }
        }

        /* Score */
        display.setTextSize(1);
        display.setCursor(58, 3);
        display.println(score);
        break;
    case STATE_GAMEOVER:
        DrawGameover(score);
        break;
    }
}
void GameStep()
{
    switch (game_state)
    {
    case STATE_MENU:
        break;
    case STATE_GAMEOVER:
        if (key_code & (KEY_A | KEY_B))
            StartNewGame();
        break;
    case STATE_PLAY:
        if (death_timer > 0)
        {
            death_timer--;
            if (death_timer == 0)
            {
                player_pos = 0;
                treasure = false;
                if (--lives == 0)
                    game_state = STATE_GAMEOVER;
            }
            return;
        }

        if (key_code & KEY_A)
        {
            randomSeed(millis());
            player_pos--;
        }
        if (key_code & KEY_B)
        {
            randomSeed(millis());
            player_pos++;
        }

        if (player_pos < 0)
        {
            if (treasure)
            {
                treasure = false;
                score++;
            }
            player_pos = 0;
        }
        if (player_pos > 5)
            player_pos = 5;

        if (player_pos == 5)
            treasure = true;

        if (random(10) == 0)
            tentacle1_pos += random(-1, 2);
        if (tentacle1_pos == 1)
            tentacle1_dir = random(2);
        if (random(10) == 0)
            tentacle2_pos += random(-1, 2);
        if (random(10) == 0)
            tentacle3_pos += random(-1, 2);
        if (random(10) == 0)
            tentacle4_pos += random(-1, 2);

        if (tentacle1_pos < 0)
            tentacle1_pos = 0;
        if (tentacle1_pos > 5)
            tentacle1_pos = 5;

        if (tentacle2_pos < 0)
            tentacle2_pos = 0;
        if (tentacle2_pos > 5)
            tentacle2_pos = 5;

        if (tentacle3_pos < 0)
            tentacle3_pos = 0;
        if (tentacle3_pos > 5)
            tentacle3_pos = 5;

        if (tentacle4_pos < 0)
            tentacle4_pos = 0;
        if (tentacle4_pos > 4)
            tentacle4_pos = 4;

        if ((player_pos == 1 && tentacle1_pos == 5 && tentacle1_dir) || (player_pos == 2 && tentacle1_pos == 5 && !tentacle1_dir) || (player_pos == 3 && tentacle2_pos == 5) || (player_pos == 4 && tentacle3_pos == 5) || (player_pos == 5 && tentacle4_pos == 4))
            death_timer = 30;

        break;
    }
}
void StartNewGame()
{
    game_state = STATE_PLAY;
    score = 0;
    lives = 3;
    player_pos = 0;
    treasure = false;
    tentacle1_dir = false;
    tentacle1_pos = tentacle2_pos = tentacle3_pos = tentacle4_pos = 0;
    death_timer = 0;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("Octopus"), 2000);
    StartNewGame();
}

void loop()
{
    ControlStep(2);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}