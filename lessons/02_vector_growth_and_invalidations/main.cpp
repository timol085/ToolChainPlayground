#include <vector>
#include <iostream>
#include <iomanip>

int main(){
    std::vector<int> v;
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

    std::cout << "final: size=" << v.size()
              << " cap=" << v.capacity()
              << " data=" << v.data() << "\n";
}
