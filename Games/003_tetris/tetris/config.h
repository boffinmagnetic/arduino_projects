#define SHAPE_SIZE 4

struct Tetromino
{
    int8_t x, y;
    uint8_t type;
    bool shape[SHAPE_SIZE][SHAPE_SIZE];
    void copy(const bool src[SHAPE_SIZE][SHAPE_SIZE])
    {
        memcpy(shape, src, 16);
    }
};

const bool shapeT[SHAPE_SIZE][SHAPE_SIZE] = {{0, 1, 0, 0},
                                             {1, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {0, 0, 0, 0}};

const bool shapeL[SHAPE_SIZE][SHAPE_SIZE] = {{1, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {0, 0, 0, 0}};

const bool shapeJ[SHAPE_SIZE][SHAPE_SIZE] = {{0, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {1, 1, 0, 0},
                                             {0, 0, 0, 0}};

const bool shapeS[SHAPE_SIZE][SHAPE_SIZE] = {{1, 0, 0, 0},
                                             {1, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {0, 0, 0, 0}};

const bool shapeZ[SHAPE_SIZE][SHAPE_SIZE] = {{0, 1, 0, 0},
                                             {1, 1, 0, 0},
                                             {1, 0, 0, 0},
                                             {0, 0, 0, 0}};

const bool shapeI[SHAPE_SIZE][SHAPE_SIZE] = {{0, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {0, 1, 0, 0},
                                             {0, 1, 0, 0}};

const bool shapeO[SHAPE_SIZE][SHAPE_SIZE] = {{0, 0, 0, 0},
                                             {0, 1, 1, 0},
                                             {0, 1, 1, 0},
                                             {0, 0, 0, 0}};
