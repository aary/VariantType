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
void run_tests();
void test_not_equals_operator();
void test_less_than_operator();
void test_greater_than_operator();
void test_equals_operator();
void test_copy_constructor();

int main() {
    VariantType::Impl obj1{"eecs281"};
    VariantType::Impl obj2{"eecs482"};

    set<VariantType::Impl> set_of_impls;
    set_of_impls.insert(std::move(obj1));
    set_of_impls.insert(std::move(obj2));

    for (const auto& ele : set_of_impls) {
        cout << ele << endl;
    }

    VariantType obj3{"eecs281"};
    VariantType obj4{"eecs482"};

    set<VariantType> set_of_variants;
    set_of_variants.insert(std::move(obj3));
    set_of_variants.insert(std::move(obj4));

    for (const auto& ele : set_of_variants) {
        cout << ele << endl;
    }

    try {
        run_tests();
    } catch(std::exception& exc) {
        cerr << exc.what();
        return 1;
    }

    return 0;
}

void test_not_equals_operator() {

    VariantType obj1 {true};
    VariantType obj2 {false};

    VariantType obj3 {"aary"};
    VariantType obj4 {"aary"};
    assert_variant(obj3 == obj4);
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
