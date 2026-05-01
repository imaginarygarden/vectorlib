#ifndef VECTORLIB_VECTOR_H
#define VECTORLIB_VECTOR_H

#include <stddef.h>

/**
 * @file vector.h
 * @brief Generic dynamic array (vector) implementation.
 *
 * Provides a resizable array capable of storing elements of arbitrary type.
 * The user must specify the size of the stored type at initialization.
 *
 * All operations are performed via raw memory copying.
 * No constructors/destructors are invoked automatically.
 */

/** Initial capacity of a new vector */
#define VECTOR_STD_CAPACITY 5

/** Growth factor when resizing up */
#define VECTOR_GROW_CAPACITY 2

/** Shrink factor when resizing down */
#define VECTOR_SHRINK_CAPACITY 2

/** Operation succeeded */
#define VECTOR_RES_OK 0

/** Operation failed */
#define VECTOR_RES_ERR 1

/**
 * @struct vector_t
 * @brief Generic dynamic array container
 *
 * @var data
 * Pointer to raw memory storing elements contiguously
 *
 * @var data_size
 * Size (in bytes) of a single element
 *
 * @var capacity
 * Allocated number of elements
 *
 * @var size
 * Current number of elements stored
 */
typedef struct vector {
    void *data;
    size_t data_size;
    size_t capacity;
    size_t size;
} vector_t;

/**
 * @brief Initialize a vector
 *
 * Allocates memory for internal storage and prepares the vector for use.
 *
 * @param vector Pointer to vector instance
 * @param data_size Size of each element in bytes
 *
 * @return VECTOR_RES_OK on success, VECTOR_RES_ERR on failure
 *
 * @note Must be paired with vector_free()
 */
int vector_init(vector_t*, size_t);

/**
 * @brief Free vector resources
 *
 * Releases all allocated memory and resets the structure.
 *
 * @param vector Pointer to vector instance
 *
 * @return VECTOR_RES_OK
 */
int vector_free(vector_t*);

/**
 * @brief Append element to the end
 *
 * Automatically grows capacity if needed.
 *
 * @param vector Vector instance
 * @param value Pointer to element to copy into vector
 *
 * @return VECTOR_RES_OK or VECTOR_RES_ERR
 */
int vector_push_back(vector_t*, void*);

/**
 * @brief Remove last element
 *
 * Optionally returns removed value.
 *
 * @param vector Vector instance
 * @param output Pointer to store removed element (must be valid memory)
 *
 * @return VECTOR_RES_OK or VECTOR_RES_ERR
 */
int vector_pop_back(vector_t*, void*);

/**
 * @brief Get element at index
 *
 * @param vector Vector instance
 * @param index Index of element (0-based)
 * @param output Pointer to store result
 *
 * @return VECTOR_RES_OK or VECTOR_RES_ERR (out of bounds)
 */
int vector_get(const vector_t*, int, void*);

/**
 * @brief Set element at index
 *
 * @param vector Vector instance
 * @param index Index of element (0-based)
 * @param value Pointer to new value
 *
 * @return VECTOR_RES_OK or VECTOR_RES_ERR (out of bounds)
 */
int vector_set(vector_t*, int, void*);

/**
 * @brief Get current capacity
 */
size_t vector_capacity(const vector_t*);

/**
 * @brief Get number of stored elements
 */
size_t vector_size(const vector_t*);

#endif //VECTORLIB_VECTOR_H
