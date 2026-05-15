#ifndef VECTORLIB_VECTORLIB_H
#define VECTORLIB_VECTORLIB_H

/**
 * @file vectorlib.h
 * @brief Generic dynamic array (vector) implementation for C
 *
 * A type-agnostic, resizable array container using void* pointers for genericity.
 * Works with any data type by specifying element size at initialization.
 *
 * Memory Management:
 * - Growth strategy: Capacity doubles when vector becomes full
 * - Shrinking strategy: Capacity is halved when size < capacity / 4
 * - Minimum capacity: Always at least VECTOR_STD_CAPACITY (5 elements)
 *
 * Error Handling:
 * - All functions return a vector_result_t status code
 * - Check return value before assuming operation succeeded
 * - No silent failures; overflow and allocation errors are reported
 *
 * Example Usage:
 *   vector_t vec;
 *   vector_init(&vec, sizeof(int));
 *
 *   int values[] = {10, 20, 30};
 *   for (int i = 0; i < 3; i++) {
 *       vector_push_back(&vec, &values[i]);
 *   }
 *
 *   int retrieved = 0;
 *   vector_get(&vec, 1, &retrieved);  // retrieved = 20
 *
 *   vector_free(&vec);
 *
 * Thread Safety: This implementation is NOT thread-safe. Synchronization
 * must be handled by the caller if used in multithreaded contexts.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief Initial capacity allocated for new vectors
 *
 * Vectors are pre-allocated with space for this many elements
 * even if they start empty. Reduces early reallocations.
 */
#define VECTOR_STD_CAPACITY 5

/**
 * @brief Growth multiplier when vector reaches capacity
 *
 * When size == capacity, the capacity is multiplied by this factor.
 * Value of 2 means doubling (amortized O(1) insertion, standard approach).
 */
#define VECTOR_GROW_FACTOR 2

/**
 * @brief Shrink multiplier applied when shrinking is triggered
 *
 * When shrinking occurs, new capacity = current capacity / this factor.
 * Value of 2 means halving the capacity.
 */
#define VECTOR_SHRINK_FACTOR 2

/**
 * @brief Threshold ratio for triggering shrinking during pop_back
 *
 * Shrinking is triggered when size < capacity / this value.
 * Value of 4 means shrinking occurs when size drops below 1/4 of capacity.
 * Prevents excessive memory waste while avoiding thrashing.
 */
#define VECTOR_SHRINK_LIMIT 4

/**
 * @brief Computes the byte offset of an element at given index
 *
 * @param vector Pointer to initialized vector_t
 * @param index Element index (must be < vector->size)
 * @return Void pointer to the element data
 *
 * @warning Does NOT perform bounds checking. Caller is responsible.
 * @note Used internally; macros are for internal library use.
 */
#define VECTOR_GET_ADDRESS(vector, index) \
    ((char *)(vector->data) + ((index) * (vector->data_size)))

/**
 * @brief Checks if multiplying capacity by factor would overflow SIZE_MAX
 *
 * @param capacity Current capacity value
 * @param factor Multiplication factor
 * @return Non-zero (true) if overflow would occur; 0 (false) otherwise
 *
 * Used before resizing operations to prevent integer overflow.
 */
#define VECTOR_CHECK_MULT_OVERFLOW(capacity, factor) \
    ((capacity) > SIZE_MAX / (factor))

/**
 * @brief Checks if adding value to capacity would overflow SIZE_MAX
 *
 * @param capacity Current capacity value
 * @param value Value to add
 * @return Non-zero (true) if overflow would occur; 0 (false) otherwise
 *
 * Used for addition-based capacity calculations to prevent integer overflow.
 */
#define VECTOR_CHECK_ADD_OVERFLOW(capacity, value) \
    ((capacity) > SIZE_MAX - (value))

/**
 * @brief Ensures capacity is at least VECTOR_STD_CAPACITY
 *
 * @param capacity Proposed capacity value
 * @return capacity if >= VECTOR_STD_CAPACITY; otherwise VECTOR_STD_CAPACITY
 *
 * Guarantees minimum capacity to avoid undersized allocations.
 */
#define VECTOR_CLAMP_CAPACITY(capacity) \
    ((capacity) < VECTOR_STD_CAPACITY ? VECTOR_STD_CAPACITY : (capacity))

/**
 * @enum vector_result
 * @brief Status codes returned by vector operations
 */
typedef enum vector_result {
    /** Operation completed successfully */
    VECTOR_RES_OK = 0,

    /** Memory allocation failed (malloc/calloc/realloc returned NULL) */
    VECTOR_RES_ERR_ALLOC,

    /** Integer overflow detected during capacity calculation */
    VECTOR_RES_ERR_OVERFLOW,

    /** Index is out of bounds for the current vector size */
    VECTOR_RES_ERR_BOUNDS,

    /** Operation requires non-empty vector, but vector is empty */
    VECTOR_RES_ERR_EMPTY,

    /** Vector pointer is NULL or vector is uninitialized */
    VECTOR_RES_ERR_INVALID_VECTOR,

    /** Size value is invalid (e.g., zero when non-zero required) */
    VECTOR_RES_ERR_INVALID_SIZE,

    /** Input pointer parameter is NULL or invalid */
    VECTOR_RES_ERR_INVALID_INPUT,

    /** Output pointer parameter is NULL or invalid */
    VECTOR_RES_ERR_INVALID_OUTPUT
} vector_result_t;

/**
 * @struct vector
 * @brief Dynamic array container structure
 *
 * Holds metadata and data pointer for a generic resizable array.
 * Users should not directly manipulate fields; use provided API functions.
 */
typedef struct vector {
    /** Pointer to allocated memory storing elements */
    void *data;

    /** Size in bytes of each element */
    size_t data_size;

    /** Current allocated capacity (max elements that fit without resize) */
    size_t capacity;

    /** Current number of elements stored in the vector */
    size_t size;
} vector_t;

/**
 * @brief Initializes a vector with elements of specified size
 *
 * Allocates initial memory and sets up the vector_t structure.
 * Must be called before any other vector operation on a given vector.
 *
 * @param vector Pointer to uninitialized vector_t struct
 * @param data_size Size of each element in bytes (e.g., sizeof(int), sizeof(double))
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_SIZE if data_size is 0
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector pointer is NULL
 * @return VECTOR_RES_ERR_OVERFLOW if VECTOR_STD_CAPACITY * data_size would overflow
 * @return VECTOR_RES_ERR_ALLOC if memory allocation fails
 *
 * @note Initial capacity is VECTOR_STD_CAPACITY (5 elements)
 * @note Does not require initialization of vector fields; function handles setup
 *
 * Example:
 *   vector_t vec;
 *   vector_result_t res = vector_init(&vec, sizeof(int));
 *   if (res != VECTOR_RES_OK) { ... handle error ... }
 */
vector_result_t vector_init(vector_t*, const size_t);

/**
 * @brief Frees all memory allocated by a vector
 *
 * Deallocates the vector's data buffer and resets all fields to safe state.
 * After calling this function, the vector must be re-initialized before reuse.
 *
 * @param vector Pointer to initialized vector_t struct
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector or vector->data is NULL
 *
 * @note Sets all vector fields to 0/NULL after freeing
 * @note Safe to call even if vector_init() previously failed
 *
 * Example:
 *   vector_result_t res = vector_free(&vec);
 */
vector_result_t vector_free(vector_t*);

/**
 * @brief Appends an element to the end of the vector
 *
 * Adds a new element at index size. Automatically resizes (doubles capacity)
 * if necessary. Element data is copied via memcpy.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param value Pointer to element data to append (must not be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_INVALID_INPUT if value pointer is NULL
 * @return VECTOR_RES_ERR_OVERFLOW if resizing would cause integer overflow
 * @return VECTOR_RES_ERR_ALLOC if memory reallocation fails during resize
 *
 * @note Time complexity: O(1) amortized; O(n) when resize occurs
 * @note Caller retains ownership of value; it is copied, not referenced
 *
 * Example:
 *   int val = 42;
 *   vector_push_back(&vec, &val);
 */
vector_result_t vector_push_back(vector_t*, const void*);

/**
 * @brief Removes and optionally retrieves the last element
 *
 * Decrements size. If output is provided, the removed element is copied to it.
 * May automatically shrink capacity if size falls below threshold.
 * Shrinking is triggered when size < capacity / VECTOR_SHRINK_LIMIT.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param output Pointer to buffer to receive removed element (may be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_EMPTY if vector contains no elements
 * @return VECTOR_RES_ERR_OVERFLOW if shrinking would cause integer overflow
 * @return VECTOR_RES_ERR_ALLOC if memory reallocation fails during shrink
 *
 * @note output buffer must be large enough to hold one element (data_size bytes)
 * @note output may be NULL; element is discarded in that case
 * @note Time complexity: O(1) amortized; O(n) when shrinking occurs
 *
 * Example:
 *   int removed_val = 0;
 *   vector_pop_back(&vec, &removed_val);  // Retrieve last element
 *   vector_pop_back(&vec, NULL);          // Discard last element
 */
vector_result_t vector_pop_back(vector_t*, void*);

/**
 * @brief Retrieves a copy of the element at the specified index
 *
 * Copies the element data to the output buffer without modifying the vector.
 * Index must be within [0, size).
 *
 * @param vector Pointer to initialized vector_t struct
 * @param index Zero-based index of element to retrieve (must be < size)
 * @param output Pointer to buffer to receive element data (must not be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_BOUNDS if index >= vector size
 * @return VECTOR_RES_ERR_INVALID_OUTPUT if output pointer is NULL
 *
 * @note output buffer must be large enough to hold one element (data_size bytes)
 * @note Time complexity: O(1)
 * @note Does not modify vector state
 *
 * Example:
 *   int val = 0;
 *   vector_get(&vec, 2, &val);  // Get element at index 2
 */
vector_result_t vector_get(const vector_t*, const size_t, void*);

/**
 * @brief Overwrites the element at the specified index
 *
 * Replaces existing element data at the given index. Index must be within [0, size).
 * Element data is copied via memcpy.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param index Zero-based index of element to overwrite (must be < size)
 * @param value Pointer to new element data (must not be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_BOUNDS if index >= vector size
 * @return VECTOR_RES_ERR_INVALID_INPUT if value pointer is NULL
 *
 * @note Time complexity: O(1)
 * @note Does not affect vector size or capacity
 * @note Caller retains ownership of value; it is copied, not referenced
 *
 * Example:
 *   int new_val = 99;
 *   vector_set(&vec, 1, &new_val);  // Set element at index 1
 */
vector_result_t vector_set(vector_t*, const size_t, const void*);

/**
 * @brief Removes all elements from the vector
 *
 * Sets size to zero but preserves capacity (no memory deallocation).
 * Vector retains allocated memory for future insertions.
 *
 * @param vector Pointer to initialized vector_t struct
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 *
 * @note Time complexity: O(1)
 * @note Capacity remains unchanged
 * @note Does not call destructors or free nested structures
 *
 * Example:
 *   vector_clear(&vec);  // Vector is now empty but still has allocated capacity
 */
vector_result_t vector_clear(vector_t*);

/**
 * @brief Increases vector capacity to at least the specified value
 *
 * Allocates additional memory if current capacity is less than the requested value.
 * Useful to pre-allocate space before many insertions to avoid repeated resizing.
 * New capacity must be strictly greater than current capacity.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param value Desired minimum capacity (must be > current capacity)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_INVALID_SIZE if value <= current capacity
 * @return VECTOR_RES_ERR_OVERFLOW if value * data_size would overflow SIZE_MAX
 * @return VECTOR_RES_ERR_ALLOC if memory reallocation fails
 *
 * @note Does not affect vector size; only capacity
 * @note Fails if value is not strictly greater than current capacity
 * @note Time complexity: O(n) due to memory copy during reallocation
 *
 * Example:
 *   vector_reserve(&vec, 1000);  // Pre-allocate space for 1000 elements
 */
vector_result_t vector_reserve(vector_t*, const size_t);

/**
 * @brief Reduces vector capacity to the specified value
 *
 * Deallocates excess memory when capacity is larger than needed.
 * New capacity must be less than current capacity but >= current size.
 * Final capacity is clamped to at least VECTOR_STD_CAPACITY.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param value Desired capacity (must satisfy: size <= value < current capacity)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_INVALID_SIZE if value >= current capacity OR value < current size
 * @return VECTOR_RES_ERR_ALLOC if memory reallocation fails
 *
 * @note Capacity is clamped to minimum of VECTOR_STD_CAPACITY
 * @note Size remains unchanged
 * @note Called automatically by pop_back() when threshold is reached
 * @note Time complexity: O(n) due to memory copy during reallocation
 *
 * Example:
 *   vector_shrink(&vec, 10);  // Reduce capacity to 10 elements
 */
vector_result_t vector_shrink(vector_t*, const size_t);

/**
 * @brief Removes a contiguous range of elements from the vector
 *
 * Removes amount elements starting at index, shifting remaining elements left.
 * If output is provided (non-NULL), removed elements are copied into a new vector.
 * If output->data is already allocated, it will be freed before assignment.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param index Zero-based starting index of elements to remove (must be < size)
 * @param amount Number of elements to remove (index + amount must be <= size)
 * @param output Pointer to vector_t struct to receive removed elements (may be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_BOUNDS if index + amount > vector size
 * @return VECTOR_RES_ERR_INVALID_SIZE if output initialization fails
 * @return VECTOR_RES_ERR_ALLOC if memory allocation for output fails
 *
 * @note If output is NULL, removed elements are discarded
 * @note If output is provided, it receives a new initialized vector with removed elements
 * @note output vector must be freed by caller when no longer needed
 * @note If output->data is already allocated, existing vector will be freed
 * @note Size decreases by amount; capacity may remain unchanged
 * @note Time complexity: O(n) where n = number of elements after removed range
 *
 * Example:
 *   vector_t removed;
 *   vector_remove(&vec, 2, 5, &removed);  // Remove 5 elements at index 2, store in removed
 *   vector_remove(&vec, 0, 1, NULL);      // Remove 1 element at index 0, discard it
 *   vector_free(&removed);
 */
vector_result_t vector_remove(vector_t*, const size_t, const size_t, vector_t*);

/**
 * @brief Inserts an element at the specified index
 *
 * Shifts all elements from index onwards to the right and inserts the new element.
 * Automatically resizes (doubles capacity) if necessary.
 * Index must be within [0, size] (index == size appends to end).
 *
 * @param vector Pointer to initialized vector_t struct
 * @param index Zero-based position to insert at (must be <= size)
 * @param value Pointer to element data to insert (must not be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_BOUNDS if index > vector size
 * @return VECTOR_RES_ERR_INVALID_INPUT if value pointer is NULL
 * @return VECTOR_RES_ERR_OVERFLOW if resizing would cause integer overflow
 * @return VECTOR_RES_ERR_ALLOC if memory reallocation fails during resize
 *
 * @note Time complexity: O(n) where n = number of elements after insertion point
 * @note Caller retains ownership of value; it is copied, not referenced
 * @note Size increases by 1
 *
 * Example:
 *   int val = 55;
 *   vector_insert(&vec, 3, &val);  // Insert element at index 3
 *   vector_insert(&vec, 0, &val);  // Insert at beginning
 */
vector_result_t vector_insert(vector_t*, const size_t, const void*);

/**
 * @brief Retrieves the current capacity of the vector
 *
 * Returns the maximum number of elements the vector can hold without resizing.
 * Useful for monitoring memory usage and planning pre-allocation strategies.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param output Pointer to size_t variable to receive capacity value (must not be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_INVALID_OUTPUT if output pointer is NULL
 *
 * @note Time complexity: O(1)
 * @note Capacity is always >= size
 * @note Capacity is always >= VECTOR_STD_CAPACITY
 *
 * Example:
 *   size_t cap = 0;
 *   vector_capacity(&vec, &cap);
 *   printf("Capacity: %zu\n", cap);
 */
vector_result_t vector_capacity(const vector_t*, size_t*);

/**
 * @brief Retrieves the current number of elements in the vector
 *
 * Returns the count of elements currently stored in the vector.
 * Valid indices range from 0 to size - 1.
 *
 * @param vector Pointer to initialized vector_t struct
 * @param output Pointer to size_t variable to receive size value (must not be NULL)
 *
 * @return VECTOR_RES_OK on success
 * @return VECTOR_RES_ERR_INVALID_VECTOR if vector is uninitialized
 * @return VECTOR_RES_ERR_INVALID_OUTPUT if output pointer is NULL
 *
 * @note Time complexity: O(1)
 * @note Size is always <= capacity
 * @note Size is 0 for newly initialized or cleared vectors
 *
 * Example:
 *   size_t len = 0;
 *   vector_size(&vec, &len);
 *   printf("Elements: %zu\n", len);
 *   for (size_t i = 0; i < len; i++) {
 *       // Access elements at indices 0 to len-1
 *   }
 */
vector_result_t vector_size(const vector_t*, size_t*);

#ifdef __cplusplus
}
#endif

#endif //VECTORLIB_VECTORLIB_H
