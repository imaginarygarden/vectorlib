# VectorLib

A lightweight, generic dynamic array (vector) implementation in C.

VectorLib provides a simple and efficient way to store dynamically sized collections of arbitrary data types using contiguous memory.

---

## Features

* Generic storage via `void*`
* Automatic resizing (growth and shrink)
* Minimal and predictable API
* Debug logging support
* CMake integration (library + FetchContent ready)
* Unit tests (Unity)
* Benchmark support

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

## Example

```c
#include <stdio.h>
#include "vectorlib.h"

int main() {
    vector_t vec;

    if (vector_init(&vec, sizeof(int)) != VECTOR_RES_OK)
        return 1;

    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    for (size_t i = 0; i < vector_size(&vec); i++) {
        int value;
        vector_get(&vec, i, &value);
        printf("%d\n", value);
    }

    vector_free(&vec);
    return 0;
}
```

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

| Macro                    | Description      | Default |
| ------------------------ | ---------------- | ------- |
| `VECTOR_STD_CAPACITY`    | Initial capacity | 5       |
| `VECTOR_GROW_FACTOR`   | Growth factor    | 2       |
| `VECTOR_SHRINK_FACTOR` | Shrink factor    | 2       |

---

## Design Notes

* Elements are stored in contiguous memory
* No type safety (by design)
* No automatic destruction of stored elements
* Manual lifecycle management required
