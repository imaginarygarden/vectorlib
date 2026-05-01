# API Reference

## Overview

VectorLib provides a generic container for storing elements of arbitrary type using raw memory.

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

---

### `vector_free`

Releases all allocated memory and resets the structure.

---

## Element Operations

### `vector_push_back`

* Appends an element
* Resizes if necessary

---

### `vector_pop_back`

* Removes last element
* Writes removed value to output

---

### `vector_get`

* Bounds-checked read

---

### `vector_set`

* Bounds-checked write

---

## Introspection

### `vector_size`

Returns number of stored elements.

---

### `vector_capacity`

Returns allocated capacity.

---

## Complexity

| Operation | Complexity     |
| --------- | -------------- |
| push_back | Amortized O(1) |
| pop_back  | O(1)           |
| get/set   | O(1)           |

---

## Error Handling

All functions return:

* `VECTOR_RES_OK`
* `VECTOR_RES_ERR`

---

## Important Notes

* No automatic memory management for stored elements
* Caller must manage ownership of stored data
* `void*` usage implies no type safety
