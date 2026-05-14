#ifndef VECTORLIB_VECTORLIB_H
#define VECTORLIB_VECTORLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/** @brief Initial capacity allocated for a new vector (elements, not bytes) */
#define VECTOR_STD_CAPACITY 5

/** @brief Multiplication factor when growing capacity */
#define VECTOR_GROW_CAPACITY 2

/** @brief Division factor when shrinking capacity */
#define VECTOR_SHRINK_CAPACITY 2

/**
 * @brief Threshold for automatic shrinking (as divisor of capacity).
 * Shrinking occurs when: size < capacity / VECTOR_SHRINK_LIMIT
 * Prevents thrashing when repeatedly pushing/popping near capacity boundary.
 */
#define VECTOR_SHRINK_LIMIT 4

/**
 * @enum vector_result_t
 * @brief Return codes for all vector operations.
 */
typedef enum vector_result {
    /** Operation succeeded */
    VECTOR_RES_OK = 0,

    /** Memory allocation or reallocation failed (malloc/calloc/realloc returned NULL) */
    VECTOR_RES_ERR_ALLOC,

    /** Requested capacity would exceed SIZE_MAX; operation aborted to prevent overflow */
    VECTOR_RES_ERR_OVERFLOW,

    /** Index parameter is >= current size */
    VECTOR_RES_ERR_BOUNDS,

    /** Vector is empty (size == 0); cannot pop or access */
    VECTOR_RES_ERR_EMPTY,

    /** Vector pointer is NULL or vector is uninitialized (data == NULL) */
    VECTOR_RES_ERR_INVALID_VECTOR,

    /** Element size is zero or requested capacity is less than current capacity */
    VECTOR_RES_ERR_INVALID_SIZE,

    /** Input data pointer is NULL where a valid pointer is required */
    VECTOR_RES_ERR_INVALID_INPUT,

    /** Output pointer is NULL where a valid pointer is required */
    VECTOR_RES_ERR_INVALID_OUTPUT
} vector_result_t;

/**
 * @struct vector_t
 * @brief Opaque generic dynamic array container.
 *
 * @var data
 *     Pointer to raw memory storing elements contiguously.
 *     NULL if uninitialized; owned and managed by vector functions.
 *
 * @var data_size
 *     Size (in bytes) of a single stored element.
 *     Set at initialization; unchanged afterward.
 *
 * @var capacity
 *     Number of elements for which space is currently allocated.
 *     Always >= size. May change during push/pop due to reallocation.
 *
 * @var size
 *     Current number of elements stored in the vector.
 *     Elements occupy indices 0 to size-1.
 *
 * @note This structure should be treated as opaque; access fields only
 *       for debugging or diagnostic purposes, never in production code.
 */
typedef struct vector {
    void *data;
    size_t data_size;
    size_t capacity;
    size_t size;
} vector_t;

/**
 * @brief Initialize a vector with a given element size.
 *
 * Allocates memory for VECTOR_STD_CAPACITY elements of size @p data_size bytes each.
 * Must be called before any other vector operations.
 *
 * @param[out] vector    Pointer to uninitialized vector_t structure
 * @param[in]  data_size Size in bytes of each element (must be > 0)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector pointer is NULL
 *         - VECTOR_RES_ERR_INVALID_SIZE if data_size is 0
 *         - VECTOR_RES_ERR_OVERFLOW if initial capacity exceeds SIZE_MAX
 *         - VECTOR_RES_ERR_ALLOC if memory allocation fails
 */
vector_result_t vector_init(vector_t*, size_t);

/**
 * @brief Free all resources associated with a vector.
 *
 * Deallocates the internal data buffer and resets vector structure fields.
 * The vector_t structure itself is not freed; caller is responsible for freeing it
 * if it was dynamically allocated.
 *
 * @param[in,out] vector Pointer to initialized vector_t structure
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 */
vector_result_t vector_free(vector_t*);

/**
 * @brief Append an element to the end of the vector.
 *
 * Grows capacity automatically if necessary. Capacity is multiplied by
 * VECTOR_GROW_CAPACITY when expansion is needed.
 *
 * @param[in,out] vector Pointer to initialized vector_t structure
 * @param[in]     value  Pointer to element data to copy (must not be NULL)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_INVALID_INPUT if value pointer is NULL
 *         - VECTOR_RES_ERR_OVERFLOW if new capacity would exceed SIZE_MAX
 *         - VECTOR_RES_ERR_ALLOC if memory reallocation fails
 */
vector_result_t vector_push_back(vector_t*, const void*);

/**
 * @brief Remove and optionally retrieve the last element from the vector.
 *
 * Decrements size. Automatically shrinks capacity if size falls below
 * capacity / VECTOR_SHRINK_LIMIT. Shrinkage does not reduce capacity below
 * VECTOR_STD_CAPACITY.
 *
 * @param[in,out] vector Pointer to initialized vector_t structure
 * @param[out]    output Pointer to buffer receiving element data, or NULL to discard
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_EMPTY if vector size is 0
 *         - VECTOR_RES_ERR_ALLOC if memory reallocation (shrinking) fails
 */
vector_result_t vector_pop_back(vector_t*, void*);

/**
 * @brief Retrieve an element from the vector without removing it.
 *
 * Copies the element at @p index to the memory pointed to by @p output.
 *
 * @param[in]  vector Pointer to initialized vector_t structure
 * @param[in]  index  Index of element (0-based, must be < size)
 * @param[out] output Pointer to buffer receiving element data (must not be NULL)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_BOUNDS if index >= size
 *         - VECTOR_RES_ERR_INVALID_OUTPUT if output pointer is NULL
 */
vector_result_t vector_get(const vector_t*, size_t, void*);

/**
 * @brief Replace an element in the vector.
 *
 * Overwrites the element at @p index with a copy of the data pointed to by @p value.
 *
 * @param[in,out] vector Pointer to initialized vector_t structure
 * @param[in]     index  Index of element (0-based, must be < size)
 * @param[in]     value  Pointer to new element data (must not be NULL)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_BOUNDS if index >= size
 *         - VECTOR_RES_ERR_INVALID_INPUT if value pointer is NULL
 */
vector_result_t vector_set(vector_t*, size_t, const void*);

/**
 * @brief Remove all elements from the vector without deallocating memory.
 *
 * Sets size to 0 but leaves capacity unchanged. Does not shrink the buffer.
 * Call vector_free() then vector_init() to reset capacity.
 *
 * @param[in,out] vector Pointer to initialized vector_t structure
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 */
vector_result_t vector_clear(vector_t*);

/**
 * @brief Increase the capacity of the vector.
 *
 * Reserves space for at least @p value additional elements beyond current capacity.
 * New capacity = old capacity + value. Does nothing if requested capacity is
 * less than or equal to current capacity (returns error).
 *
 * @param[in,out] vector Pointer to initialized vector_t structure
 * @param[in]     value  Number of additional elements to reserve (must result in capacity > current capacity)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_INVALID_SIZE if value <= (capacity - current capacity)
 *         - VECTOR_RES_ERR_OVERFLOW if new capacity would exceed SIZE_MAX
 *         - VECTOR_RES_ERR_ALLOC if memory reallocation fails
 */
vector_result_t vector_reserve(vector_t*, size_t);

/**
 * @brief Query the allocated capacity of a vector.
 *
 * Returns the number of elements the vector can hold without reallocation.
 *
 * @param[in]  vector Pointer to initialized vector_t structure
 * @param[out] output Pointer to size_t variable receiving capacity (must not be NULL)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_INVALID_OUTPUT if output pointer is NULL
 */
vector_result_t vector_capacity(const vector_t*, size_t*);

/**
 * @brief Query the current size of a vector.
 *
 * Returns the number of elements currently stored in the vector.
 *
 * @param[in]  vector Pointer to initialized vector_t structure
 * @param[out] output Pointer to size_t variable receiving size (must not be NULL)
 *
 * @return VECTOR_RES_OK on success, or error code on failure:
 *         - VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *         - VECTOR_RES_ERR_INVALID_OUTPUT if output pointer is NULL
 */
vector_result_t vector_size(const vector_t*, size_t*);

#ifdef __cplusplus
}
#endif

#endif //VECTORLIB_VECTORLIB_H
