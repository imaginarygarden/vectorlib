#include <stdio.h>
#include "vectorlib.h"

int main(void) {
    vector_t vec = {0};

    vector_init(&vec, sizeof(int));

    int values[] = {10, 20, 30};

    for (int i = 0; i < sizeof values / sizeof values[0]; ++i) {
        vector_push_back(&vec, &values[i]);
    }

    int result;

    vector_get(&vec, 1, &result);

    printf("%d\n", result);

    vector_free(&vec);

    return 0;
}
