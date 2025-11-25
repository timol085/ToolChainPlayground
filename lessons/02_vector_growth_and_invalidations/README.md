# Lesson 2: Vector Growth and Invalidation

## Goal

- Understand size vs capacity
- Observe growth (reallocations) and element moves
- Know exact invalidation rules (and how to avoid surprises)
- Practice `reserve`, `resize`, `shrink_to_fit`, `push_back` vs `emplace_back`

## Table of Contents

- [Step 1: Instrumented Capacity Logger](#step-1-instrumented-capacity-logger-precise-growth-pattern)
- [Step 2: reserve vs resize](#step-2-reserve-vs-resize)
- [Step 3: Invalidation Demo with AddressSanitizer](#step-3-invalidation-demo-with-addresssanitizer)
- [Step 4: push_back vs emplace_back](#step-4-push_back-vs-emplace_back)
- [Step 5: shrink_to_fit](#step-5-shrink_to_fit)
- [Step 6: Reference vs Iterator vs Pointer Validity](#step-6-reference-vs-iterator-vs-pointer-validity)
- [Micro-benchmark: The reserve Effect](#micro-benchmark-the-reserve-effect-optional-but-great)
- [Quick Q&A](#quick-qa-you-can-answer-now)
- [CMake Configuration for ASan](#cmake-configuration-for-asan-nice-quality-of-life)
- [Learning Checklist](#learning-checklist)

## Step 1: Instrumented Capacity Logger (precise growth pattern)

Running the capacity logger shows how `std::vector` grows:
```
start: size=0 cap=0 prev_cap=0x0
grow @i= 0 size= 1 cap= 1 data:0x6000036ac020
grow @i= 1 size= 2 cap= 2 data:0x6000036ac030  (RELOCATED)
grow @i= 2 size= 3 cap= 4 data:0x6000036ac020  (RELOCATED)
grow @i= 4 size= 5 cap= 8 data:0x6000034a9200  (RELOCATED)
grow @i= 8 size= 9 cap=16 data:0x6000021a81c0  (RELOCATED)
grow @i=16 size=17 cap=32 data:0x600001aa8080  (RELOCATED)
grow @i=32 size=33 cap=64 data:0x6000008a8000  (RELOCATED)
final: size=64 cap=64 data=0x6000008a8000
```

### Key Observations

- **Capacity jumps** follow a pattern: `0→1→2→4→8→16→32→64`
- Some implementations (like libc++) may show small quirks at tiny sizes: `0→1→2→3→...` before settling into doubling
- **"(RELOCATED)"** indicates the buffer address changed
    - All pointers, iterators, and references to elements are now invalid
    - This is the most common source of bugs with vectors

### How Vector Growth Works

`std::vector` stores elements contiguously in memory. When size exceeds capacity:

1. Allocates a larger block (growth factor typically 1.5x–2x)
2. Moves or copies all elements to the new block
3. Frees the old block
4. Updates internal pointers

This process invalidates all pointers, iterators, and references to elements.

## Step 2: reserve vs resize

Understanding the difference between these operations is critical:

### reserve(n)

- **Purpose**: Pre-allocate capacity to prevent reallocation storms
- **Effect on size**: None (size remains unchanged)
- **Effect on capacity**: Sets capacity to at least `n`
- **Element construction**: Does not construct any elements
- **Use case**: When you know how many elements you'll add
```cpp
std::vector<int> v;
v.reserve(100);  // capacity is now 100, size is still 0
// Now you can push_back 100 times without any reallocations
```

### resize(n)

- **Purpose**: Change the actual number of elements
- **Effect on size**: Sets size to exactly `n`
- **Effect on capacity**: May increase capacity if needed
- **Element construction**: Creates or destroys elements to reach size `n`
- **Use case**: When you want a specific number of elements
```cpp
std::vector<int> v;
v.resize(5);     // size is now 5, elements are default-constructed (0 for int)
// v = {0, 0, 0, 0, 0}
```

### Comparison Table

| Operation | Changes Size | Changes Capacity | Constructs Elements |
|-----------|--------------|------------------|---------------------|
| `reserve(n)` | No | Yes (if needed) | No |
| `resize(n)` | Yes | Yes (if needed) | Yes |

## Step 3: Invalidation Demo with AddressSanitizer

AddressSanitizer (ASan) is a runtime memory error detector that catches bugs like use-after-free and buffer overflows.

### Building with AddressSanitizer
```bash
cd lessons/02_vector_growth_and_invalidations
clang++ -std=c++20 -O0 -g -fsanitize=address,undefined main.cpp -o lesson02_asan
./lesson02_asan
```

### Sample Output
```
lesson02_asan(90401,0x1f7704840) malloc: nano zone abandoned due to inability to reserve vm space.
start: size=0 cap=0 prev_cap=0x0
grow @i= 0 size= 1 cap= 1 data:0x6020000000b0
grow @i= 1 size= 2 cap= 2 data:0x6020000000d0  (RELOCATED)
grow @i= 2 size= 3 cap= 4 data:0x6020000000f0  (RELOCATED)
grow @i= 4 size= 5 cap= 8 data:0x603000001bd0  (RELOCATED)
grow @i= 8 size= 9 cap=16 data:0x6060000002c0  (RELOCATED)
grow @i=16 size=17 cap=32 data:0x60c0000001c0  (RELOCATED)
grow @i=32 size=33 cap=64 data:0x611000000040  (RELOCATED)
reserve only: size=0 cap=100
resize: size=5 cap=5
before insert: *p=3
=================================================================
==90401==ERROR: AddressSanitizer: heap-use-after-free on address 0x603000001c3c
READ of size 4 at 0x603000001c3c thread T0
    #0 0x0001003f1f00 in main main.cpp:55
```

### What This Tells Us

The ASan error shows:

1. **The bug**: Reading from freed memory (heap-use-after-free)
2. **Where**: Line 55 in main.cpp
3. **Why**: The memory was freed during a vector reallocation
4. **Stack trace**: Shows the exact sequence of calls that led to the error

This demonstrates how keeping a pointer to a vector element across operations that may reallocate is dangerous and leads to undefined behavior.

### The Bug Pattern
```cpp
std::vector<int> v = {1, 2, 3};
int* p = &v[2];              // Pointer to third element
v.insert(v.begin(), 0);      // May reallocate
std::cout << *p;             // BOOM - use-after-free
```

## Step 4: push_back vs emplace_back

Both add elements to the end of the vector, but they differ in how they construct the element.

### Understanding the Difference
```cpp
struct Big {
    Big(int v) : v(v) {
        std::cout << "Big(" << v << ") constructed\n";
    }
    Big(const Big& other) : v(other.v) {
        std::cout << "Big copied\n";
    }
    Big(Big&& other) noexcept : v(other.v) {
        std::cout << "Big moved\n";
    }
    int v;
};

std::vector<Big> bv;
bv.reserve(4);

// push_back: constructs temporary, then moves into vector
bv.push_back(Big{1});     // Output: Big(1) constructed
                          //         Big moved

// emplace_back: constructs directly in place
bv.emplace_back(2);       // Output: Big(2) constructed
                          //         (no move!)

std::cout << "push vs emplace done, size=" << bv.size() << "\n";
```

### Key Points

- **`push_back(value)`**: Takes an existing object (or temporary), then moves/copies it into the vector
- **`emplace_back(args...)`**: Constructs the element directly in place using the provided arguments
- **Performance benefit**: `emplace_back` avoids creating a temporary object when constructing from arguments
- **Important limitation**: `emplace_back` does NOT prevent invalidation during reallocation

### When to Use Each

| Use `push_back` when: | Use `emplace_back` when: |
|----------------------|--------------------------|
| You already have an object to insert | You want to construct from arguments |
| Readability is more important | You need to avoid temporary construction |
| Example: `v.push_back(existing_obj)` | Example: `v.emplace_back(arg1, arg2)` |

### Common Misconception

`emplace_back` is often thought to be "magic" that prevents all invalidation. This is false. Reallocation still moves all elements, and invalidation rules still apply.
```cpp
std::vector<Big> v;
Big* p = &v.emplace_back(1);  // OK, v.capacity() is now 1
v.emplace_back(2);             // Reallocation! p is now invalid
```

## Step 5: shrink_to_fit

`shrink_to_fit()` is a non-binding request to reduce capacity to fit the current size.

### Example
```cpp
std::vector<int> s;
for (int i = 0; i < 1000; ++i) {
    s.push_back(i);
}
std::cout << "before shrink: size=" << s.size() 
          << " cap=" << s.capacity() << "\n";
// Output: before shrink: size=1000 cap=1024 (or similar)

s.resize(10);
std::cout << "after  resize: size=" << s.size() 
          << " cap=" << s.capacity() << "\n";
// Output: after  resize: size=10 cap=1024
// Capacity hasn't changed!

s.shrink_to_fit();
std::cout << "after shrink: size=" << s.size() 
          << " cap=" << s.capacity() << "\n";
// Output: after shrink: size=10 cap=10 (typically)
```

### Important Characteristics

- **Non-binding**: The standard allows implementations to ignore the request
- **Typical behavior**: Most implementations do reduce capacity to size
- **Use case**: Reclaim memory after removing many elements
- **Performance**: May trigger reallocation and element moves
- **Invalidation**: If reallocation occurs, all iterators/pointers/references are invalidated

### When to Use

Use `shrink_to_fit()` when:
- You've removed many elements and want to free memory
- Memory usage is more important than potential reallocation cost
- You're done modifying the vector for a while

Don't use it:
- In performance-critical loops
- If you'll add elements again soon
- If the memory savings are negligible

### Soundbite

"shrink_to_fit() is a non-binding request. It often reduces capacity to size, but the standard allows the library to ignore it."

## Step 6: Reference vs Iterator vs Pointer Validity

Understanding invalidation rules is crucial for writing correct vector code.

### Complete Invalidation Rules

#### 1. Reallocation (any operation that grows past capacity)

**Invalidates**: All pointers, references, and iterators

**Operations that may cause reallocation**:
- `push_back()` when size equals capacity
- `emplace_back()` when size equals capacity
- `insert()` when resulting size exceeds capacity
- `resize(n)` when `n > capacity`
- `reserve(n)` when `n > capacity`
```cpp
std::vector<int> v = {1, 2, 3};
int* p = &v[0];
auto it = v.begin();
int& ref = v[1];

v.push_back(4);  // May reallocate
// p, it, and ref are ALL invalid now
```

#### 2. Insert/Erase Without Reallocation

**At the end** (`push_back` within capacity):
- All pointers, references, and iterators remain valid
- `end()` iterator is invalidated
```cpp
std::vector<int> v;
v.reserve(10);
v.push_back(1);
int* p = &v[0];
v.push_back(2);  // No reallocation, p is still valid
```

**Middle insert/erase**:
- Invalidates iterators and references at or after the modification point
- Elements shift in memory to fill gaps or make space
```cpp
std::vector<int> v = {1, 2, 3, 4, 5};
v.reserve(10);
int* p1 = &v[1];  // Points to 2
int* p4 = &v[4];  // Points to 5

v.erase(v.begin() + 2);  // Removes 3
// p1 is still valid (before modification point)
// p4 is INVALID (at or after modification point)
```

#### 3. Other Operations

**`clear()`**:
- Invalidates all iterators and references
- Does not change capacity
- All elements are destroyed

**`reserve(n)`**:
- Does not invalidate unless it causes reallocation
- If `n <= capacity`, no effect
- If `n > capacity`, reallocates (invalidates everything)

**`resize(more)`**:
- May reallocate if new size exceeds capacity
- If reallocates, invalidates everything
- If doesn't reallocate, only `end()` iterator changes

**`resize(less)`**:
- Does not reallocate
- Destroys tail elements
- References to destroyed elements are invalid
- Iterators to remaining elements stay valid

**`shrink_to_fit()`**:
- Usually reallocates (implementation-dependent)
- If reallocates, invalidates everything

### Quick Reference Table

| Operation | Reallocation | Invalidates |
|-----------|-------------|-------------|
| `push_back()` (within capacity) | No | Only `end()` |
| `push_back()` (exceeds capacity) | Yes | Everything |
| `insert()` middle (within capacity) | No | At and after insertion point |
| `insert()` (exceeds capacity) | Yes | Everything |
| `erase()` | No | At and after erasure point |
| `clear()` | No | Everything (elements destroyed) |
| `reserve(n)` where `n > capacity` | Yes | Everything |
| `reserve(n)` where `n <= capacity` | No | Nothing |
| `resize(n)` where `n > capacity` | Yes | Everything |
| `shrink_to_fit()` | Usually | Everything (if reallocation occurs) |

### How to Avoid Pain

#### Use Indices Instead of Pointers
```cpp
// BAD: Pointer invalidation
std::vector<int> v = {1, 2, 3};
int* p = &v[1];
v.push_back(4);  // May invalidate p
std::cout << *p; // Undefined behavior

// GOOD: Use index
std::vector<int> v = {1, 2, 3};
size_t idx = 1;
v.push_back(4);
std::cout << v[idx];  // Always safe
```

#### Call reserve Before Bulk Inserts
```cpp
// BAD: Multiple reallocations
std::vector<int> v;
for (int i = 0; i < 1000; ++i) {
    v.push_back(i);  // May reallocate multiple times
}

// GOOD: Single allocation
std::vector<int> v;
v.reserve(1000);     // Allocate once
for (int i = 0; i < 1000; ++i) {
    v.push_back(i);  // No reallocation
}
```

#### Consider Alternative Containers for Stable References

If you need references that remain valid across insertions:

- **`std::deque`**: Fewer invalidations than vector, but not contiguous
    - Insertions at front/back don't invalidate references to other elements
    - Middle insertions still invalidate

- **Node-based containers** (`std::list`, `std::map`, `std::set`):
    - References remain valid except when that specific element is erased
    - Trade-off: No contiguous memory, worse cache locality
```cpp
// When you need stable references:
std::deque<int> d;
d.push_back(1);
int& ref = d[0];
d.push_back(2);
// ref is still valid (vector would have invalidated it)
```

### Real-World Bug Example
```cpp
void process_items(std::vector<Item>& items) {
    for (auto& item : items) {
        if (item.needs_expansion()) {
            // BUG: This may reallocate and invalidate 'item' reference
            items.push_back(item.expand());
        }
    }
}

// FIX: Use index-based loop
void process_items(std::vector<Item>& items) {
    size_t original_size = items.size();
    for (size_t i = 0; i < original_size; ++i) {
        if (items[i].needs_expansion()) {
            items.push_back(items[i].expand());  // Safe: re-fetch items[i]
        }
    }
}
```

## Micro-benchmark: The reserve Effect (optional but great)

Content coming soon. This section will demonstrate the performance impact of using `reserve()` before bulk insertions, comparing multiple reallocations vs. a single pre-allocation.

## Quick Q&A You Can Answer Now

Content coming soon. This section will include common questions and answers about vector behavior, such as:
- Why does capacity sometimes exceed what I reserved?
- Can I force a vector to not reallocate?
- What's the performance difference between growth factors?
- When should I use vector vs deque vs list?

## CMake Configuration for ASan (nice quality-of-life)

Content coming soon. This section will show how to set up a CMake target with AddressSanitizer enabled for easy testing and debugging of memory issues.

## Learning Checklist

Ensure you understand each concept:

- [ ] I can explain the difference between size and capacity
- [ ] I can predict when a vector will reallocate based on its growth pattern
- [ ] I can identify when pointers/iterators/references become invalid
- [ ] I understand when to use `reserve()` vs `resize()`
- [ ] I know when `emplace_back()` is beneficial vs `push_back()`
- [ ] I can explain why `shrink_to_fit()` is non-binding
- [ ] I can rewrite pointer-based code to use indices to avoid invalidation bugs
- [ ] I can run code with AddressSanitizer and interpret the output
- [ ] I can name alternative containers when stable references are needed

## Building and Running
```bash
# Basic build
cd lessons/02_vector_growth_and_invalidations
clang++ -std=c++20 main.cpp -o lesson02

# With AddressSanitizer (recommended)
clang++ -std=c++20 -O0 -g -fsanitize=address,undefined main.cpp -o lesson02_asan

# Run
./lesson02
# or
./lesson02_asan
```

## Further Reading

- [C++ Reference - std::vector](https://en.cppreference.com/w/cpp/container/vector)
- [Iterator Invalidation Rules](https://en.cppreference.com/w/cpp/container#Iterator_invalidation)
- [AddressSanitizer Documentation](https://clang.llvm.org/docs/AddressSanitizer.html)
- [Understanding Vector Growth Strategies](https://stackoverflow.com/questions/5232198/about-vectors-growth)