#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <list>
#include <cassert>

#include "list_intermediate.hpp"
#include "list_pro.hpp"

template<class List>
void test_list(size_t size) {
    List lst; //make empty list
    using It = decltype(lst.begin());
    It n; //make "iterator" variable

    //fill the half of it with emplace_back
    for (size_t s = 0; s < size/2; s++) {
        n = lst.emplace_back(s);
    }
    //fill the other half of it with emplace_after
    for (size_t s = size/2; s < size; s++) {
        n = lst.emplace_after(n, s);
    }
    assert(lst.size() == size); //assert that all values were added

    //assert that all values were added correctly
    size_t s = 0;
    for (It it = lst.begin(); it != lst.end(); it++) {
        size_t val = *it;
        assert(val == s);
        s++;
    }

    List lst2(lst);
    assert(std::equal(lst2.begin(), lst2.end(), lst.begin()));
    std::list<int> l;

    //erase half if it via "erase"
    n = lst.begin();
    for (size_t s = 0; s < size/2; s++) {
        n = lst.erase(n);
    }
    //assert that half of it were erased
    assert(lst.size() == size - size/2);
    lst.clear(); //clear the rest of it
    //assert that the list is empty
    assert(lst.size() == 0);
    assert(lst.begin() == lst.end());
}

int main() {
    const size_t size = 10000;
    test_list<intermediate::list<size_t>>(size);
    test_list<pro::list<size_t>>(size);
    return 0;
}