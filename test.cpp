#include <set>
using std::set;
#include "VariantType.hpp"
#include "VariantTypeImpl.hpp"
#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <cassert>
#include <stdexcept>
using std::exception;
using std::logic_error;

template <typename ELEMENT>
void print_element(ELEMENT ele) {
    cout << ele << endl;
}

// Unit tests
extern void run_tests();
extern void test_not_equals_operator();
extern void test_less_than_operator();
extern void test_greater_than_operator();
extern void test_equals_operator();
extern void test_copy_constructor();

int main() {

    try {
        run_tests();
    } catch(std::exception& exc) {
        cerr << exc.what();
        return 1;
    }

    return 0;
}
