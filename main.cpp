// Scratch space for trying out your Vector. Build and run with:
//     make main && ./main
// This file is not graded.

#include <iostream>
#include "Vector.h"

int main() {
    Vector<int> v;
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
        std::cout << "size = " << v.size() << ", capacity = " << v.capacity() << "\n";
    }
    return 0;
}
