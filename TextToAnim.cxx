#include <stdlib.h>
#include <iostream>
#include "TextToAnim.h"

char toKnownCharacter(char c) {
    switch (c) {
        case 'a':
        case 'A':
            return 'a';
        case 'o':
        case 'O':
        case '0':
            return 'o';
        default:
            return ' ';
    }
}

void getAnimationKind(char from, char to, animation_bit_t* animation) {
    const char froms[] = {0, 0, 'o'};
    const char tos[] = {'a', 'o', 'a'};
    int i = 0;

    from = tolower(from);
    to = tolower(to);

    if (from == to) {
        animation->type = NONE;
        animation->reversed = false;
    }
    
    for (i = 0; i < ANIM_KINDS; i++) {
        if (froms[i] == from && tos[i] == to) {
            animation->reversed = false;
            animation->type = (animation_kind_t) i;
            return;
        }
        if (froms[i] == to && tos[i] == from) {
            animation->reversed = true;
            animation->type = (animation_kind_t) i;
            return;
        }
    }

    animation->type = NONE;
    animation->reversed = false;
}

animation_bit_t* getAnimationBits(char const* pText, int pLength, double pDuration) {
    int i;
    int lCount = 0;
    int lLast = -1;
    animation_bit_t* lBits = NULL;
    char lText[1024] = "";
    
    // On ne garde que les caractères que l'on reconnait
    for (i = 0; pText[i] != 0 && i < pLength && i < 1024; i++) {
        lText[i] = toKnownCharacter(pText[i]);
        if (lText[i] != ' ')
            lCount++;
    }
    if (i != 1024) {
        lText[i - 1] = 0;
    }
    lCount += 2; // Pour que la bouche se referme à la fin et pour annoncer la fin de la liste

    printf("(%d) %s\n", lCount, lText);

    // Si on avait reçu une longueur trop grande, on la diminue ici.
    if (i != pLength) {
        pLength = i;
    }

    lBits = (animation_bit_t*) malloc(lCount * sizeof *lBits);

    lCount = 0;
    for (i = 0; i < pLength; i++) {
        if (lText[i] == 0) {
            break;
        }
        if (lText[i] == ' ') {
            continue;
        }

        getAnimationKind(lLast == -1 ? 0 : lText[lLast], lText[i], &lBits[lCount]);
        lBits[lCount].duration = pDuration * (i - lLast) / pLength;

        lLast = i;
        lCount++;
    }
    getAnimationKind(lText[lLast], 0, &lBits[lCount]);
    lBits[lCount].duration = pDuration * (pLength - lLast) / pLength;
    lCount++;

    lBits[lCount].type = END;

    return lBits;
}
