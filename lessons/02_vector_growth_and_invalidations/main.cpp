#include <vector>
#include <iostream>
#include <iomanip>

int main(){
    std::vector<int> v;
    // v.reserve(64);
    std::size_t prev_cap = v.capacity();
    const void* prev_data = nullptr;

    std::cout << "start: size=" << v.size()
              << " cap=" << v.capacity()
              << " prev_cap=" << v.data() << "\n";
    for (int i =0; i < 64; ++i) {
        v.push_back(i);

        if (v.capacity() != prev_cap) {
            std::cout << "grow @i=" << std::setw(2) << i
                      << " size=" << std::setw(2) << v.size()
                      << " cap=" << std::setw(2) << v.capacity()
                      << " data:" << v.data();

            if (prev_data && prev_data != static_cast<const void *>(v.data()))
                std::cout << "  (RELOCATED)";
            std::cout << "\n";

            prev_cap  = v.capacity();
            prev_data = static_cast<const void *>(v.data());
        }
    }

    // --- reserve: only affects capacity, not size ---
    std::vector<int> a;
    a.reserve(100);
    std::cout << "reserve only: size=" << a.size() << " cap=" << a.capacity() << "\n";

    // --- resize: changes size; adds default-initialized elements if growing ---
    std::vector<int> b;
    b.resize(5); // size becomes 5 (elements default-constructed)
    std::cout << "resize: size=" << b.size() << " cap=" << b.capacity() << "\n";

    // --- invalidation demo ---
    std::vector<int> w;
    w.reserve(8);                 // keep capacity stable initially
    for (int i = 0; i < 8; ++i) w.push_back(i);

    int* p = &w[3];               // pointer into vector
    std::cout << "before insert: *p=" << *p << "\n";

    // This may shift elements; if reallocation happens (e.g., capacity full),
    // ALL pointers/iterators/references become invalid.
    w.insert(w.begin() + 2, 999);

    // UB: p may now dangle or point to moved data
    std::cout << "after insert : *p=" << *p << "   <-- UB if shifted/reallocated\n";

    // --- Step 4: push_back vs emplace_back ---
    struct Big {
        Big(int v) : v(v) {}
        Big(const Big& other) : v(other.v) { std::cout << "copy\n"; }
        Big(Big&& other) noexcept : v(other.v) { std::cout << "move\n"; }
        int v;
    };

    std::vector<Big> bv;
    bv.reserve(4);
    bv.push_back(Big{1});     // temp Big{1} then move into vector
    bv.emplace_back(2);       // construct directly in place
    std::cout << "push vs emplace done, size=" << bv.size() << "\n";

    // --- Step 5: shrink_to_fit ---
    std::vector<int> s;
    for (int i=0;i<1000;++i) s.push_back(i);
    std::cout << "before shrink: size="<<s.size()<<" cap="<<s.capacity()<<"\n";
    s.resize(10);
    std::cout << "after  resize: size="<<s.size()<<" cap="<<s.capacity()<<"\n";
    s.shrink_to_fit(); // non-binding; usually reduces capacity to >= size
    std::cout << "after shrink: size="<<s.size()<<" cap="<<s.capacity()<<"\n";

    std::cout << "final: size=" << v.size()
              << " cap=" << v.capacity()
              << " data=" << v.data() << "\n";
}
