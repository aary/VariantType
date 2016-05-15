#include <string> 
#include <cassert>
#include <new>
#include <iostream>
#include <tuple>
#include <type_traits>
#include "template_metaprogrammed_variant.hpp"
using std::cout;
using std::endl;
using std::string;

class Something {
public:

    Something() {
        cout << "Something constructed" << endl;
    }

    Something(__attribute__((unused)) const Something& other) {
        cout << "Something copy constructed" << endl;
    }

    ~Something() {
        cout << "Something destroyed" << endl;
    }
};

class SomethingElse {
public:

    template <typename Other>
    SomethingElse (__attribute__((unused)) const Other& other) {
        cout << "SomethingElse copy constructed" << endl;
    }
};

int main() {

    VariadicTemplatedType<int, string> temp {string{"hello"}};
    temp.apply_visitor([](const auto& var) { cout << var << endl; });
    VariadicTemplatedType<int, double> temp1 {1};
    cout << "size of VariadicTemplatedType<int, double> is " << sizeof(temp1)
        << endl;

    return 0;
}

