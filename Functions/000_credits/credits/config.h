struct Object
{
    int8_t x, y;
    uint8_t w, h;
    Object(int8_t x, int8_t y, uint8_t w, uint8_t h) : x(x), y(y), w(w), h(h) {}
};

#define SPEED 2

#define NAMES_SIZE 18
const char *const names[NAMES_SIZE] = {"Owned by",
                                       "Boffin",
                                       "Created by",
                                       "David Paskevic",
                                       "HW & FW by",
                                       "Ondrej Nentvich",
                                       "Michal Valkoun",
                                       "Design by",
                                       "Petr Duba",
                                       "Mira Vesela",
                                       "Quang Do Xuan",
                                       "Aneta Kalouskova",
                                       "Support",
                                       "Jan Tacik",
                                       "Libor Kolacny",
                                       "Mascot",
                                       "Roxy"};