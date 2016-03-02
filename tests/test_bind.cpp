#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <functional>

struct Something {
    int element;
};

template <typename Type>
void function(const Type& element) {
    cout << "In one element version : " << element << endl;
}
template <typename Type>
void function(const Type& element_one, const Type& element_two) {
    cout << "In two element version : [" << element_one << ", " 
        << element_two << "]" << endl << endl;
}

template <typename... Vs>
void preprocess_variadically(const Vs&... vs) {
    static_assert(sizeof...(Vs) < 3, "Cannot pass more than 2 elements to "
            "preprocess_variadically");

    cout << "Doing some preprocessing with " << sizeof...(Vs) 
        << " elements..." << endl;
    cout << "Now calling function with data members" << endl;
    function(vs.element...);
}

int main() {

    // int a = 1;
    // auto lambda = [&a] (auto parameter) {
    //     cout << a << endl;
    //     cout << parameter << endl;
    // };

    // auto bound_function_one = std::bind(lambda, "Hello");
    // auto bound_function_two = std::bind(lambda, string("World"));

    // bound_function_one();
    // bound_function_two();

    Something s1 {1};
    Something s2 {2};
    preprocess_variadically(s1, s2);
    preprocess_variadically(s1);

    return 0;
}
