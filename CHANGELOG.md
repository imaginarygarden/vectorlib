# Changelog

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
