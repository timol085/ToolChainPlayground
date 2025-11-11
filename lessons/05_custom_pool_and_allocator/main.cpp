#include <iostream>
#include <vector>
struct Pool{
    char buf[1024*1024]; size_t offset=0;
    void* alloc(size_t n){void* p=buf+offset;offset+=n;return p;}
};
template<class T> struct PoolAlloc{
    using value_type=T; Pool* pool;
    PoolAlloc(Pool* p):pool(p){}
    T* allocate(size_t n){return (T*)pool->alloc(n*sizeof(T));}
    void deallocate(T*,size_t)noexcept{}
};
int main(){
    Pool pool;
    std::vector<int,PoolAlloc<int>> v{PoolAlloc<int>(&pool)};
    for(int i=0;i<10;i++) v.push_back(i);
    std::cout<<"used "<<pool.offset<<" bytes\n";
}
