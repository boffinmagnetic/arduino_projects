#define KEY_0 3
#define KEY_1 5
#define KEY_2 6
#define KEY_3 7
#define KEY_4 8

#define X_AXIS A3
#define Y_AXIS A2

enum GameState
{
    STATE_MENU,
    STATE_PLAY,
    STATE_GAMEOVER,
    STATE_WIN
};

enum KeyCode
{
    KEY_J = 0x0001,
    KEY_A = 0x0002,
    KEY_B = 0x0004,
    KEY_X = 0x0008,
    KEY_Y = 0x0010
};

enum Dir
{
    LEFT = 0x0001,
    RIGHT = 0x002,
    UP = 0x004,
    DOWN = 0x008
};

uint8_t game_state;
uint8_t key_code;
uint8_t dir;

void ButtonsSetup()
{
    pinMode(KEY_0, INPUT_PULLUP);
    pinMode(KEY_1, INPUT_PULLUP);
    pinMode(KEY_2, INPUT_PULLUP);
    pinMode(KEY_3, INPUT_PULLUP);
    pinMode(KEY_4, INPUT_PULLUP);

    pinMode(X_AXIS, INPUT);
    pinMode(Y_AXIS, INPUT);
}

void ControlStep(uint8_t max_slow_all = 5, uint8_t max_slow_joy = 5, uint8_t max_slow_key_j = 0, uint8_t max_slow_key_a = 0, uint8_t max_slow_key_b = 0, uint8_t max_slow_key_x = 0, uint8_t max_slow_key_y = 0)
{
    static uint8_t slow_all = 0, slow_joy = 0, slow_key_j = 0, slow_key_a = 0, slow_key_b = 0, slow_key_x = 0, slow_key_y = 0;
    uint8_t rotation = display.getRotation();
    key_code = 0;
    dir = 0;

    if (slow_joy < max_slow_joy)
        slow_joy++;
    else
    {
        uint16_t value;

        if (rotation == 2)
        {
            value = analogRead(X_AXIS);
            if (value < 300)
                dir |= RIGHT;
            else if (value > 730)
                dir |= LEFT;

            value = analogRead(Y_AXIS);
            if (value < 300)
                dir |= DOWN;
            else if (value > 730)
                dir |= UP;
        }
        else if (rotation == 3)
        {
            value = analogRead(Y_AXIS);
            if (value < 300)
                dir |= RIGHT;
            else if (value > 730)
                dir |= LEFT;

            value = analogRead(X_AXIS);
            if (value < 300)
                dir |= UP;
            else if (value > 730)
                dir |= DOWN;
        }
        if (dir)
            slow_joy = 0;
    }

    if (slow_all < max_slow_all)
        slow_all++;
    else
    {
        if (slow_key_j < max_slow_key_j)
            slow_key_j++;
        else if (!digitalRead(KEY_0))
        {
            key_code |= KEY_J;
            slow_key_j = 0;
        }

        if (slow_key_a < max_slow_key_a)
            slow_key_a++;
        else if (!digitalRead(KEY_1))
        {
            key_code |= KEY_A;
            slow_key_a = 0;
        }

        if (slow_key_b < max_slow_key_b)
            slow_key_b++;
        else if (!digitalRead(KEY_4))
        {
            key_code |= KEY_B;
            slow_key_b = 0;
        }

        if (slow_key_x < max_slow_key_x)
            slow_key_x++;
        else if (!digitalRead(KEY_2))
        {
            key_code |= KEY_X;
            slow_key_x = 0;
        }

        if (slow_key_y < max_slow_key_y)
            slow_key_y++;
        else if (!digitalRead(KEY_3))
        {
            key_code |= KEY_Y;
            slow_key_y = 0;
        }

        if (key_code)
            slow_all = 0;
    }
}