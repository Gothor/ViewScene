#ifndef TEXT_TO_ANIM_H
#define TEXT_TO_ANIM_H

typedef enum {
    CLOSE_TO_A = 0,
    CLOSE_TO_O = 1,
    O_TO_A = 2,
    CLOSE_TO_I = 3,
    A_TO_I = 4,
    O_TO_I = 5,
    ANIM_KINDS,

    NONE = 999,
    END = 1000,
} animation_kind_t;

typedef struct {
    animation_kind_t type;
    bool reversed;
    double duration;
} animation_bit_t;

animation_bit_t* getAnimationBits(char const* text, int length, double duration);

#endif // TEXT_TO_ANIM_H
