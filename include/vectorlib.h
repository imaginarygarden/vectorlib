#ifndef VECTORLIB_VECTORLIB_H
#define VECTORLIB_VECTORLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @warning This container performs raw byte copies and does not call
 * constructors, destructors, or custom copy logic.
 * Complex C++ objects and ownership-sensitive data structures should not
 * be stored directly.
 *
 * @warning Any pointer into vector storage may become invalid after
 * vector_push_back(), vector_reserve(), vector_shrink(), or vector_pop_back().
 *
 * @note This library is not thread-safe.
 * Concurrent access requires external synchronization.
 */

/** @brief Initial capacity allocated for a new vector (elements, not bytes) */
#define VECTOR_STD_CAPACITY 5

/** @brief Multiplication factor when growing capacity */
#define VECTOR_GROW_FACTOR 2

/** @brief Division factor when shrinking capacity */
#define VECTOR_SHRINK_FACTOR 2

/**
 * @brief Threshold for automatic shrinking (as divisor of capacity).
 * Shrinking occurs when: size < capacity / VECTOR_SHRINK_LIMIT
 * Prevents thrashing when repeatedly pushing/popping near capacity boundary.
 */
#define VECTOR_SHRINK_LIMIT 4

/** @cond INTERNAL */

/** @brief Computes the byte address of an element at index */
#define VECTOR_GET_ADDRESS(data, data_size, index) \
    ((char *)(data) + ((index) * (data_size)))

/** @brief Checks whether capacity growth would overflow SIZE_MAX */
#define VECTOR_CHECK_OVERFLOW(capacity, factor) \
    ((capacity) > SIZE_MAX / (factor))

/** @brief Ensures capacity never shrinks below VECTOR_STD_CAPACITY */
#define VECTOR_CLAMP_CAPACITY(capacity) \
    ((capacity) < VECTOR_STD_CAPACITY ? VECTOR_STD_CAPACITY : (capacity))

/** @endcond */

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
 * @brief Generic dynamically-resizing array container.
 *
 * Stores contiguous elements of fixed size determined during initialization.
 * Elements are copied into internal storage using memcpy.
 *
 * The vector grows automatically when capacity is exceeded and may shrink
 * automatically when enough elements are removed.
 *
 * This implementation is type-agnostic and intended for POD/trivially-copyable
 * data types.
 *
 * Members should not be modified directly outside the library.
 */
typedef struct vector {
    /** Pointer to contiguous element storage */
    void *data;

    /** Size of a single element in bytes */
    size_t data_size;

    /** Allocated capacity in elements */
    size_t capacity;

    /** Number of valid stored elements */
    size_t size;
} vector_t;

/**
 * @brief Initializes a vector.
 *
 * Allocates internal storage with an initial capacity of
 * VECTOR_STD_CAPACITY elements.
 *
 * @param vector Pointer to vector instance.
 * @param data_size Size of each stored element in bytes.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL.
 * @return VECTOR_RES_ERR_INVALID_SIZE if data_size is 0.
 * @return VECTOR_RES_ERR_OVERFLOW if allocation size would overflow SIZE_MAX.
 * @return VECTOR_RES_ERR_ALLOC if memory allocation fails.
 *
 * @note The vector must later be released using vector_free().
 */
vector_result_t vector_init(vector_t*, const size_t);

/**
 * @brief Releases all memory owned by the vector.
 *
 * Frees internal storage and resets all fields to zero/NULL.
 *
 * @param vector Pointer to initialized vector.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 *
 * @note Safe cleanup requires successful initialization beforehand.
 * @note After this call, the vector may be reinitialized using vector_init().
 */
vector_result_t vector_free(vector_t*);

/**
 * @brief Appends an element to the end of the vector.
 *
 * Automatically grows capacity when size reaches capacity.
 * The element is copied into internal storage using memcpy().
 *
 * @param vector Pointer to initialized vector.
 * @param value Pointer to element to append.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_INVALID_INPUT if value is NULL.
 * @return VECTOR_RES_ERR_OVERFLOW if capacity growth would overflow.
 * @return VECTOR_RES_ERR_ALLOC if reallocation fails.
 *
 * @note value must point to at least data_size bytes.
 */
vector_result_t vector_push_back(vector_t*, const void*);

/**
 * @brief Removes the last element from the vector.
 *
 * Optionally copies the removed value into output.
 * The vector may automatically shrink after removal.
 *
 * @param vector Pointer to initialized vector.
 * @param output Optional destination for removed element.
 *               May be NULL to discard the value.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_EMPTY if the vector contains no elements.
 * @return VECTOR_RES_ERR_ALLOC if shrinking reallocation fails.
 *
 * @note output must point to at least data_size bytes when non-NULL.
 */
vector_result_t vector_pop_back(vector_t*, void*);

/**
 * @brief Retrieves an element from the vector.
 *
 * Copies the element at index into output using memcpy().
 *
 * @param vector Pointer to initialized vector.
 * @param index Element index.
 * @param output Destination buffer.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_BOUNDS if index >= size.
 * @return VECTOR_RES_ERR_INVALID_OUTPUT if output is NULL.
 *
 * @note output must point to at least data_size bytes.
 */
vector_result_t vector_get(const vector_t*, const size_t, void*);

/**
 * @brief Replaces an existing element in the vector.
 *
 * Copies value into the specified index using memcpy().
 *
 * @param vector Pointer to initialized vector.
 * @param index Element index to overwrite.
 * @param value Pointer to replacement value.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_BOUNDS if index >= size.
 * @return VECTOR_RES_ERR_INVALID_INPUT if value is NULL.
 *
 * @note value must point to at least data_size bytes.
 */
vector_result_t vector_set(vector_t*, const size_t, const void*);

/**
 * @brief Removes all elements from the vector.
 *
 * Resets size to zero without releasing allocated memory.
 * Capacity remains unchanged.
 *
 * @param vector Pointer to initialized vector.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 *
 * @note Existing element data becomes logically invalid.
 */
vector_result_t vector_clear(vector_t*);

/**
 * @brief Expands vector capacity.
 *
 * Reallocates internal storage to at least value elements.
 *
 * @param vector Pointer to initialized vector.
 * @param value Requested new capacity in elements.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_INVALID_SIZE if value is smaller than current capacity.
 * @return VECTOR_RES_ERR_OVERFLOW if allocation size would overflow SIZE_MAX.
 * @return VECTOR_RES_ERR_ALLOC if reallocation fails.
 *
 * @note Existing elements are preserved.
 * @note This function never shrinks capacity.
 */
vector_result_t vector_reserve(vector_t*, const size_t);

/**
 * @brief Shrinks vector capacity.
 *
 * Reallocates internal storage to a smaller capacity.
 * Capacity is clamped to at least VECTOR_STD_CAPACITY.
 *
 * @param vector Pointer to initialized vector.
 * @param value Requested reduced capacity.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_INVALID_SIZE if value is greater than or equal to current capacity.
 * @return VECTOR_RES_ERR_ALLOC if reallocation fails.
 *
 * @note Existing elements are preserved up to the new allocation size.
 */
vector_result_t vector_shrink(vector_t*, const size_t);

// vector_result_t vector_remove(vector_t*, const size_t, const size_t, vector_t*);

// vector_result_t vector_insert(vector_t*, const size_t, const void*);

/**
 * @brief Retrieves current allocated capacity.
 *
 * @param vector Pointer to initialized vector.
 * @param output Destination for capacity value.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_INVALID_OUTPUT if output is NULL.
 */
vector_result_t vector_capacity(const vector_t*, size_t*);

/**
 * @brief Retrieves current number of stored elements.
 *
 * @param vector Pointer to initialized vector.
 * @param output Destination for size value.
 *
 * @return VECTOR_RES_OK on success.
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is NULL or uninitialized.
 * @return VECTOR_RES_ERR_INVALID_OUTPUT if output is NULL.
 */
vector_result_t vector_size(const vector_t*, size_t*);

#ifdef __cplusplus
}
#endif

#endif //VECTORLIB_VECTORLIB_H
