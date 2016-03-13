#include <iostream>
using std::cout;
using std::endl;
#include <functional>
#include <utility>
using std::forward;

class TestClass {
public:
    TestClass(const TestClass& other) {
        this->integer = other.integer;
        cout << "Copy constructed" << endl;
    }
    TestClass() : integer(0) {
        cout << "Default constructed" << endl;
    }
    TestClass(TestClass&& other) {
        cout << "Move constructed" << endl;
        this->integer = other.integer;
    }

    int integer;
};

template <typename FunctionType, typename ...Args>
void my_function(FunctionType function, Args&&... args) {
    cout << "in function" << endl;
    auto bound_function = std::bind(function, args...);
    bound_function();
}

int main() {

    auto my_lambda = [](const auto& one, const auto& two) {
        cout << one.integer << two.integer << endl;
    };

    TestClass test1;
    TestClass test2;
    my_function(my_lambda, std::ref(test1), std::ref(test2));
    // auto bound_function = std::bind(my_lambda, std::ref(test1), std::ref(test2));
    // bound_function();

    return 0;
}
