# 09. Bit Masking & Packed Flags

This lesson teaches bit masking from first principles and builds toward practical patterns:
- flags (multiple booleans in one integer)
- packed fields (small integers stored in selected bit ranges)
- type-safe flag APIs with `enum class`
- common pitfalls (signedness, precedence, undefined behavior in shifting)

## Why this matters
Bit operations show up everywhere in systems code:
- protocol headers and file formats
- CPU/GPU feature flags
- packed state in game engines / sims
- performance: fewer bytes, fewer cache misses, faster comparisons

## Concepts

### 1) Bits and masks
A *mask* is a value with specific bits set that selects (or modifies) parts of another value.

Common operations:
- **Test a bit**: `x & mask`
- **Set a bit**: `x |= mask`
- **Clear a bit**: `x &= ~mask`
- **Toggle a bit**: `x ^= mask`

### 2) Shift operators
- `1u << n` creates a mask with bit `n` set.
- Shifting by a value >= bit-width is **undefined behavior** in C++.
- Left shift on signed types is a common footgun → prefer unsigned.

### 3) Packed flags
Store multiple independent booleans in one integer:
- great for cache locality and fast comparisons
- but readability suffers → wrap in helpers

### 4) Packed fields (bit ranges)
Store small integers in selected bits, e.g.:
- bits [0..3] = mode (0-15)
- bits [4..7] = quality (0-15)
- bit 8 = enabled

Pattern:
- clear range
- OR in (value shifted into position)

### 5) Type-safe flags with enum class
Prefer `enum class` and helper operators over raw integers to avoid mixing unrelated flags.

## Build & Run

From the lesson folder:

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./lesson09
