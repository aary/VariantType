#include "Utilities.hpp"
#include <iostream>
using std::cout;
using std::endl;

int main() {

    static_assert(AreAllSame<int, int, int>::value, "");
    static_assert(!AreAllSame<int, double>::value, "");
    static_assert(AreAllSame<>::value, "");
    static_assert(!AreAllSame<double, int, int>::value, "");
    static_assert(!AreAllSame<int, double, int>::value, "");
    static_assert(!AreAllSame<int, int, double>::value, ""); 
    static_assert(!AreAllSame<int, double, double>::value, "");
    static_assert(!AreAllSame<double, int, double>::value, "");
    static_assert(!AreAllSame<double, double, int>::value, "");
    static_assert(AreAllSame<double, double, double>::value, "");

    return 0;
}
