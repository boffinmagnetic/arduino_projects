struct Object
{
    float x, y;
    uint8_t w, h;
    Object(float x, float y, uint8_t w, uint8_t h) : x(x), y(y), w(w), h(h) {}
};

struct Block
{
    const uint8_t x, y;
    bool visible;
    Block(const uint8_t x, const uint8_t y) : x(x), y(y), visible(true) {}
};

const uint8_t PROGMEM paddle_pic[] = {
    0x3f, 0xff, 0xfc,
    0x44, 0x00, 0x22,
    0xb4, 0x00, 0x2d,
    0xb4, 0x00, 0x2d,
    0x44, 0x00, 0x22,
    0x3f, 0xff, 0xfc};