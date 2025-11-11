#include <vector>
#include <iostream>
int main(){
    std::vector<int> v;
    std::cout << "cap=" << v.capacity() << "\n";
    const int* base=nullptr;
    for(int i=0;i<30;++i){
        v.push_back(i);
        if(base!=&v[0]){
            std::cout<<"realloc @"<<i<<" old="<<(const void*)base<<" new="<<(const void*)&v[0]<<"\n";
            base=&v[0];
        }
    }
}
