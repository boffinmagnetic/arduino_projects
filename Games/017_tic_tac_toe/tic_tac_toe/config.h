enum Player
{
    PLAYER_NONE,
    PLAYER1,
    PLAYER2
};

struct Move
{
    int8_t x, y;
    Move(int8_t x, int8_t y) : x(x), y(y) {}
    Move() : x(0), y(0) {}
};
