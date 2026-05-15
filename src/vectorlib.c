#ifdef __cplusplus
extern "C" {
#endif

#include "vectorlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef DEBUG_MODE

#include <stdio.h>

#define DEBUG_PRINT(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
#define DEBUG_ERROR(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#define DEBUG_ERROR(fmt, ...)
#endif

vector_result_t vector_init(vector_t *restrict vector, const size_t data_size) {
    if (data_size == 0) {
        DEBUG_ERROR("[VECTORLIB] Failed to initialize vector %p due to invalid data size of %zu\n", vector, data_size);

        return VECTOR_RES_ERR_INVALID_SIZE;
    }

    if (vector == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to initialize vector %p due to invalid pointer\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (VECTOR_STD_CAPACITY > SIZE_MAX / data_size) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p due to capacity overflow\n", vector);

        return VECTOR_RES_ERR_OVERFLOW;
    }

    vector->capacity = VECTOR_STD_CAPACITY;

    vector->data_size = data_size;

    vector->data = calloc(vector->capacity, vector->data_size);

    if (vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to initialize vector %p\n", vector);

        return VECTOR_RES_ERR_ALLOC;
    }

    vector->size = 0;

    DEBUG_PRINT("[VECTORLIB] Initialized vector %p\n", vector);

    return VECTOR_RES_OK;
}

vector_result_t vector_free(vector_t *restrict vector) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to free invalid vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    free(vector->data);

    vector->data = NULL;

    vector->data_size = 0;

    vector->capacity = 0;

    vector->size = 0;

    DEBUG_PRINT("[VECTORLIB] Destroyed vector %p\n", vector);

    return VECTOR_RES_OK;
}

vector_result_t vector_push_back(vector_t *restrict vector, const void *restrict value) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to push to uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (value == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to push to vector %p due to invalid input pointer\n", vector);

        return VECTOR_RES_ERR_INVALID_INPUT;
    }

    if (vector->size >= vector->capacity) {
        if (VECTOR_CHECK_MULT_OVERFLOW(vector->capacity, VECTOR_GROW_FACTOR)) {
            DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p due to capacity overflow\n", vector);

            return VECTOR_RES_ERR_OVERFLOW;
        }

        vector_result_t resize_result = vector_reserve(vector, vector->capacity * VECTOR_GROW_FACTOR);

        if (resize_result != VECTOR_RES_OK) {
            return resize_result;
        }
    }

    memcpy(VECTOR_GET_ADDRESS(vector, vector->size++), value, vector->data_size);

    DEBUG_PRINT("[VECTORLIB] Pushed to vector %p with new size of %zu\n", vector, vector->size);

    return VECTOR_RES_OK;
}

vector_result_t vector_pop_back(vector_t *restrict vector, void *restrict output) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to pop from uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (vector->size == 0) {
        DEBUG_ERROR("[VECTORLIB] Failed to pop from vector %p due to empty content\n", vector);

        return VECTOR_RES_ERR_EMPTY;
    }

    if (output != NULL) {
        DEBUG_PRINT("[VECTORLIB] Popped value from vector %p was copied to output parameter\n", vector);

        memcpy(output, VECTOR_GET_ADDRESS(vector, vector->size - 1), vector->data_size);
    }

    --vector->size;

    if (vector->size < vector->capacity / VECTOR_SHRINK_LIMIT) {
        vector_result_t shrink_result = vector_shrink(vector, vector->capacity / VECTOR_SHRINK_FACTOR);

        if (shrink_result != VECTOR_RES_OK) {
            return shrink_result;
        }
    }

    DEBUG_PRINT("[VECTORLIB] Popped from vector %p with new size of %zu\n", vector, vector->size);

    return VECTOR_RES_OK;
}

vector_result_t vector_get(const vector_t *restrict vector, const size_t index, void *restrict output) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve value from uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (index >= vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve value from vector %p at index %zu\n", vector, index);

        return VECTOR_RES_ERR_BOUNDS;
    }

    if (output == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve value from vector %p at index %zu due to invalid output parameter\n", vector, index);

        return VECTOR_RES_ERR_INVALID_OUTPUT;
    }

    memcpy(output, VECTOR_GET_ADDRESS(vector, index), vector->data_size);

    DEBUG_PRINT("[VECTORLIB] Retrieved value from vector %p at index %zu\n", vector, index);

    return VECTOR_RES_OK;
}

vector_result_t vector_set(vector_t *restrict vector, const size_t index, const void *restrict value) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to set value in uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (index >= vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to set value in vector %p out of bounds\n", vector);

        return VECTOR_RES_ERR_BOUNDS;
    }

    if (value == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to set value in vector %p at index %zu due to invalid input pointer\n", vector, index);

        return VECTOR_RES_ERR_INVALID_INPUT;
    }

    memcpy(VECTOR_GET_ADDRESS(vector, index), value, vector->data_size);

    DEBUG_PRINT("[VECTORLIB] Set value in vector %p at index %zu\n", vector, index);

    return VECTOR_RES_OK;
}

vector_result_t vector_clear(vector_t *restrict vector) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to clear uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    vector->size = 0;

    DEBUG_PRINT("[VECTORLIB] Cleared vector %p\n", vector);

    return VECTOR_RES_OK;
}

vector_result_t vector_reserve(vector_t *restrict vector, const size_t value) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (value <= vector->capacity) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p due to invalid size\n", vector);

        return VECTOR_RES_ERR_INVALID_SIZE;
    }

    size_t old_capacity = vector->capacity;

    size_t new_capacity = value;

    if (new_capacity > SIZE_MAX / vector->data_size) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p due to capacity overflow\n", vector);

        return VECTOR_RES_ERR_OVERFLOW;
    }

    void *temp = realloc(vector->data, new_capacity * vector->data_size);

    if (temp == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p (%zu -> %zu)\n", vector, old_capacity,
                    new_capacity);

        return VECTOR_RES_ERR_ALLOC;
    }

    vector->capacity = new_capacity;

    vector->data = temp;

    DEBUG_PRINT("[VECTORLIB] Resized vector %p (%zu -> %zu)\n", vector, old_capacity, vector->capacity);

    return VECTOR_RES_OK;
}

vector_result_t vector_shrink(vector_t *restrict vector, const size_t value) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (value >= vector->capacity || value < vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to resize vector %p due to invalid size\n", vector);

        return VECTOR_RES_ERR_INVALID_SIZE;
    }

    size_t old_capacity = vector->capacity;

    size_t new_capacity = VECTOR_CLAMP_CAPACITY(value);

    void *temp = realloc(vector->data, new_capacity * vector->data_size);

    if (temp == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to shrink vector %p (%zu -> %zu)\n", vector, old_capacity,
                    new_capacity);

        return VECTOR_RES_ERR_ALLOC;
    }

    vector->capacity = new_capacity;

    vector->data = temp;

    DEBUG_PRINT("[VECTORLIB] Shrank vector %p (%zu -> %zu)\n", vector, old_capacity, new_capacity);

    return VECTOR_RES_OK;
}

vector_result_t vector_remove(vector_t *restrict vector, const size_t index, const size_t amount) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to remove from uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (index + amount > vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to remove from vector %p out of bounds\n", vector);

        return VECTOR_RES_ERR_BOUNDS;
    }

    if (index + amount < vector->size) {
        DEBUG_PRINT("[VECTORLIB] Residual values from vector %p were moved\n", vector);

        memmove(VECTOR_GET_ADDRESS(vector, index), VECTOR_GET_ADDRESS(vector, index + amount), vector->data_size * (vector->size - index - amount));
    }

    vector->size -= amount;

    DEBUG_PRINT("[VECTORLIB] Removed from vector %p with new size of %zu\n", vector, vector->size);

    return VECTOR_RES_OK;
}

vector_result_t vector_insert(vector_t *restrict vector, const size_t index, const void *restrict value) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to insert into uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (vector->size >= vector->capacity) {
        if (VECTOR_CHECK_MULT_OVERFLOW(vector->capacity, VECTOR_GROW_FACTOR)) {
            DEBUG_ERROR("[VECTORLIB] Failed to insert into vector %p due to capacity overflow\n", vector);

            return VECTOR_RES_ERR_OVERFLOW;
        }

        vector_result_t resize_result = vector_reserve(vector, vector->capacity * VECTOR_GROW_FACTOR);

        if (resize_result != VECTOR_RES_OK) {
            return resize_result;
        }
    }

    if (index > vector->size) {
        DEBUG_ERROR("[VECTORLIB] Failed to insert into vector %p out of bounds\n", vector);

        return VECTOR_RES_ERR_BOUNDS;
    }

    if (value == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to insert into vector %p due to invalid input\n", vector);

        return VECTOR_RES_ERR_INVALID_INPUT;
    }

    memmove(VECTOR_GET_ADDRESS(vector, index + 1), VECTOR_GET_ADDRESS(vector, index), (vector->size - index) * vector->data_size);

    ++vector->size;

    vector_result_t set_result = vector_set(vector, index, value);

    if (set_result != VECTOR_RES_OK) {
        return set_result;
    }

    return VECTOR_RES_OK;
}

vector_result_t vector_capacity(const vector_t *restrict vector, size_t *restrict output) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve capacity of uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (output == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve capacity from vector %p due to invalid output parameter\n", vector);

        return VECTOR_RES_ERR_INVALID_OUTPUT;
    }

    DEBUG_PRINT("[VECTORLIB] Retrieved capacity of vector %p\n", vector);

    *output = vector->capacity;

    return VECTOR_RES_OK;
}

vector_result_t vector_size(const vector_t *restrict vector, size_t *restrict output) {
    if (vector == NULL || vector->data == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve size of uninitialized vector %p\n", vector);

        return VECTOR_RES_ERR_INVALID_VECTOR;
    }

    if (output == NULL) {
        DEBUG_ERROR("[VECTORLIB] Failed to retrieve size of vector %p due to invalid output parameter\n", vector);

        return VECTOR_RES_ERR_INVALID_OUTPUT;
    }

    DEBUG_PRINT("[VECTORLIB] Retrieved size of vector %p\n", vector);

    *output = vector->size;

    return VECTOR_RES_OK;
}

#ifdef __cplusplus
}
#endif
