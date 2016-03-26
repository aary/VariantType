#include <tuple>
#include <type_traits>
#include <iostream>
using std::cout;
using std::endl;

template <typename... Vs>
struct IndexInTemplatePack;

// found it
template <typename T, typename... R>
struct IndexInTemplatePack<T, T, R...> 
    : std::integral_constant<size_t, 0>
{};

// still looking
template <typename T, typename F, typename... R>
struct IndexInTemplatePack<T, F, R...> 
    : std::integral_constant<size_t, 1 + IndexInTemplatePack<T,R...>::value>
{};

int main() {

    cout << IndexInTemplatePack<int, int, float, int>::value << endl;
    cout << IndexInTemplatePack<
        typename std::tuple_element<1, std::tuple<int, double>>::type, 
                 int,
                 double>::value << endl;

    return 0;
}
