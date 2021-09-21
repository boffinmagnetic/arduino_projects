struct Object
{
    int16_t x, y;
    int16_t w, h;
    Object(int16_t x, int16_t y, int16_t w, int16_t h) : x(x), y(y), w(w), h(h) {}
    Object() : x(0), y(0), w(0), h(0) {}
};