#define BLOCK_SIZE 8

struct SnakePart
{
    uint8_t x, y;
    SnakePart(uint8_t x, uint8_t y) : x(x), y(y) {}
    SnakePart() : x(x), y(y) {}
};