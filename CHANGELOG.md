# Changelog

## [0.2.0] - Expanded API & Robust Error Handling

### Added

* New API functions:

    * `vector_insert` – insert an element at a specific index
    * `vector_remove` – remove a range of elements
    * `vector_clear` – remove all elements while keeping allocated capacity
    * `vector_reserve` – pre-allocate capacity
    * `vector_shrink` – reduce capacity manually
* Comprehensive return codes (`vector_result_t`) for all operations
* Minimum capacity enforcement and safer memory management
* Improved shrinking logic: automatic capacity reduction when size < 1/4 of capacity
* Updated documentation and README examples for full API coverage
* Example usage demonstrating `insert`, `remove`, `clear`, `reserve`, `shrink`

### Changed

* Expanded `vectorlib.h` API to cover more use cases beyond basic push/pop/get/set
* README updated with detailed error handling, thread-safety notes, and configuration macros
* Amortized O(1) guarantees clarified for push/pop operations
* Added safety macros for overflow detection and capacity clamping

### Notes

* No automatic destruction of stored elements (manual memory management required)
* Thread safety: still **not thread-safe**, synchronization must be handled externally

## [0.1.1] - Submodule integration

### Added

* Testing library as submodule
* Automatic test compilation disabled
* Submodule initialization with CMakeLists

## [0.1.0] - Initial Release

### Added

* Generic dynamic array (`vector_t`)
* Core API:

    * `vector_init`
    * `vector_free`
    * `vector_push_back`
    * `vector_pop_back`
    * `vector_get`
    * `vector_set`
    * `vector_size`
    * `vector_capacity`
* Automatic resizing (growth and shrink)
* Debug logging via `DEBUG_MODE`
* Unity-based unit tests
* Benchmark executable
* CMake build system
* FetchContent compatibility
* CI pipeline (GitHub Actions with sanitizers)

### Notes

* Initial stable baseline
* Focus on simplicity and portability
