#include <iostream>
int main(){
    int* p=new int[2];
    p[2]=42;      // OOB
    delete[] p;
    std::cout<<"Done\n";
}
