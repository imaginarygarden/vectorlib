# API Reference

## Overview

VectorLib provides a generic container for storing elements of arbitrary type using raw memory. It supports automatic growth, optional shrinking, and robust error handling.

---

## Data Structure

### `vector_t`

```c
typedef struct vector {
    void *data;
    size_t data_size;
    size_t capacity;
    size_t size;
} vector_t;
```

---

## Lifecycle

### `vector_init`

Initializes the vector.

* Fails if `data_size == 0`
* Allocates initial capacity (`VECTOR_STD_CAPACITY`)
* Returns `vector_result_t` status code

---

### `vector_free`

Releases all allocated memory and resets the structure.

* Safe to call on partially initialized vectors
* Returns `vector_result_t` status code

---

## Element Operations

### `vector_push_back`

* Appends an element to the end
* Automatically resizes if necessary
* Returns `vector_result_t` status code

---

### `vector_pop_back`

* Removes the last element
* Writes removed value to output if provided
* Shrinks capacity if size < capacity / `VECTOR_SHRINK_LIMIT`
* Returns `vector_result_t` status code

---

### `vector_get`

* Retrieves a copy of element at index
* Bounds-checked
* Returns `vector_result_t` status code

---

### `vector_set`

* Overwrites element at index
* Bounds-checked
* Returns `vector_result_t` status code

---

### `vector_insert`

* Inserts an element at specified index
* Shifts subsequent elements right
* Resizes if necessary
* Returns `vector_result_t` status code

---

### `vector_remove`

* Removes a contiguous range of elements starting at index
* Shifts subsequent elements left
* Returns `vector_result_t` status code

---

### `vector_clear`

* Removes all elements
* Retains allocated memory for future insertions
* Returns `vector_result_t` status code

---

### `vector_reserve`

* Increases capacity to at least the requested value
* Useful to preallocate space
* Returns `vector_result_t` status code

---

### `vector_shrink`

* Reduces capacity to a specified value (>= size and >= `VECTOR_STD_CAPACITY`)
* Returns `vector_result_t` status code

---

## Introspection

### `vector_size`

Returns number of stored elements.

* Output via pointer
* Returns `vector_result_t` status code

---

### `vector_capacity`

Returns allocated capacity.

* Output via pointer
* Returns `vector_result_t` status code

---

## Complexity

| Operation      | Complexity     |
| -------------- | -------------- |
| push_back      | Amortized O(1) |
| pop_back       | O(1)           |
| get/set        | O(1)           |
| insert/remove  | O(n)           |
| clear          | O(1)           |
| reserve/shrink | O(n)           |

---

## Error Handling

All functions return `vector_result_t` codes:

* `VECTOR_RES_OK` – success
* `VECTOR_RES_ERR_ALLOC` – memory allocation failed
* `VECTOR_RES_ERR_OVERFLOW` – integer overflow in capacity calculation
* `VECTOR_RES_ERR_BOUNDS` – index out of bounds
* `VECTOR_RES_ERR_EMPTY` – operation on empty vector
* `VECTOR_RES_ERR_INVALID_VECTOR` – null/uninitialized vector
* `VECTOR_RES_ERR_INVALID_SIZE` – invalid size parameter
* `VECTOR_RES_ERR_INVALID_INPUT` – null input pointer
* `VECTOR_RES_ERR_INVALID_OUTPUT` – null output pointer

Always check the return code before assuming success.

---

## Important Notes

* No automatic destruction of stored elements
* Caller must manage ownership of stored data
* `void*` usage implies no type safety
* Thread safety: **not provided**, synchronization must be handled externally
