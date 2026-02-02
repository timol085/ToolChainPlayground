#include <bitset>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace bits {

// ---- Part 1: single-bit helpers ----
constexpr uint32_t mask_u32(unsigned n) {
    // NOTE: shifting by >= 32 is UB; guard.
    return (n < 32) ? (1u << n) : 0u;
}

constexpr bool test(uint32_t x, unsigned n) {
    return (x & mask_u32(n)) != 0;
}

constexpr void set(uint32_t& x, unsigned n) {
    x |= mask_u32(n);
}

constexpr void clear(uint32_t& x, unsigned n) {
    x &= ~mask_u32(n);
}

constexpr void toggle(uint32_t& x, unsigned n) {
    x ^= mask_u32(n);
}

// ---- Part 2: bit-range helpers (packed fields) ----
// Create a mask covering [offset, offset+width)
constexpr uint32_t range_mask(unsigned offset, unsigned width) {
    // width==0 => mask 0
    // guard to avoid UB for shifts of 32
    if (width == 0 || offset >= 32) return 0;
    if (width >= 32 - offset) {
        // mask from offset to end
        return 0xFFFF'FFFFu << offset;
    }
    const uint32_t m = ((1u << width) - 1u) << offset;
    return m;
}

constexpr uint32_t get_field(uint32_t x, unsigned offset, unsigned width) {
    const uint32_t m = range_mask(offset, width);
    return (m == 0) ? 0 : ((x & m) >> offset);
}

constexpr void set_field(uint32_t& x, unsigned offset, unsigned width, uint32_t value) {
    const uint32_t m = range_mask(offset, width);
    if (m == 0) return;

    // clamp value to fit width
    const uint32_t maxv = (width >= 32) ? 0xFFFF'FFFFu : ((1u << width) - 1u);
    value &= maxv;

    x = (x & ~m) | ((value << offset) & m);
}

} // namespace bits

// ---- Part 3: type-safe flags ----
enum class RenderFlags : uint32_t {
    None        = 0,
    Visible     = 1u << 0,
    Selected    = 1u << 1,
    CastShadow  = 1u << 2,
    ReceivesAO  = 1u << 3,
};

constexpr RenderFlags operator|(RenderFlags a, RenderFlags b) {
    return static_cast<RenderFlags>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}
constexpr RenderFlags operator&(RenderFlags a, RenderFlags b) {
    return static_cast<RenderFlags>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}
constexpr RenderFlags operator~(RenderFlags a) {
    return static_cast<RenderFlags>(~static_cast<uint32_t>(a));
}
constexpr bool has(RenderFlags value, RenderFlags flag) {
    return (value & flag) != RenderFlags::None;
}

int main() {
    std::cout << "== Bit masking playground ==\n\n";

    // Part 1: bit ops
    uint32_t x = 0;
    bits::set(x, 3);
    bits::set(x, 7);
    bits::toggle(x, 3); // clears it
    std::cout << "x = " << x << "  (bin " << std::bitset<16>(x) << ")\n";
    std::cout << "bit7? " << bits::test(x, 7) << "\n\n";

    // Part 2: packed fields in a single uint32_t
    // Layout:
    // bits 0..3   : mode   (4 bits)
    // bits 4..11  : id     (8 bits)
    // bit  12     : enabled (1 bit)
    uint32_t packed = 0;
    bits::set_field(packed, 0, 4, 9);     // mode = 9
    bits::set_field(packed, 4, 8, 200);   // id = 200
    bits::set_field(packed, 12, 1, 1);    // enabled = 1

    std::cout << "packed = " << packed << " (bin " << std::bitset<16>(packed) << ")\n";
    std::cout << "mode    = " << bits::get_field(packed, 0, 4) << "\n";
    std::cout << "id      = " << bits::get_field(packed, 4, 8) << "\n";
    std::cout << "enabled = " << bits::get_field(packed, 12, 1) << "\n\n";

    // Part 3: enum class flags
    RenderFlags flags = RenderFlags::Visible | RenderFlags::CastShadow;
    std::cout << "has Visible?    " << has(flags, RenderFlags::Visible) << "\n";
    std::cout << "has Selected?   " << has(flags, RenderFlags::Selected) << "\n";
    std::cout << "has CastShadow? " << has(flags, RenderFlags::CastShadow) << "\n\n";

    // Pitfall demo: precedence
    // NOTE: (x & 1u << n) is parsed as x & (1u << n) ? Actually << has higher precedence than &
    // But many people *think* it's the other way. Still: always parenthesize for readability.
    std::cout << "Pitfall reminder: always write (x & (1u << n))\n";

    // Pitfall demo: signedness
    int32_t s = -1;
    uint32_t u = static_cast<uint32_t>(s);
    std::cout << "signed -1 as uint32: " << u << " (bin " << std::bitset<16>(u) << ")\n";

    std::cout << "\nDone.\n";
    return 0;
}
