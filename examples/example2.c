#include <stdio.h>
#include "vectorlib.h"

int main(void) {
    vector_t vec;

    if (vector_init(&vec, sizeof(int)) != VECTOR_RES_OK)
        return 1;

    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    for (size_t i = 0; i < vector_size(&vec); i++) {
        int value;
        vector_get(&vec, i, &value);
        printf("%d\n", value);
    }

    vector_free(&vec);
    return 0;
}
