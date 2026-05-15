# Architecture

## Overview

VectorLib is a minimal abstraction over a contiguous heap-allocated buffer, providing type-agnostic dynamic arrays with controlled resizing.

---

## Memory Layout

```
vector_t
 ├── data (heap buffer)
 ├── data_size
 ├── capacity
 └── size
```

Elements are stored sequentially:

```
[data][data][data]...
```

---

## Addressing

```c
(char*)vector->data + index * data_size
```

Used internally for indexing without type knowledge.

---

## Resizing Strategy

### Growth

Triggered when:

```
size >= capacity
```

New capacity:

```
capacity *= VECTOR_GROW_FACTOR
```

---

### Shrink

Triggered when:

```
size < capacity / VECTOR_SHRINK_LIMIT
```

New capacity:

```
capacity /= VECTOR_SHRINK_FACTOR
```

Capacity is always clamped to at least `VECTOR_STD_CAPACITY`.

---

## Allocation Strategy

* `calloc` → initial memory allocation
* `realloc` → resizing (growth/shrink)
* `free` → memory cleanup

---

## Debug Layer

Enabled via:

```
DEBUG_MODE
```

Provides runtime logging for:

* Allocations
* Resizing
* Errors
