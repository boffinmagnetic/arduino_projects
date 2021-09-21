#include <Adafruit_SH1106.h>
Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

#define BOARD_SIZE 6
#define BOARD_SPACE 10
Type my_board[BOARD_SIZE][BOARD_SIZE];
Type opp_board[BOARD_SIZE][BOARD_SIZE];

#define SHIP_COUNT 4
Ship ship;
Move game_move;

uint8_t wait_cnt;
bool playing;
bool menu;

void DrawBoard(Type board[BOARD_SIZE][BOARD_SIZE])
{
    PrintCenter(F("SHIPS"), 0, 2);
    display.drawLine(0, 16, display.width(), 16, 1);
    for (uint8_t i = 0; i <= BOARD_SIZE; i++)
    {
        display.drawLine(0, 64 + BOARD_SPACE * i, BOARD_SPACE * BOARD_SIZE, 64 + BOARD_SPACE * i, 1);
        display.drawLine(BOARD_SPACE * i, 64, BOARD_SPACE * i, 64 + BOARD_SPACE * BOARD_SIZE, 1);
    }

    for (uint8_t i = 0; i < BOARD_SIZE; i++)
        for (uint8_t j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j].marked)
                display.drawRoundRect(BOARD_SPACE * i + 3, 64 + BOARD_SPACE * j + 3, BOARD_SPACE - 5, BOARD_SPACE - 5, 1, 1);
            if (board[i][j].ship)
                switch (board[i][j].type)
                {
                case STR_HOR:
                    display.drawRect(BOARD_SPACE * i, 64 + BOARD_SPACE * j + 2, BOARD_SPACE + 1, BOARD_SPACE - 3, 1);
                    break;
                case STR_VER:
                    display.drawRect(BOARD_SPACE * i + 2, 64 + BOARD_SPACE * j, BOARD_SPACE - 3, BOARD_SPACE + 1, 1);
                    break;
                case SHIP_TOP:
                    display.drawRect(BOARD_SPACE * i + 2, 64 + BOARD_SPACE * j + 2, BOARD_SPACE - 3, BOARD_SPACE - 1, 1);
                    break;
                case SHIP_BOTTOM:
                    display.drawRect(BOARD_SPACE * i + 2, 64 + BOARD_SPACE * j, BOARD_SPACE - 3, BOARD_SPACE - 1, 1);
                    break;
                case SHIP_LEFT:
                    display.drawRect(BOARD_SPACE * i + 2, 64 + BOARD_SPACE * j + 2, BOARD_SPACE - 1, BOARD_SPACE - 3, 1);
                    break;
                case SHIP_RIGHT:
                    display.drawRect(BOARD_SPACE * i, 64 + BOARD_SPACE * j + 2, BOARD_SPACE - 1, BOARD_SPACE - 3, 1);
                    break;
                }
            if (board[i][j].dead)
            {
                display.drawLine(BOARD_SPACE * i + 2, 64 + BOARD_SPACE * j + 2, BOARD_SPACE * (i + 1) - 2, 64 + BOARD_SPACE * (j + 1) - 2, 1);
                display.drawLine(BOARD_SPACE * i + 2, 64 + BOARD_SPACE * (j + 1) - 2, BOARD_SPACE * (i + 1) - 2, 64 + BOARD_SPACE * j + 2, 1);
            }
        }
}

void DrawCurrsor()
{
    static uint8_t blink_timer = 0;
    if (blink_timer++ % 16 < 12)
        display.drawRect(BOARD_SPACE * game_move.x + 1, 64 + BOARD_SPACE * game_move.y + 1, BOARD_SPACE - 1, BOARD_SPACE - 1, 1);
}

void Draw()
{
    switch (game_state)
    {
    case STATE_MENU:
        DrawBoard(my_board);
        switch (menu)
        {
        case 0:
            PrintCenter(F("Player 1"), 30, 1);
            PrintCenter(F("Player 2"), 41, 1);
            if (game_move.y == 0)
                display.drawRect(0, 28, display.width(), 12, 1);
            else if (game_move.y == 1)
                display.drawRect(0, 39, display.width(), 12, 1);
            break;
        case 1:
            PrintCenter(F("Arrange"), 35, 1);
            PrintCenter(F("ships!"), 46, 1);
            display.drawRect(BOARD_SPACE * ship.start.x + 2, 64 + BOARD_SPACE * ship.start.y + 2, BOARD_SPACE * (ship.horiz ? ship.len : 1) - 3, BOARD_SPACE * (ship.horiz ? 1 : ship.len) - 3, 1);
            break;
        }
        break;
    case STATE_PLAY:
        if (wait_cnt < 50)
        {
            if (playing)
                DrawBoard(my_board);
            else
                DrawBoard(opp_board);
        }
        else if (playing)
        {
            PrintCenter(F("Your"), 35, 1);
            PrintCenter(F("Turn"), 46, 1);

            DrawBoard(opp_board);
            DrawCurrsor();
        }
        else
        {
            PrintCenter(F("Opponents"), 35, 1);
            PrintCenter(F("Turn"), 46, 1);

            DrawBoard(my_board);
        }
        break;
    case STATE_GAMEOVER:
        PrintCenter(F("You"), 20, 2);
        PrintCenter(F("Lose"), 40, 2);
        DrawBoard(my_board);
        break;
    case STATE_WIN:
        PrintCenter(F("You"), 20, 2);
        PrintCenter(F("Win"), 40, 2);
        DrawBoard(my_board);
        break;
    }
}

void GameStep()
{
    switch (game_state)
    {
    case STATE_MENU:
        switch (menu)
        {
        case 0:
            if (dir & UP)
                game_move.y--;
            else if (dir & DOWN)
                game_move.y++;

            if (game_move.y < 0)
                game_move.y = 0;
            else if (game_move.y > 1)
                game_move.y = 1;

            if (key_code & KEY_J)
            {
                playing = (game_move.y == 1 ? false : true);
                menu = 1;
            }
            break;
        case 1:
            static uint8_t ship_num = 0;
            if (dir & LEFT)
                game_move.x--;
            else if (dir & RIGHT)
                game_move.x++;
            if (dir & UP)
                game_move.y--;
            else if (dir & DOWN)
                game_move.y++;

            bool turned = false;
            if (game_move.x < 0)
            {
                game_move.x = 0;
                ship.horiz = false;
                turned = true;
            }
            if (game_move.y < 0)
            {
                game_move.y = 0;
                ship.horiz = true;
                turned = true;
            }

            if (game_move.x > BOARD_SIZE - 1)
                game_move.x = BOARD_SIZE - 1;
            if (game_move.y > BOARD_SIZE - 1)
                game_move.y = BOARD_SIZE - 1;

            if ((ship.horiz ? game_move.x : game_move.y) + ship.len > BOARD_SIZE)
            {
                (ship.horiz ? game_move.x : game_move.y) = BOARD_SIZE - ship.len;
                if (!turned)
                    ship.horiz ? ship.horiz = false : ship.horiz = true;
                if ((ship.horiz ? game_move.x : game_move.y) + ship.len > BOARD_SIZE)
                    (ship.horiz ? game_move.x : game_move.y) = BOARD_SIZE - ship.len;
            }

            ship.start = game_move;

            if (key_code & KEY_J)
            {
                bool flag = true;
                for (uint8_t i = 0; i < ship.len; i++)
                    if (my_board[ship.start.x + (ship.horiz ? i : 0)][ship.start.y + (ship.horiz ? 0 : i)].ship == true)
                        flag = false;
                if (flag)
                {
                    for (uint8_t i = 0; i < ship.len; i++)
                    {
                        my_board[ship.start.x + (ship.horiz ? i : 0)][ship.start.y + (ship.horiz ? 0 : i)].ship = true;
                        if (i == 0)
                            my_board[ship.start.x + (ship.horiz ? i : 0)][ship.start.y + (ship.horiz ? 0 : i)].type = ship.horiz ? SHIP_LEFT : SHIP_TOP;
                        else if (i == ship.len - 1)
                            my_board[ship.start.x + (ship.horiz ? i : 0)][ship.start.y + (ship.horiz ? 0 : i)].type = ship.horiz ? SHIP_RIGHT : SHIP_BOTTOM;
                        else
                            my_board[ship.start.x + (ship.horiz ? i : 0)][ship.start.y + (ship.horiz ? 0 : i)].type = ship.horiz ? STR_HOR : STR_VER;
                    }
                    ship.start = game_move = {0, 0};
                    ship_num++;
                    ship.len++;
                    ship.horiz = false;
                    if (ship_num >= SHIP_COUNT)
                    {
                        game_state = STATE_PLAY;
                        ship_num = 0;
                    }
                }
            }
            break;
        }
        break;
    case STATE_GAMEOVER:
        if (key_code & KEY_J)
            StartNewGame();
        break;
    case STATE_WIN:
        if (key_code & KEY_J)
            StartNewGame();
        break;
    case STATE_PLAY:
        if (dir & LEFT)
            game_move.x--;
        else if (dir & RIGHT)
            game_move.x++;
        if (dir & UP)
            game_move.y--;
        else if (dir & DOWN)
            game_move.y++;

        if (game_move.x < 0)
            game_move.x = 0;
        else if (game_move.x > BOARD_SIZE - 1)
            game_move.x = BOARD_SIZE - 1;
        if (game_move.y < 0)
            game_move.y = 0;
        else if (game_move.y > BOARD_SIZE - 1)
            game_move.y = BOARD_SIZE - 1;

        if (wait_cnt < 50)
        {
            wait_cnt++;
        }
        else if (playing)
        {
            bool flag = false;
            if (key_code & KEY_J)
            {
                if (opp_board[game_move.x][game_move.y].ship == false && opp_board[game_move.x][game_move.y].marked == false)
                {
                    uint8_t tmp[2] = {(uint8_t)game_move.x, (uint8_t)game_move.y};
                    Serial.write(tmp, 2);
                    flag = true;
                }
            }
            while (flag)
                if (Serial.available() > 0)
                {
                    if (Serial.read())
                    {
                        opp_board[game_move.x][game_move.y].ship = true;
                        opp_board[game_move.x][game_move.y].dead = true;
                        while (flag)
                            if (Serial.available() > 0)
                            {
                                opp_board[game_move.x][game_move.y].type = Serial.read();
                                flag = false;
                            }
                        if (CheckWin())
                            game_state = STATE_WIN;
                    }
                    else
                    {
                        opp_board[game_move.x][game_move.y].marked = true;
                        playing = !playing;
                        wait_cnt = 0;
                    }
                    flag = false;
                }
        }
        else
        {
            if (Serial.available() >= 2)
            {
                uint8_t tmp[2];
                Serial.readBytes(tmp, 2);
                game_move.x = tmp[0];
                game_move.y = tmp[1];

                if (my_board[game_move.x][game_move.y].ship == true)
                {
                    my_board[game_move.x][game_move.y].dead = true;
                    Serial.write(true);
                    Serial.write(my_board[game_move.x][game_move.y].type);
                    if (CheckWin())
                        game_state = STATE_GAMEOVER;
                }
                else
                {
                    my_board[game_move.x][game_move.y].marked = true;
                    Serial.write(false);
                    playing = !playing;
                    wait_cnt = 0;
                }
            }
        }
        break;
    }
}

bool CheckWin()
{
    uint8_t my = 0, opp = 0;
    for (uint8_t i = 0; i < BOARD_SIZE; i++)
        for (uint8_t j = 0; j < BOARD_SIZE; j++)
        {
            if (my_board[i][j].dead)
                my++;
            if (opp_board[i][j].dead)
                opp++;
        }
    if (my >= 14 || opp >= 14)
        return true;
    return false;
}

void StartNewGame()
{
    game_state = STATE_MENU;
    ship.len = 2;
    menu = 0;
    wait_cnt = 100;
    ClearBoards();
}

void ClearBoards()
{
    for (uint8_t i = 0; i < BOARD_SIZE; i++)
        for (uint8_t j = 0; j < BOARD_SIZE; j++)
            my_board[i][j] = opp_board[i][j] = Type();
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("SHIPS"), 2000);
    Serial.begin(38400);
    StartNewGame();
    display.setRotation(3);
}

void loop()
{
    ControlStep();
    GameStep();
    Draw();
    display.display();
    display.clearDisplay();
}
