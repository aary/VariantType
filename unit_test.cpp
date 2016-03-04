#include "VariantType.hpp"
#include "VariantTypeImpl.hpp"
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <string>
using std::string;
#include <sstream>
#include <vector>
using std::vector;
#include <cassert>
#include <stdexcept>
using std::exception;
using std::logic_error;
#include <unordered_map>
using std::unordered_map;
#include <map>
using std::map;
#include <set>
using std::set;
#include <functional>

/*
 * An assert for this library that throws exceptions
 */
#ifdef NDEBUG
#define ASSERT_VARIANT(condition)
#else
#define ASSERT_VARIANT(condition) \
    assert_variant_private(__LINE__, condition, #condition, __FILE__, \
            __FUNCTION__);
#endif


// A string stream for in test debugging output.  Define the macro
// TEST_DEBUG_OUTPUT to print all data sent to this to standard out as well
std::ostringstream oss;
#ifdef TEST_DEBUG_OUTPUT
std::ostream& test_output_stream = cout;
#else 
std::ostream& test_output_stream = oss;
#endif


/*
 * Private function to the library, this should not be used directly
 */
static void assert_variant_private(int line_number, 
        bool condition_to_assert, const char* condition, 
        const char* filename, const char* function) {

    // If not condition then display condition and line number and throw
    // exception
    if (!condition_to_assert) {
        throw std::logic_error(
                string("ASSERTION FAILED on condition [") + 
                string(condition) + string("] on line : [") + 
                string(filename) + string(":") + 
                std::to_string(line_number) + string("] in function : [") +
                string(function) + string("()]\n"));
    }
}

static void test_not_equals_operator() {

    VariantType obj1 {true};
    VariantType obj2 {false};

    VariantType obj3 {"aary"};
    VariantType obj4 {"aary"};
    ASSERT_VARIANT(obj3 == obj4);
    ASSERT_VARIANT(obj3 == "aary");
    ASSERT_VARIANT(obj1 == true);
}

static void test_less_than_operator() {
    VariantType obj1 {1};
    VariantType obj2 {2};
    ASSERT_VARIANT(obj1 < obj2);

    obj1 = std::move(VariantType{2});
    ASSERT_VARIANT(!(obj1 < obj2));
}

static void test_greater_than_operator() {
    VariantType obj1 {1};
    VariantType obj2 {2};
    ASSERT_VARIANT(obj2 > obj1);

    obj1 = std::move(VariantType{2});
    ASSERT_VARIANT(!(obj1 > obj2));
}

static void test_equals_operator() {
    VariantType obj1 {1};
    VariantType obj2 {1};
    ASSERT_VARIANT(obj1 == obj2);

    VariantType other_type{"what?"};
    try {
        ASSERT_VARIANT(obj1 == other_type);
    } catch(std::exception& exc) {
    } catch(...) {
        throw std::logic_error("Comparing objects of different types did not "
                "fail");
    }
}

static void test_inclusion_maps() {

    // make a std::set with VariantType objects
    std::set<VariantType> set_objects;
    set_objects.emplace("dog");
    set_objects.emplace("cat");
    for (const auto& obj : set_objects) {
        test_output_stream << obj << endl;
    }

    // make a std::map with VariantType objects
    std::map<VariantType, string> map_objects;
    map_objects["aary"] = "my favorite";
    map_objects["aary"] = "what";
    for (const auto& pair_objects : map_objects) {
        test_output_stream << pair_objects.first << " : " 
            << pair_objects.second << endl;
    }

    std::unordered_map<VariantType, string> hash_table_objects;
    hash_table_objects["dogs"] = "my favorites";
    for (const auto& pair_objects : hash_table_objects) {
        test_output_stream << pair_objects.first << " : " 
            << pair_objects.second << endl;
    }
}

auto run_tests() {

    cout << "Running tests..." << endl;

    // the vector of tests
    vector<std::function<void ()>> test_functions {
        &test_not_equals_operator, 
        &test_less_than_operator,
        &test_greater_than_operator,
        &test_equals_operator,
        &test_inclusion_maps
    };

    // run all the tests
    for (decltype(test_functions.size()) i = 0; i < test_functions.size(); ++i) {

        test_functions[i]();
        auto percent_done = static_cast<double>(i) / 
            static_cast<double>(test_functions.size());
        cout << percent_done * 100 << "% tests completed" << endl;
    }

    cout << "All tests passed" << endl;
}
