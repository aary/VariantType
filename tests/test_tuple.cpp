#include <tuple>
using std::tuple;
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

class Something {
public:
    Something() : a{1} {
        cout << "Default constructed" << endl;
    }

    Something(const Something& other) {
        this->a = other.a;
        cout << "Copy constructed" << endl;
    }

    Something(Something&& other) {
        this->a = other.a;
        cout << "Move constructed" << endl;
    }

    int a;
};

template <typename... Vs>
auto variadic_function(const Vs&... vs) {
    std::tuple<const Vs&...> tup {std::forward_as_tuple(vs...)};
    return std::get<0>(tup);
}

int main() {
    Something s1, s2, s3;
    Something s4 {variadic_function(s1, s2, s3)};
    cout << "Just to get rid of warning " << s4.a << endl;

    try {
        throw 1;
    } catch (std::string& str) {
    } catch (...) {
        cerr << "No exception thrown" << endl;
    }

    return 0;
}
