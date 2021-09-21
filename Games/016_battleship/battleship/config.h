#define STR_HOR 0
#define STR_VER 1
#define SHIP_TOP 2
#define SHIP_BOTTOM 3
#define SHIP_LEFT 4
#define SHIP_RIGHT 5

struct Type
{
    bool marked;
    bool ship;
    bool dead;
    uint8_t type;
    Type() : marked(0), ship(0), dead(0) {}
};

struct Move
{
    int8_t x, y;
    Move(int8_t x, int8_t y) : x(x), y(y) {}
    Move() : x(0), y(0) {}
};

struct Ship
{
    Move start;
    uint8_t len;
    bool horiz;
    Ship(uint8_t len) : start(0, 0), len(len), horiz(false) {}
    Ship() : start(0, 0), len(0), horiz(false) {}
};
