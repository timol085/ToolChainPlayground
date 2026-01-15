# ToolChainPlayground

A hands-on C++ learning project that explores low-level toolchain concepts, memory management, and performance optimization through practical examples.

## Overview

ToolChainPlayground is a collection of focused lessons that demystify C++ compilation, memory behavior, and optimization techniques. Each lesson is self-contained with its own examples, explanations, and exercises.

## Prerequisites

- **Compiler**: Clang with C++20 support (or GCC 10+)
- **Build System**: CMake 3.20 or higher
- **Platform**: macOS, Linux, or Windows
- **Knowledge**: Basic understanding of C++ syntax and command-line tools

## Project Structure

```
ToolChainPlayground/
├── CMakeLists.txt          # Root build configuration
├── README.md               # This file
├── lessons/                # Individual lesson directories
│   ├── 01_preprocessor_to_binary/
│   ├── 02_vector_growth_and_invalidations/
│   ├── 03_map_vs_unordered_map/
│   ├── 04_asan_ubsan_memory_playground/
│   ├── 05_custom_pool_and_allocator/
│   ├── 06_odr_and_headers/
│   ├── 07_static_vs_dynamic_linking/
│   └── 08_lto_and_pgo_basics/
├── build/                  # Build output (generated)
└── cmake-build-debug/      # IDE build directory (generated)
```

## Lessons

### 01. Preprocessor to Binary
Understand the complete C++ build pipeline from source code to executable binary.

**Topics**: Preprocessing, compilation to assembly, object files, linking, symbol tables, ODR

**Key takeaways**: 
- How `#include` works at a mechanical level
- What happens at each stage of compilation
- Understanding the One Definition Rule (ODR)

---

### 02. Vector Growth and Invalidations
Deep dive into `std::vector` memory behavior and iterator invalidation.

**Topics**: Size vs capacity, reallocation, iterator invalidation, `reserve` vs `resize`, AddressSanitizer

**Key takeaways**:
- Predict when vectors reallocate
- Avoid iterator invalidation bugs
- Performance optimization with `reserve()`

---

### 03. Map vs Unordered Map
Compare `std::map` and `std::unordered_map` in terms of performance and use cases.

**Topics**: Binary search trees, hash tables, lookup complexity, cache locality

**Key takeaways**:
- When to use ordered vs unordered associative containers
- Performance trade-offs in different scenarios

---

### 04. ASan & UBSan Memory Playground
Explore memory safety tools and undefined behavior detection.

**Topics**: AddressSanitizer, UndefinedBehaviorSanitizer, common memory bugs, sanitizer options

**Key takeaways**:
- Detect memory errors at runtime
- Identify undefined behavior before it causes problems

---

### 05. Custom Pool and Allocator
Build custom memory allocators and understand allocation strategies.

**Topics**: Memory pools, custom allocators, allocation patterns, performance optimization

**Key takeaways**:
- When and how to write custom allocators
- Performance implications of different allocation strategies

---

### 06. ODR and Headers
Explore the One Definition Rule in depth with practical examples.

**Topics**: ODR violations, header organization, inline functions, templates, linkage

**Key takeaways**:
- Avoid ODR violations
- Organize headers effectively
- Understand inline and template mechanics

---

### 07. Static vs Dynamic Linking
Compare static and dynamic linking strategies and their trade-offs.

**Topics**: Static libraries, shared libraries, symbol resolution, load-time vs runtime linking

**Key takeaways**:
- Choose appropriate linking strategy
- Understand library dependencies
- Debug linking issues

---

### 08. LTO and PGO Basics
Explore Link-Time Optimization and Profile-Guided Optimization.

**Topics**: Whole program optimization, profile collection, feedback-driven optimization

**Key takeaways**:
- Leverage advanced optimization techniques
- Measure and improve performance systematically

## Getting Started

### Building All Lessons

```bash
# Clone the repository
git clone <repository-url>
cd ToolChainPlayground

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build all lessons
cmake --build .
```

### Building Individual Lessons

Each lesson can be built independently:

```bash
cd lessons/01_preprocessor_to_binary
clang++ -std=c++20 main.cpp math_utils.cpp -o lesson01
./lesson01
```

See each lesson's README for specific build instructions and examples.

## Learning Path

**Recommended order for beginners**:
1. Start with **Lesson 01** to understand the build pipeline
2. Move to **Lesson 02** for practical container knowledge
3. Try **Lesson 04** to learn debugging tools
4. Explore remaining lessons based on interest

**For experienced developers**:
- Jump to any lesson that interests you
- Each lesson is self-contained with minimal dependencies

## Tools and Commands

Common tools used throughout the lessons:

| Tool | Purpose | Example |
|------|---------|---------|
| `clang++` | Compile C++ code | `clang++ -std=c++20 main.cpp -o app` |
| `objdump` / `llvm-objdump` | Inspect object files | `llvm-objdump -t file.o` |
| `nm` | List symbols | `nm -C file.o` |
| `otool` / `ldd` | Show dynamic dependencies | `otool -L app` (macOS) |
| `size` | Display section sizes | `size app` |
| `strings` | Extract strings from binary | `strings app` |

## Sanitizers

Many lessons use sanitizers for debugging. Build with:

```bash
# AddressSanitizer (memory errors)
clang++ -std=c++20 -g -fsanitize=address main.cpp -o app_asan

# UndefinedBehaviorSanitizer
clang++ -std=c++20 -g -fsanitize=undefined main.cpp -o app_ubsan

# Both
clang++ -std=c++20 -g -fsanitize=address,undefined main.cpp -o app_sanitized
```

## Contributing

This is a personal learning project, but suggestions and improvements are welcome:

1. Fork the repository
2. Create a feature branch
3. Add or improve lessons with clear explanations
4. Submit a pull request

## Resources

- [C++ Reference](https://en.cppreference.com/)
- [Compiler Explorer](https://godbolt.org/) - See assembly output online
- [CMake Documentation](https://cmake.org/documentation/)
- [LLVM Documentation](https://llvm.org/docs/)
- [AddressSanitizer Documentation](https://clang.llvm.org/docs/AddressSanitizer.html)

## License

This project is open source and available for educational purposes.

## Acknowledgments

Created as a practical learning resource for understanding C++ toolchains, memory management, and performance optimization from first principles.
