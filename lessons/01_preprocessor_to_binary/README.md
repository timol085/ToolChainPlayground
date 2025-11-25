# C++ Build Pipeline: From Source to Binary

A step-by-step guide to understanding the C++ compilation process, from preprocessor directives to executable binaries.

## Table of Contents

- [Overview](#overview)
- [Step 1: Preprocessor](#step-1-preprocessor-expand-includes-and-macros-i)
- [Step 2: Compilation to Assembly](#step-2-compilation-to-assembly-s)
- [Step 3: Assemble to Object File](#step-3-assemble-to-object-file-o)
- [Step 4: Link to Runnable Binary](#step-4-link-to-runnable-binary)
- [Step 5: Verify Sizes and Strings](#step-5-verify-sizes-and-strings-sanity-checks)
- [Step 6: Full Pipeline in One Command](#step-6-see-the-whole-pipeline-in-one-go-quality-of-life)
- [Step 7: Headers and the ODR](#step-7-connect-dots-to-headers-and-the-odr)
- [Key Concepts](#key-concepts)
- [Learning Checklist](#learning-checklist)

## Overview

This guide walks through each stage of the C++ build process using Clang on macOS (ARM64), showing how source code transforms into an executable binary.

## Step 1: Preprocessor (expand includes and macros → .i)

The preprocessor copies the contents of headers into the source and expands macros. This is pure text transformation—the compiler has not parsed C++ yet.
```bash
cd lessons/01_preprocessor_to_binary
clang++ -std=c++20 -E main.cpp -o main.i
```

**What happens:**
- The compiler preprocesses the C++ code and creates `main.i`
- This file is huge because `<iostream>` pulls in a lot of code
- Your own code is found at the bottom
- Included `.hpp` files are now inlined header text

**Key points:**
- `#include` is a copy-and-paste of the header content during preprocessing
- `#pragma once` (or include guards) prevents duplicate inclusion within the same translation unit

## Step 2: Compilation to Assembly (→ .s)

The compiler parses the preprocessed C++ code and emits human-readable assembly.
```bash
clang++ -std=c++20 -x c++-cpp-output -S main.i -o main.s
```

**What happens:**
- Creates `main.s` file with ARM64 assembly instructions
- Includes instructions for:
    - Construction of iostream sentry
    - Writing to `std::cout`
    - Returning from functions

## Step 3: Assemble to Object File (→ .o)

The assembler converts assembly to machine code with sections (text, data, cstring, etc.) and a symbol table. This is not a runnable program yet.
```bash
clang++ -std=c++20 -c main.s -o main.o
```

**Inspect the object file:**
```bash
# View symbol table
xcrun llvm-objdump --demangle -t main.o | less

# View sections
xcrun llvm-objdump -section-headers main.o

# Alternative symbol inspection
nm -m main.o
nm -gU --demangle main.o
```

**What to look for:**
- `*UND*` symbols (undefined) that must be resolved at link time:
    - `std::__1::cout`
    - iostream helper functions
    - `___gxx_personality_v0` (C++ exception handling)
    - `_strlen`

## Step 4: Link to Runnable Binary

The linker resolves all `*UND*` symbols against libraries and glues objects into an executable.
```bash
clang++ main.o math_utils.cpp -o main
```

**Inspect linked libraries:**
```bash
otool -L main
```

**Run the program:**
```bash
./main
```

## Step 5: Verify Sizes and Strings (sanity checks)

Check segment sizes and embedded text in the binary.
```bash
# Show segment sizes (text/data/bss)
size main

# Peek at embedded text
strings main | head
```

## Step 6: See the Whole Pipeline in One Go (quality-of-life)

Keep all intermediate files automatically:
```bash
clang++ -std=c++20 -O2 -save-temps=obj main.cpp math_utils.cpp -o app
```

This emits:
- `main.o`, `math_utils.o`
- `main.i`, `main.s` (next to their objects)

**Show underlying subcommands:**
```bash
clang++ -std=c++20 -O2 -save-temps=obj main.cpp math_utils.cpp -o app -###
```

## Step 7: Connect Dots to Headers and the ODR

### Why headers contain templates and inline functions

- **Templates** must be visible to all TUs (Translation Units) using them
    - Instantiation happens where used
- **Inline functions** avoid multiple-definition errors across TUs

### Headers vs source files

- `math_utils.hpp` has `constexpr` and templates (header-only)
- `math_utils.cpp` can host non-template, non-inline function definitions
    - Centralizes codegen
    - Improves build times
    - Provides ABI stability

### ODR (One Definition Rule)

If you define the same non-inline function in two TUs, you get a linker error.

**Try it:**
Add a non-inline free function to both `main.cpp` and `math_utils.cpp` and watch the linker complain.

## Key Concepts

### Macro vs constexpr

| Macro | constexpr |
|-------|-----------|
| Text replacement during preprocessing | Type-safe compile-time constant |
| No type checking | Full type checking |
| `#define PI 3.14` | `constexpr double PI = 3.14;` |

### Header guards vs pragma once

**Header guards:**
```cpp
#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP
// header content
#endif
```

**pragma once:**
```cpp
#pragma once
// header content
```

Both prevent duplicate inclusion, but `#pragma once` is simpler and widely supported.

## Learning Checklist

Ensure you've learned each step:

- [ ] I can generate `main.i`, open it, and point to where my header got inlined
- [ ] I can generate `main.s`, find `_main`, and roughly explain what the instructions are doing
- [ ] I can show `objdump -t main.o` and identify `_main` vs `*UND*` symbols
- [ ] I can link and use `otool -L main` to name the libraries
- [ ] I can use `-save-temps=obj` and `-###` to see the full build pipeline

## Directory Structure
```
lessons/01_preprocessor_to_binary/
├── main.cpp
├── math_utils.hpp
├── math_utils.cpp
└── README.md
```

## Requirements

- Clang with C++20 support
- macOS (for ARM64 examples and `otool`)
- Basic understanding of C++ syntax

## Further Reading

- [C++ Reference - Translation phases](https://en.cppreference.com/w/cpp/language/translation_phases)
- [Understanding the C++ compilation process](https://www.learncpp.com/cpp-tutorial/introduction-to-the-compiler-linker-and-libraries/)
- [One Definition Rule (ODR)](https://en.cppreference.com/w/cpp/language/definition)