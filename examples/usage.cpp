#include <algorithm>
#include <cassert>
#include <iostream>
#include "Vector.h"

int main() {
    Vector<int> values{4, 1, 3};
    // The argument aliases the full buffer that push_back will replace.
    values.push_back(values[0]);
    values.insert(1, 2);
    std::sort(values.begin(), values.end());
    assert((values == Vector<int>{1, 2, 3, 4, 4}));

    const Vector<int>& read_only = values;
    std::cout << "sorted:";
    for (int value : read_only) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';

    Vector<int> copied = values;
    copied[0] = 99;
    assert(values[0] == 1 && copied[0] == 99);
    std::cout << "original first: " << values[0]
              << ", copy first: " << copied[0] << '\n';

    values.erase(1);
    values.shrink_to_fit();
    assert((values == Vector<int>{1, 3, 4, 4}));
    assert(values.size() == 4 && values.capacity() == 4);
    std::cout << "after erase:";
    for (int value : values) {
        std::cout << ' ' << value;
    }
    std::cout << "\nsize: " << values.size()
              << ", capacity: " << values.capacity() << '\n';
}
