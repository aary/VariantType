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

void test_not_equals_operator() {

    VariantType obj1 {true};
    VariantType obj2 {false};

    VariantType obj3 {"aary"};
    VariantType obj4 {"aary"};
    assert_variant(obj3 == obj4);
    assert_variant(obj3 == "aary");
    assert_variant(obj1 == true);
}

void test_less_than_operator() {
    VariantType obj1 {1};
    VariantType obj2 {2};
    assert_variant(obj1 < obj2);

    obj1 = std::move(VariantType{2});
    assert_variant(!(obj1 < obj2));
}

void test_greater_than_operator() {
    VariantType obj1 {1};
    VariantType obj2 {2};
    assert_variant(obj2 > obj1);

    obj1 = std::move(VariantType{2});
    assert_variant(!(obj1 > obj2));
}

void test_equals_operator() {
    VariantType obj1 {1};
    VariantType obj2 {1};
    assert_variant(obj1 == obj2);

    VariantType other_type{"what?"};
    try {
        assert_variant(obj1 == other_type);
    } catch(std::exception& exc) {
    } catch(...) {
        throw std::logic_error("Comparing objects of different types did not "
                "fail");
    }
}

void run_tests() {
    test_not_equals_operator();
    test_less_than_operator();
    test_greater_than_operator();
    test_equals_operator();
}
