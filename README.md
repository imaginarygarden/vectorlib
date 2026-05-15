# VectorLib

A lightweight, generic dynamic array (vector) implementation in C.

VectorLib provides a type-agnostic, resizable array for storing arbitrary data types in contiguous memory. It supports automatic growth and shrink, robust error handling, and a simple, predictable API.

---

## Features

* Generic storage using `void*` and `data_size`
* Automatic resizing:

    * Growth: doubles capacity when full
    * Shrink: halves capacity when size < 1/4 of capacity
* Comprehensive API:

    * `push_back`, `pop_back`, `insert`, `remove`, `get`, `set`
    * `clear`, `reserve`, `shrink`, `size`, `capacity`
* Minimal and predictable API
* Error handling with detailed return codes (`vector_result_t`)
* CMake integration (library + FetchContent ready)
* Unit tests (Unity)
* Benchmark support
* Safe memory management strategies with minimum capacity enforcement

---

## Thread Safety

This library is **not thread-safe**. If used in multithreaded contexts, synchronization must be handled externally.

---

## Installation

### Using CMake (recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
    vectorlib
    GIT_REPOSITORY https://github.com/imaginarygarden/vectorlib.git
    GIT_TAG master
)

FetchContent_MakeAvailable(vectorlib)

target_link_libraries(your_target PRIVATE vectorlib)
```

---

### Manual build

```bash
cmake -S . -B build
cmake --build build
```

---

## Example Usage

```c
#include <stdio.h>
#include "vectorlib.h"

int main() {
    vector_t vec;

    if (vector_init(&vec, sizeof(int)) != VECTOR_RES_OK)
        return 1;

    // Append elements
    for (int i = 0; i < 5; i++)
        vector_push_back(&vec, &i);

    // Insert an element at index 2
    int val = 42;
    vector_insert(&vec, 2, &val);

    // Remove 2 elements starting at index 1
    vector_remove(&vec, 1, 2);

    // Access elements
    size_t len = 0;
    vector_size(&vec, &len);
    for (size_t i = 0; i < len; i++) {
        int value;
        vector_get(&vec, i, &value);
        printf("%d\n", value);
    }

    vector_free(&vec);
    return 0;
}
```

---

## Error Handling

All operations return a `vector_result_t` status code:

| Code                            | Meaning                                  |
| ------------------------------- | ---------------------------------------- |
| `VECTOR_RES_OK`                 | Success                                  |
| `VECTOR_RES_ERR_ALLOC`          | Memory allocation failed                 |
| `VECTOR_RES_ERR_OVERFLOW`       | Integer overflow in capacity calculation |
| `VECTOR_RES_ERR_BOUNDS`         | Index out of bounds                      |
| `VECTOR_RES_ERR_EMPTY`          | Operation requires non-empty vector      |
| `VECTOR_RES_ERR_INVALID_VECTOR` | Null or uninitialized vector             |
| `VECTOR_RES_ERR_INVALID_SIZE`   | Invalid size argument                    |
| `VECTOR_RES_ERR_INVALID_INPUT`  | Null input pointer                       |
| `VECTOR_RES_ERR_INVALID_OUTPUT` | Null output pointer                      |

Always check return codes before assuming an operation succeeded.

---

## Testing

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

---

## Benchmark

```bash
cmake -S . -B build -DVECTORLIB_BUILD_BENCHMARK=ON
cmake --build build
./build/benchmark
```

---

## Configuration

| Macro                  | Description                          | Default |
| ---------------------- | ------------------------------------ | ------- |
| `VECTOR_STD_CAPACITY`  | Initial allocation (elements)        | 5       |
| `VECTOR_GROW_FACTOR`   | Growth multiplier when full          | 2       |
| `VECTOR_SHRINK_FACTOR` | Shrink multiplier when reducing      | 2       |
| `VECTOR_SHRINK_LIMIT`  | Shrink threshold (size < capacity/x) | 4       |

---

## Design Notes

* Elements stored in contiguous memory
* No type safety (generic `void*` design)
* No automatic destruction of elements; manual memory management required
* Amortized O(1) insertion/removal at end
* O(n) complexity for operations involving shifting (`insert`, `remove`, `shrink`)
