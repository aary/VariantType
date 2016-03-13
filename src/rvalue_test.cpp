#include <utility>
#include <iostream>
using std::cout;
using std::endl;

class Something {
public:
    Something() = default;
    Something(__attribute__((unused)) const Something& other) {
        cout << "Copy constructor called" << endl;
    }
    Something(__attribute__((unused)) Something&& other) {
        cout << "Move constructor called" << endl;
    }

    void print() {
        cout << "Something::print() called" << endl;
    }
};

void function_1(Something&& one) {
    cout << "version two called" << endl;
    Something inner{std::move(one)};
    inner.print();
}
void function_1(const Something& one) {
    Something inner(one);
    inner.print();
}

template <typename... T>
void test_function(T&&... ts) {
    function_1(std::forward<T>(ts)...);
}

void function_second_level(int& input) {
    cout << "second level lvalue version called with " << input << endl;
}
void function_second_level(int&& input) {
    cout << "second level move version called with " << input << endl;
}
void function_first_level(int&& input) {
    function_second_level(std::move(input));
}

int main() {

    // const Something some1 {Something()};

    // test_function(some1);
    // test_function(Something());

    function_first_level(1);

    return 0;
}
