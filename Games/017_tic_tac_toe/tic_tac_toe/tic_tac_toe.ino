#include <Adafruit_SH1106.h>

Adafruit_SH1106 display(4);

#include "config.h"
#include "display.h"
#include "buttons.h"

#define BOARD_MAX_SIZE 10
uint8_t board_space;
uint8_t board_size;
Player board[BOARD_MAX_SIZE][BOARD_MAX_SIZE];

Move game_move;

Player player;
Player winner;

bool computer;
uint8_t menu;

void DrawBoard()
{
    uint8_t board_offset = (display.width() - 1 - board_size * board_space) / 2;
    PrintCenter(F("TicTacToe"), 0, 1);
    display.drawLine(0, 11, display.width(), 11, 1);
    for (uint8_t i = 0; i <= board_size; i++)
    {
        display.drawLine(board_offset, 64 + board_space * i, board_offset + board_space * board_size, 64 + board_space * i, 1);
        display.drawLine(board_offset + board_space * i, 64, board_offset + board_space * i, 64 + board_space * board_size, 1);
    }
    for (uint8_t i = 0; i < board_size; i++)
        for (uint8_t j = 0; j < board_size; j++)
            if (board[i][j] == PLAYER1)
            {
                display.drawLine(board_offset + board_space * i, 64 + board_space * j, board_offset + board_space * (i + 1), 64 + board_space * (j + 1), 1);
                display.drawLine(board_offset + board_space * i, 64 + board_space * (j + 1), board_offset + board_space * (i + 1), 64 + board_space * j, 1);
            }
            else if (board[i][j] == PLAYER2)
            {
                display.drawRoundRect(board_offset + board_space * i + 2, 64 + board_space * j + 2, board_space - 3, board_space - 3, 90, 1);
            }
}

void DrawCurrsor()
{
    static uint8_t blink_timer = 0;
    uint8_t board_offset = (display.width() - 1 - board_size * board_space) / 2;
    if (blink_timer++ % 16 < 12)
        display.drawRect(board_offset + board_space * game_move.x + 1, 64 + board_space * game_move.y + 1, board_space - 1, board_space - 1, 1);
}

void Draw()
{
    switch (game_state)
    {
    case STATE_MENU:
        DrawBoard();
        switch (menu)
        {
        case 0:
            PrintCenter(F("1 Player"), 20, 1);
            PrintCenter(F("2 Players"), 30, 1);
            if (game_move.y == 0)
                display.drawRect(0, 18, display.width(), 11, 1);
            else if (game_move.y == 1)
                display.drawRect(0, 28, display.width(), 11, 1);
            break;
        case 1:
            PrintCenter(F("First X"), 20, 1);
            PrintCenter(F("First O"), 30, 1);
            if (game_move.y == 0)
                display.drawRect(0, 18, display.width(), 11, 1);
            else if (game_move.y == 1)
                display.drawRect(0, 28, display.width(), 11, 1);
            break;
        case 2:
            PrintCenter(F("Board Size"), 28, 1);
            PrintCenter(board_size, 38, 1);
            break;
        }
        break;
    case STATE_PLAY:
        DrawBoard();
        if (!computer || player == PLAYER2)
            DrawCurrsor();
        if (player == PLAYER1)
        {
            PrintCenter(F("X"), 20, 1);
            PrintCenter(F("Plays"), 30, 1);
        }
        else if (player == PLAYER2)
        {
            PrintCenter(F("O"), 20, 1);
            PrintCenter(F("Plays"), 30, 1);
        }

        break;
    case STATE_GAMEOVER:
        DrawBoard();
        switch (winner)
        {
        case PLAYER_NONE:
            PrintCenter(F("Tie"), 25, 2);
            break;
        case PLAYER1:
            PrintCenter(F("X"), 15, 2);
            PrintCenter(F("Wins"), 35, 2);
            break;
        case PLAYER2:
            PrintCenter(F("O"), 15, 2);
            PrintCenter(F("Wins"), 35, 2);
            break;
        }
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

            if (key_code & (KEY_J | KEY_A))
                menu = (game_move.y == 0 ? 1 : 2);
            break;
        case 1:
            if (dir & UP)
                game_move.y--;
            else if (dir & DOWN)
                game_move.y++;

            if (game_move.y < 0)
                game_move.y = 0;
            else if (game_move.y > 1)
                game_move.y = 1;

            if (key_code & (KEY_J | KEY_A))
            {
                player = (game_move.y == 0 ? PLAYER1 : PLAYER2);
                computer = true;
                game_move = {0, 0};
                game_state = STATE_PLAY;
            }
            break;
        case 2:
            if (dir & LEFT)
                board_size--;
            else if (dir & RIGHT)
                board_size++;

            if (board_size < 1)
                board_size = 1;
            else if (board_size > BOARD_MAX_SIZE)
                board_size = BOARD_MAX_SIZE;

            board_space = (display.width() - 1) / board_size;

            if (key_code & (KEY_J | KEY_A))
            {
                player = PLAYER1;
                computer = false;
                game_move = {0, 0};
                game_state = STATE_PLAY;
            }
            break;
        }
        break;
    case STATE_GAMEOVER:
        if (key_code & (KEY_J | KEY_A))
            StartNewGame();
        break;
    case STATE_PLAY:
        if (key_code & KEY_B)
            StartNewGame();

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
        else if (game_move.x > board_size - 1)
            game_move.x = board_size - 1;
        if (game_move.y < 0)
            game_move.y = 0;
        else if (game_move.y > board_size - 1)
            game_move.y = board_size - 1;

        if (computer && player == PLAYER1)
            PlayMove(FindBestMove());
        else if (key_code & (KEY_J | KEY_A) && board[game_move.x][game_move.y] == PLAYER_NONE)
            PlayMove(game_move);

        break;
    }
}

void StartNewGame()
{
    game_state = STATE_MENU;
    display.setRotation(3);
    winner = PLAYER_NONE;
    game_move = {0, 0};
    menu = 0;
    board_size = 3;
    board_space = (display.width() - 1) / board_size;
    for (uint8_t i = 0; i < BOARD_MAX_SIZE; i++)
        for (uint8_t j = 0; j < BOARD_MAX_SIZE; j++)
            board[i][j] = PLAYER_NONE;

    randomSeed(millis());
}

void PlayMove(Move move)
{
    board[move.x][move.y] = player;

    if (Wined(player, move))
    {
        winner = player;
        game_state = STATE_GAMEOVER;
    }
    else if (!MovesLeft())
        game_state = STATE_GAMEOVER;

    player = (player == PLAYER2 ? PLAYER1 : PLAYER2);
}

bool Wined(Player cur_player, Move move)
{
    uint8_t sign_length = 0;

    //Horizontal
    for (uint8_t i = move.x; i < board_size; i++)
        if (board[i][move.y] == cur_player)
            sign_length++;
        else
            break;
    for (int8_t i = move.x - 1; i >= 0; i--)
        if (board[i][move.y] == cur_player)
            sign_length++;
        else
            break;

    if (sign_length >= (board_size < 4 ? board_size : 4))
        return true;
    else
        sign_length = 0;

    //Vertical
    for (uint8_t j = move.y; j < board_size; j++)
        if (board[move.x][j] == cur_player)
            sign_length++;
        else
            break;
    for (int8_t j = move.y - 1; j >= 0; j--)
        if (board[move.x][j] == cur_player)
            sign_length++;
        else
            break;
    if (sign_length >= (board_size < 4 ? board_size : 4))
        return true;
    else
        sign_length = 0;

    //Diagonal 1
    for (uint8_t i = 0; move.x + i < board_size && move.y + i < board_size; i++)
        if (board[move.x + i][move.y + i] == cur_player)
            sign_length++;
        else
            break;
    for (uint8_t i = 1; move.x - i >= 0 && move.y - i >= 0; i++)
        if (board[move.x - i][move.y - i] == cur_player)
            sign_length++;
        else
            break;
    if (sign_length >= (board_size < 4 ? board_size : 4))
        return true;
    else
        sign_length = 0;

    //Diagonal 2
    for (uint8_t i = 0; move.x - i >= 0 && move.y + i < board_size; i++)
        if (board[move.x - i][move.y + i] == cur_player)
            sign_length++;
        else
            break;
    for (uint8_t i = 1; move.x + i < board_size && move.y - i >= 0; i++)
        if (board[move.x + i][move.y - i] == cur_player)
            sign_length++;
        else
            break;
    if (sign_length >= (board_size < 4 ? board_size : 4))
        return true;
    else
        sign_length = 0;

    return false;
}

Move CheckBreakPoint(Player cur_player)
{
    for (int i = 0; i < 3; i++)
    {
        if (board[0][i] == cur_player && board[1][i] == cur_player && board[2][i] == PLAYER_NONE)
            return Move(2, i);
        else if (board[0][i] == cur_player && board[1][i] == PLAYER_NONE && board[2][i] == cur_player)
            return Move(1, i);
        else if (board[0][i] == PLAYER_NONE && board[1][i] == cur_player && board[2][i] == cur_player)
            return Move(0, i);

        if (board[i][0] == cur_player && board[i][1] == cur_player && board[i][2] == PLAYER_NONE)
            return Move(i, 2);
        else if (board[i][0] == cur_player && board[i][1] == PLAYER_NONE && board[i][2] == cur_player)
            return Move(i, 1);
        else if (board[i][0] == PLAYER_NONE && board[i][1] == cur_player && board[i][2] == cur_player)
            return Move(i, 0);
    }
    if (board[0][0] == cur_player && board[1][1] == cur_player && board[2][2] == PLAYER_NONE)
        return Move(2, 2);
    else if (board[0][0] == cur_player && board[1][1] == PLAYER_NONE && board[2][2] == cur_player)
        return Move(1, 1);
    else if (board[0][0] == PLAYER_NONE && board[1][1] == cur_player && board[2][2] == cur_player)
        return Move(0, 0);

    if (board[0][2] == cur_player && board[1][1] == cur_player && board[2][0] == PLAYER_NONE)
        return Move(2, 0);
    else if (board[0][2] == cur_player && board[1][1] == PLAYER_NONE && board[2][0] == cur_player)
        return Move(1, 1);
    else if (board[0][2] == PLAYER_NONE && board[1][1] == cur_player && board[2][0] == cur_player)
        return Move(0, 2);

    return Move(10, 10);
}

Move FindBestMove()
{
    //== commented out for hard bot ==

    // uint8_t movesPlayed = 0;

    // Move firstMoves[] = {{0, 0}, {2, 0}, {2, 0}, {2, 2}};

    // for (uint8_t i = 0; i < 4; i++)
    //     if (board[firstMoves[i].x][firstMoves[i].y] != PLAYER_NONE)
    //         movesPlayed++;
    delay(random(1500) + 500);
    Move nextMove = CheckBreakPoint(PLAYER1);
    if (nextMove.x != 10)
        return nextMove;

    nextMove = CheckBreakPoint(PLAYER2);

    if (nextMove.x == 10)
        while (true)
        {
            // if (movesPlayed == 4)
            // {
            Move randomMove = {(int8_t)random(3), (int8_t)random(3)};
            if (board[randomMove.x][randomMove.y] == PLAYER_NONE)
                return randomMove;
            // }
            // else
            // {
            //     Move move = firstMoves[random(4)];

            //     if (board[move.x][move.y] == PLAYER_NONE)
            //         return move;
            // }
        }
    else
        return nextMove;
}

bool MovesLeft()
{
    for (uint8_t i = 0; i < board_size; i++)
        for (uint8_t j = 0; j < board_size; j++)
            if (board[i][j] == PLAYER_NONE)
                return true;
    return false;
}

void setup()
{
    DisplaySetup();
    ButtonsSetup();
    DrawLogo(F("TicTacToe"), 2000);
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
