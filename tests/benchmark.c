#include <stdio.h>
#include <time.h>
#include "vectorlib.h"

#define N 1'000'000

int main(void) {
    vector_t vec;
    vector_init(&vec, sizeof(int));

    clock_t start = clock();

    for (int i = 0; i < N; ++i) {
        vector_push_back(&vec, &i);
    }

    clock_t end = clock();

    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;

    printf("Push %d elements: %f seconds\n", N, time_spent);

    vector_free(&vec);
    return 0;
}
