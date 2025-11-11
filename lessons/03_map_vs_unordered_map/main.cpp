#include <map>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>
int main(){
    constexpr int N=100000;
    std::vector<int> v(N);
    for(int i=0;i<N;i++) v[i]=i;
    std::map<int,int> m;
    std::unordered_map<int,int> um;
    auto t0=std::chrono::high_resolution_clock::now();
    for(int i:v) m[i]=i;
    auto t1=std::chrono::high_resolution_clock::now();
    for(int i:v) um[i]=i;
    auto t2=std::chrono::high_resolution_clock::now();
    std::cout<<"map insert "<<std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count()
             <<"ms umap insert "<<std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()<<"ms\n";
}
