# Contributing

## Guidelines

* Keep the library minimal and focused
* Avoid unnecessary abstractions
* Prefer clarity over cleverness

---

## Build

```bash
cmake -S . -B build
cmake --build build
```

---

## Testing

```bash
ctest --test-dir build
```

---

## Code Style

* Follow `.clang-format`
* Use `snake_case`
* Keep functions small and focused
* Document public APIs

---

## Adding Features

When adding functionality:

* Maintain backward compatibility
* Add unit tests
* Update documentation

---

## Reporting Issues

Include:

* Minimal reproducible example
* Expected vs actual behavior
* Environment details

---

## Future Improvements

* Iterator support
* Custom allocator support
* Optional destructors
* Thread-safe variant
* Reserve/shrink APIs
