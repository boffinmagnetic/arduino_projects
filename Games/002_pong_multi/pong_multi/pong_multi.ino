#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

Object paddle_1(5, (display.height() - 24) / 2, 6, 24),
    paddle_2(display.width() - 5 - 6, (display.height() - 24) / 2, 6, 24),
    ball(0, 0, 5, 5);

#define PADDLE_SPEED 2

uint16_t score1;
uint16_t score2;

float ball_dir_x;
float ball_dir_y;

int8_t ball_speed;

bool single;

void Draw()
{
    switch (game_state)
    {
    case STATE_PLAY:
        display.setTextSize(1);
        display.setCursor(display.width() / 2 - 2 - DigitCount(score1) * 6, 2);
        display.println(score1);
        display.setCursor(display.width() / 2 + 4, 2);
        display.println(score2);

        display.drawLine(0, 0, display.width(), 0, 1);
        display.drawLine(0, display.height() - 1, display.width(), display.height() - 1, 1);

        for (uint8_t i = 2; i < display.height() - 4; i += 8)
            display.drawLine(display.width() / 2, i, display.width() / 2, i + 3, 1);

        display.fillCircle(ball.x + 2, ball.y + 2, 2, 1);
        display.drawBitmap(paddle_1.x, paddle_1.y, paddle_pic, paddle_1.w, paddle_1.h, 1);
        display.drawBitmap(paddle_2.x, paddle_2.y, paddle_pic, paddle_2.w, paddle_2.h, 1);
        break;

    case STATE_GAMEOVER:
        if (score1 > score2)
            PrintCenter(F("Left Wins"), 10, 2);
        else if (score1 < score2)
            PrintCenter(F("Right Wins"), 10, 2);
        else
            PrintCenter(F("Tie"), 10, 2);
        PrintCenter(String(score1) + String(':') + String(score2), 40, 1);
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
        if (key_code & KEY_J)
            game_state = STATE_GAMEOVER;

        if (random(100) == 0)
            ball_speed++;

        if (dir & UP)
            paddle_1.y -= PADDLE_SPEED;
        else if (dir & DOWN)
            paddle_1.y += PADDLE_SPEED;

        if (paddle_1.y < 1)
            paddle_1.y = 1;
        else if (paddle_1.y > display.height() - paddle_1.h - 1)
            paddle_1.y = display.height() - paddle_1.h - 1;

        if (key_code & KEY_A)
            paddle_2.y += PADDLE_SPEED;
        else if (key_code & KEY_B)
            paddle_2.y -= PADDLE_SPEED;

        if (paddle_2.y < 1)
            paddle_2.y = 1;
        if (paddle_2.y > display.height() - paddle_2.h - 1)
            paddle_2.y = display.height() - paddle_2.h - 1;

        ball.x += ball_dir_x * ball_speed;
        ball.y += ball_dir_y * ball_speed;

        if (ball.x < paddle_1.x + paddle_1.w && ball.x + ball.w > paddle_1.x &&
            ball.y < paddle_1.y + paddle_1.h && ball.y + ball.h > paddle_1.y)
        {
            ball.x = paddle_1.x + paddle_1.w;
            float t = ((ball.y + ball.h / 2 - paddle_1.y) / paddle_1.h) - 0.5f;
            ball_dir_x = abs(ball_dir_x);
            ball_dir_y = t;
        }

        if (ball.x + ball.w > paddle_2.x && ball.x < paddle_2.x + paddle_2.w &&
            ball.y + ball.h > paddle_2.y && ball.y < paddle_2.y + paddle_2.h)
        {
            ball.x = paddle_2.x - ball.w;
            float t = ((ball.y + ball.h / 2 - paddle_2.y) / paddle_2.h) - 0.5f;
            ball_dir_x = -abs(ball_dir_x);
            ball_dir_y = t;
        }

        if (ball.x < -ball.w)
        {
            score2 += 1;
            ResetBall();
        }
        else if (ball.x > display.width())
        {
            score1 += 1;
            ResetBall();
        }

        if (ball.y < 1)
        {
            ball.y = 1;
            ball_dir_y = abs(ball_dir_y);
        }
        else if (ball.y > display.height() - ball.h - 1)
        {
            ball.y = display.height() - ball.h - 1;
            ball_dir_y = -abs(ball_dir_y);
        }
        break;
    }
}

void StartNewGame()
{
    game_state = STATE_PLAY;
    score1 = score2 = 0;
    paddle_1.y = (display.height() - paddle_1.h) / 2;
    paddle_2.y = (display.height() - paddle_2.h) / 2;
    ResetBall();
}

uint16_t DigitCount(uint16_t n)
{
    uint8_t i = 0;
    do
    {
        i++;
        n /= 10;
    } while (n > 0);
    return i;
}
void ResetBall()
{
    ball.x = (display.width() - ball.w) / 2;
    ball.y = (display.height() - ball.h) / 2;
    ball_dir_x = -1.0f;
    ball_dir_y = 0;
    ball_speed = 2;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("PongMulti"), 2000);
    StartNewGame();
}

void loop()
{
    ControlStep(0, 0, 5, 0);
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}