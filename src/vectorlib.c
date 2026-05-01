#include "vectorlib.h"
#include <stdlib.h>
#include <stddef.h>
#include <memory.h>

#ifdef DEBUG_MODE
    #include <stdio.h>
    #define DEBUG_PRINT(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
    #define DEBUG_ERROR(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
    #define DEBUG_ERROR(fmt, ...)
#endif

int vector_init(vector_t *vector, size_t data_size) {
    if (data_size == 0) {
        DEBUG_ERROR("[VECTORLIB] Failed to initialize vector %p due to invalid data size of %zu\n", vector, data_size);

        return VECTOR_RES_ERR;
    }

    vector->capacity = VECTOR_STD_CAPACITY;

    vector->data_size = data_size;

    vector->data = calloc(vector->capacity, vector->data_size);

    if (vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to initialize vector %p\n", vector);

        return VECTOR_RES_ERR;
    }

    vector->size = 0;

    DEBUG_PRINT("[VECTORLIB] Initialized vector %p\n", vector);

    return VECTOR_RES_OK;
}

int vector_free(vector_t *vector) {
    free(vector->data);

    vector->data = NULL;

    vector->data_size = 0;

    vector->capacity = 0;

    vector->size = 0;

    DEBUG_PRINT("[VECTORLIB] Destroyed vector %p\n", vector);

    return VECTOR_RES_OK;
}

int vector_push_back(vector_t *vector, void *value) {
    if (vector->size >= vector->capacity) {
        size_t old_capacity = vector->capacity;

        vector->capacity *= VECTOR_GROW_CAPACITY;

        void *temp = realloc(vector->data, vector->capacity * vector->data_size);

        if (temp == NULL) {
            DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p (%zu -> %zu)\n", vector, old_capacity, vector->capacity);

            vector->capacity /= VECTOR_GROW_CAPACITY;

            return VECTOR_RES_ERR;
        }

        vector->data = temp;

        memset((char *)vector->data + old_capacity * vector->data_size, 0, (vector->capacity - old_capacity) * vector->data_size);

        DEBUG_PRINT("[VECTORLIB] Resized vector %p (%zu -> %zu)\n", vector, old_capacity, vector->capacity);
    }

    memcpy((char *)vector->data + vector->size++ * vector->data_size, value, vector->data_size);

    DEBUG_PRINT("[VECTORLIB] Pushed to vector %p with new size of %zu\n", vector, vector->size);

    return VECTOR_RES_OK;
}

int vector_pop_back(vector_t *vector, void *output) {
    if (vector->size <= 0) {
        DEBUG_ERROR("[VECTORLIB] Failed to pop from vector %p due to empty content\n", vector);

        return VECTOR_RES_ERR;
    }

    memcpy(output, (char *)vector->data + --vector->size * vector->data_size, vector->data_size);

    memset((char *)vector->data + vector->size * vector->data_size, 0, vector->data_size);

    if (vector->size < vector->capacity / VECTOR_SHRINK_CAPACITY) {
        size_t old_capacity = vector->capacity;

        vector->capacity /= VECTOR_SHRINK_CAPACITY;

        void *temp = realloc(vector->data, vector->capacity * vector->data_size);

        if (temp == NULL) {
            DEBUG_ERROR("[VECTORLIB] Failed to shrink vector %p (%zu -> %zu)\n", vector, old_capacity, vector->capacity);

            vector->capacity *= VECTOR_SHRINK_CAPACITY;

            return VECTOR_RES_ERR;
        }

        vector->data = temp;

        DEBUG_PRINT("[VECTORLIB] Shrinked vector %p (%zu -> %zu)\n", vector, old_capacity, vector->capacity);
    }


    DEBUG_PRINT("[VECTORLIB] Popped from vector %p with new size of %zu\n", vector, vector->size);

    return VECTOR_RES_OK;
}

int vector_get(const vector_t *vector, int index, void *output) {
    if (index < 0 || index >= vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve value from vector %p at index %d\n", vector, index);

        return VECTOR_RES_ERR;
    }

    memcpy(output, (char *)vector->data + index * vector->data_size, vector->data_size);

    DEBUG_PRINT("[VECTORLIB] Retrieved value from vector %p at index %d\n", vector, index);

    return VECTOR_RES_OK;
}

int vector_set(vector_t *vector, int index, void *value) {
    if (index < 0 || index >= vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to set value in vector %p at index %d\n", vector, index);

        return VECTOR_RES_ERR;
    }

    memcpy((char *)vector->data + index * vector->data_size, value, vector->data_size);

    DEBUG_PRINT("[VECTORLIB] Set value in vector %p at index %d\n", vector, index);

    return VECTOR_RES_OK;
}

size_t vector_capacity(const vector_t *vector) {
    DEBUG_PRINT("[VECTORLIB] Retrieved capacity of vector %p\n", vector);

    return vector->capacity;
}

size_t vector_size(const vector_t *vector) {
    DEBUG_PRINT("[VECTORLIB] Retrieved size of vector %p\n", vector);

    return vector->size;
}
