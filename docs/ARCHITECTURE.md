# Architecture

## Overview

VectorLib is a minimal abstraction over a contiguous heap-allocated buffer.

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

---

## Resizing Strategy

### Growth

Triggered when:

```
size >= capacity
```

New capacity:

```
capacity *= VECTOR_GROW_CAPACITY
```

---

### Shrink

Triggered when:

```
size < capacity / VECTOR_SHRINK_CAPACITY
```

New capacity:

```
capacity /= VECTOR_SHRINK_CAPACITY
```

---

## Allocation Strategy

* `calloc` → initialization
* `realloc` → resizing
* `free` → cleanup

---

## Debug Layer

Enabled via:

```
DEBUG_MODE
```

Provides runtime logging for:

* allocations
* resizing
* errors
