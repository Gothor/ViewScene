#include <stdlib.h>
#include <iostream>
#include "TextToAnim.h"

char toKnownCharacter(char c) {
    switch (c) {
				case 'i':
				case 'j':
            return 'i';
				case 'A':
        case 'a':
				case 'E':
            return 'a';
        case 'o':
        case 'O':
        case '0':
				case 'y':
				case '@':
            return 'o';
        case 'b':
        case 'm':
        case 'p':
				case 'f':
				case 'd':
				case 't':
				case 'v':
            return 'b';
        default:
            return ' ';
    }
}

void getAnimationKind(char from, char to, animation_bit_t* animation) {
    const char froms[] = {'b', 'b', 'o', 'b', 'a', 'o'};
    const char tos[] = {'a', 'o', 'a', 'i', 'i', 'i'};
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
    if (i == 1024) {
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

        getAnimationKind(lLast == -1 ? 'b' : lText[lLast], lText[i], &lBits[lCount]);
        lBits[lCount].duration = pDuration * (i - lLast) / pLength;

        lLast = i;
        lCount++;
    }
    printf("Duration: %lf\n", pDuration);
    printf("%lf\n", pDuration * (pLength - lLast) / pLength);
    getAnimationKind(lText[lLast], 'b', &lBits[lCount]);
    lBits[lCount].duration = pDuration * (pLength - lLast) / pLength;
    lCount++;

    lBits[lCount].type = END;

    return lBits;
}
