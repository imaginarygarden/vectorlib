#ifndef VECTORLIB_VECTORLIB_H
#define VECTORLIB_VECTORLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @file vectorlib.h
 * @brief Generic dynamic array (vector) implementation in C.
 *
 * Provides a type-agnostic resizable array capable of storing elements of any type.
 * The user must specify the size of stored elements (in bytes) at initialization.
 *
 * @section memory Memory Model
 * - All data is stored as raw bytes in contiguous memory managed by malloc/realloc/free.
 * - Elements are copied byte-for-byte using memcpy(). No constructors/destructors are invoked.
 * - If you store pointers, only the pointer values are copied, not the pointed-to data.
 * - No alignment is guaranteed beyond what malloc provides (typically pointer-aligned).
 *
 * @section copying Data Copying Behavior
 * All functions that accept element data (push_back, set) perform deep memory copies.
 * The vector stores independent copies; modifying the original data after push has no effect.
 * Example:
 *   int x = 42;
 *   vector_push_back(&vec, &x);
 *   x = 100;  // vec still contains 42
 *
 * @section lifetime Vector Lifetime
 * Each vector must be initialized with vector_init() before use.
 * After initialization, the vector owns its allocated memory.
 * All vectors must be freed with vector_free() to prevent memory leaks.
 * Reusing an uninitialized vector is undefined behavior.
 */

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

    /** Element size (data_size) is invalid; must be > 0 (only from vector_init) */
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
 * @brief Initialize a vector for use.
 *
 * Allocates initial memory and prepares the vector for element storage.
 * Initial capacity is set to VECTOR_STD_CAPACITY.
 * Initial size is 0 (empty).
 *
 * @param vector
 *     Pointer to an uninitialized vector_t structure.
 *     Must not be NULL.
 * @param data_size
 *     Size (in bytes) of each element to be stored.
 *     Must be > 0. A size of 0 is rejected with VECTOR_RES_ERR_INVALID_SIZE.
 *
 * @return
 *     - VECTOR_RES_OK: Vector ready for use.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector parameter is NULL.
 *     - VECTOR_RES_ERR_INVALID_SIZE: data_size is 0.
 *     - VECTOR_RES_ERR_OVERFLOW: Initial allocation size would overflow SIZE_MAX.
 *     - VECTOR_RES_ERR_ALLOC: Memory allocation failed.
 *
 * @note Must be paired with vector_free() to avoid memory leaks.
 * @note Calling vector_init() on an already-initialized vector causes a memory leak.
 *
 * @example
 *   vector_t vec;
 *   if (vector_init(&vec, sizeof(int)) != VECTOR_RES_OK) {
 *       // Handle error
 *   }
 */
vector_result_t vector_init(vector_t*, size_t);

/**
 * @brief Release all resources held by a vector.
 *
 * Frees allocated memory and resets the vector structure.
 * After this call, the vector is uninitialized and must not be used
 * until re-initialized with vector_init().
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 *     If NULL or data==NULL, returns VECTOR_RES_ERR_INVALID_VECTOR without freeing.
 *
 * @return
 *     - VECTOR_RES_OK: Memory freed and vector reset.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *
 * @note Safe to call on uninitialized vectors (returns error but does not crash).
 * @note After free, the vector pointer itself remains valid but the structure is unusable.
 *
 * @example
 *   vector_free(&vec);
 *   // vec is now uninitialized; do not use until vector_init() called again
 */
vector_result_t vector_free(vector_t*);

/**
 * @brief Append a copy of an element to the end.
 *
 * Automatically grows capacity (doubling by default) if needed.
 * The element data is copied; the original data is not referenced.
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 * @param value
 *     Pointer to element data to copy (must be valid memory of size data_size).
 *     Must not be NULL.
 *
 * @return
 *     - VECTOR_RES_OK: Element appended; size incremented.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *     - VECTOR_RES_ERR_INVALID_INPUT: value is NULL.
 *     - VECTOR_RES_ERR_OVERFLOW: Growing capacity would exceed SIZE_MAX.
 *     - VECTOR_RES_ERR_ALLOC: Memory reallocation failed (vector unchanged).
 *
 * @note Growth is automatic and transparent; user does not need to manage capacity.
 * @note If reallocation fails, the vector state is unchanged and can continue to be used.
 *
 * @example
 *   int x = 42;
 *   vector_push_back(&vec, &x);
 */
vector_result_t vector_push_back(vector_t*, const void*);

/**
 * @brief Remove and optionally retrieve the last element.
 *
 * Decrements size by 1. If size becomes smaller than capacity/VECTOR_SHRINK_LIMIT,
 * capacity is reduced by dividing by VECTOR_SHRINK_CAPACITY to save memory.
 * Shrinking failure does not prevent the pop operation from succeeding.
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 * @param output
 *     Optional pointer to store the removed element (size data_size bytes).
 *     If NULL, the removed element is discarded without error.
 *     If not NULL, must point to valid writable memory of size data_size.
 *
 * @return
 *     - VECTOR_RES_OK: Element removed; size decremented.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *     - VECTOR_RES_ERR_EMPTY: size is already 0; nothing to remove.
 *     - VECTOR_RES_ERR_ALLOC: Shrinking failed (pop still succeeds; capacity unchanged).
 *
 * @note output==NULL is allowed; the removed element is simply discarded.
 * @note Even if shrinking fails, the pop succeeds and size is decremented.
 *
 * @example
 *   int x;
 *   if (vector_pop_back(&vec, &x) == VECTOR_RES_OK) {
 *       printf("Removed: %d\n", x);
 *   }
 *
 *   // Or discard the value:
 *   vector_pop_back(&vec, NULL);
 */
vector_result_t vector_pop_back(vector_t*, void*);

/**
 * @brief Retrieve a copy of an element at a given index.
 *
 * The element is copied to the output pointer; the vector retains its copy.
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 * @param index
 *     0-based index of the element to retrieve.
 *     Must be < size.
 * @param output
 *     Pointer to memory where the element will be copied (size data_size bytes).
 *     Must not be NULL and must point to valid writable memory.
 *
 * @return
 *     - VECTOR_RES_OK: Element copied to output.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *     - VECTOR_RES_ERR_BOUNDS: index >= size.
 *     - VECTOR_RES_ERR_INVALID_OUTPUT: output is NULL.
 *
 * @note output is required (cannot be NULL); if you don't need the value, don't call this.
 *
 * @example
 *   int x;
 *   if (vector_get(&vec, 0, &x) == VECTOR_RES_OK) {
 *       printf("First element: %d\n", x);
 *   }
 */
vector_result_t vector_get(const vector_t*, size_t, void*);

/**
 * @brief Replace an element at a given index with a new value.
 *
 * Overwrites the element at index with a copy of the provided value.
 * Does not change size or capacity.
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 * @param index
 *     0-based index of the element to replace.
 *     Must be < size.
 * @param value
 *     Pointer to new element data to copy (must be valid memory of size data_size).
 *     Must not be NULL.
 *
 * @return
 *     - VECTOR_RES_OK: Element replaced.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *     - VECTOR_RES_ERR_BOUNDS: index >= size.
 *     - VECTOR_RES_ERR_INVALID_INPUT: value is NULL.
 *
 * @note This function does not grow the vector; index must be valid.
 *       Use vector_push_back() to add new elements at the end.
 *
 * @example
 *   int new_val = 99;
 *   vector_set(&vec, 0, &new_val);
 */
vector_result_t vector_set(vector_t*, size_t, const void*);

/**
 * @brief Query the current allocated capacity.
 *
 * Returns the number of elements for which space is currently allocated.
 * This is always >= size.
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 * @param output
 *     Pointer to size_t where capacity will be stored.
 *     Must not be NULL.
 *
 * @return
 *     - VECTOR_RES_OK: Capacity copied to output.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *     - VECTOR_RES_ERR_INVALID_OUTPUT: output is NULL.
 *
 * @example
 *   size_t cap;
 *   vector_capacity(&vec, &cap);
 *   printf("Capacity: %zu\n", cap);
 */
vector_result_t vector_capacity(const vector_t*, size_t*);

/**
 * @brief Query the current number of stored elements.
 *
 * Returns the number of elements currently in the vector.
 * Valid indices are 0 to size-1.
 *
 * @param vector
 *     Pointer to an initialized vector_t.
 * @param output
 *     Pointer to size_t where size will be stored.
 *     Must not be NULL.
 *
 * @return
 *     - VECTOR_RES_OK: Size copied to output.
 *     - VECTOR_RES_ERR_INVALID_VECTOR: vector is NULL or uninitialized.
 *     - VECTOR_RES_ERR_INVALID_OUTPUT: output is NULL.
 *
 * @example
 *   size_t sz;
 *   vector_size(&vec, &sz);
 *   printf("Elements stored: %zu\n", sz);
 */
vector_result_t vector_size(const vector_t*, size_t*);

#ifdef __cplusplus
}
#endif

#endif //VECTORLIB_VECTORLIB_H
