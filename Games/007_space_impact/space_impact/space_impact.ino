#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

#define SHIP_SPEED 3
#define SHIP_BULLET_SPEED 7
#define STAR_BULLET_SPEED 5
#define STAR_SPEED 2
#define MAX_LIVES 5

Object ship(0, 0, 16, 16),
    star(0, 0, 20, 20),
    ship_bullet(0, 0, 5, 2),
    star_bullet(0, 0, 5, 5);

int8_t lives;
uint8_t score;
bool ship_shot;
bool star_shot;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        display.drawPixel(97, 49, 1);
        display.drawPixel(110, 57, 1);
        display.drawPixel(33, 55, 1);
        display.drawPixel(46, 15, 1);
        display.drawPixel(91, 11, 1);
        display.drawPixel(27, 35, 1);
        display.drawPixel(71, 21, 1);
        display.drawPixel(113, 7, 1);
        display.drawPixel(94, 28, 1);
        display.drawPixel(13, 15, 1);
        display.drawPixel(126, 11, 1);
        display.drawPixel(92, 51, 1);
        display.drawPixel(32, 37, 1);
        display.drawPixel(88, 0, 1);
        display.drawPixel(2, 40, 1);
        display.drawPixel(102, 2, 1);
        display.drawPixel(108, 6, 1);
        display.drawPixel(52, 3, 1);
        display.drawPixel(113, 7, 1);
        display.drawPixel(45, 26, 1);

        if (ship_shot)
            display.fillRect(ship_bullet.x, ship_bullet.y, ship_bullet.w, ship_bullet.h, 1);
        if (star_shot)
            display.drawRoundRect(star_bullet.x, star_bullet.y, star_bullet.w, star_bullet.h, 90, 1);

        display.drawBitmap(ship.x, ship.y, ship_pic, ship.w, ship.h, 1);
        display.fillRoundRect(star.x, star.y, star.w, star.h, 90, 1);
        display.fillRoundRect(star.x + star.w / 4, star.y + star.h / 4, star.w / 3, star.h / 3, 90, 0);
        PrintCenter(String("lives ") + String(lives), 0, 1);
        PrintCenter(String("score ") + String(score), 56, 1);
        break;
    case STATE_GAMEOVER:
        PrintCenter(F("Game Over!"), 10, 2);
        PrintCenter(F("Score"), 30, 1);
        PrintCenter(score, 40, 1);
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
    case STATE_PLAY:
        static bool star_dir = false;

        ship.y = map(analogRead(Y_AXIS), 0, 1023, display.height() - ship.h, 0);
        star_dir ? star.y += STAR_SPEED : star.y -= STAR_SPEED;

        if (star.y <= 0 || star.y >= display.height() - star.h)
            star_dir = !star_dir;

        if (key_code & (KEY_A | KEY_B) && !ship_shot)
        {
            ship_bullet.x = ship.x + ship.w - ship_bullet.w;
            ship_bullet.y = ship.y + ship.h / 2 - 1;
            ship_shot = true;
        }
        if (ship_shot)
        {
            ship_bullet.x += SHIP_BULLET_SPEED;
            if (ship_bullet.x < star.x + star.w && ship_bullet.x + ship_bullet.w > star.x &&
                ship_bullet.y < star.y + star.h && ship_bullet.y + ship_bullet.h > star.y)
            {
                ship_shot = false;
                score++;
            }
            if (ship_bullet.x > display.width())
                ship_shot = false;
        }
        if (!star_shot)
        {
            star_bullet.x = star.x + star.w / 2;
            star_bullet.y = star.y + random(star.h);
            star_bullet.w = star_bullet.h = random(4) + 2;
            star_shot = true;
        }
        if (star_shot)
        {
            star_bullet.x -= STAR_BULLET_SPEED;
            if (star_bullet.x < ship.x + ship.w && star_bullet.x + star_bullet.w > ship.x &&
                star_bullet.y < ship.y + ship.h && star_bullet.y + star_bullet.h > ship.y)
            {
                star_shot = false;
                lives--;
            }
            if (star_bullet.x < -star_bullet.w)
                star_shot = false;
        }
        if (ship_shot && star_shot)
            if (star_bullet.x < ship_bullet.x + ship_bullet.w && star_bullet.x + star_bullet.w > ship_bullet.x &&
                star_bullet.y < ship_bullet.y + ship_bullet.h && star_bullet.y + star_bullet.h > ship_bullet.y)
                star_shot = ship_shot = false;

        if (lives <= 0)
            game_state = STATE_GAMEOVER;
        break;
    }
}
void StartNewGame()
{
    game_state = STATE_PLAY;
    lives = MAX_LIVES;
    score = 0;

    ship.x = 0;
    ship.y = display.height() / 2;

    star.x = display.width() - star.w;
    star.y = display.height() / 2;
    randomSeed(millis());
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo("Space Impact", 2000);
    StartNewGame();
}

void loop()
{
    ControlStep(0, 0);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}