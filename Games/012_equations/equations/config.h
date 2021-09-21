enum Wrong
{
    WRONG_NONE,
    WRONG_TIME,
    WRONG_QUESTION
};

struct Object
{
    uint8_t x, y, w, h;
    Object(uint8_t x, uint8_t y, uint8_t w, uint8_t h) : x(x), y(y), w(w), h(h) {}
};

struct Question
{
    uint8_t first;
    uint8_t second;
    int8_t result;
    char operation;
    bool answer;
};