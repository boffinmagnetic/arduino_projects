struct Object
{
    float x, y;
    uint8_t w, h;
    Object(float x, float y, uint8_t w, uint8_t h) : x(x), y(y), w(w), h(h) {}
};

const uint8_t PROGMEM car_pic[] = {
    0x03, 0xc0,
    0x3c, 0x3c,
    0x7f, 0xfe,
    0x50, 0x0a,
    0x50, 0x0a,
    0xdf, 0xfb,
    0xc0, 0x03,
    0xb8, 0x1d,
    0xa8, 0x15,
    0xbf, 0xfd,
    0xe4, 0x27,
    0x9f, 0xf9,
    0xf0, 0x0f};

const uint8_t PROGMEM tree_pic[] = {
    0x02, 0x00,
    0x07, 0x00,
    0x0f, 0x80,
    0x1f, 0xc0,
    0x07, 0x00,
    0x0f, 0x80,
    0x1f, 0xc0,
    0x3f, 0xe0,
    0x7f, 0xf0,
    0x0f, 0x80,
    0x1f, 0xc0,
    0x3f, 0xe0,
    0x7f, 0xf0,
    0xff, 0xf8,
    0x07, 0x00,
    0x07, 0x00};