/*
 * Written by Aaryaman Sagar (rmn100@gmail.com)
 * 
 * This header file contains a templated variant type that can be used with
 * any number of type arguments.  So for example the following are legal uses
 *      VariadicTemplatedType<int, double> temp {1};
 *      VariadicTemplatedType<int, float> temp {1.0};
 */
#include <string> 
#include <cassert>
#include <new>
#include <iostream>
#include <tuple>
#include <type_traits>
#include "template_metaprogrammed_variant_utilities.hpp"
using std::cout;
using std::endl;
using std::string;

template <typename... Vs>
class VariadicTemplatedType {
public:

    // The data that is going to be used as the buffer for this variant type,
    // i.e. any type included in the Vs... parameter pack can fit here
    static const size_t data_size = static_max<sizeof(Vs)...>::value;
    static const size_t data_align = static_max<alignof(Vs)...>::value;
    typename std::aligned_storage<data_size, data_align>::type buffer;
 
    // the index in the template pack having the current type of the element
    int current_type_index;

    // a templated constructor and a regular destructor
    template <typename Type>
    VariadicTemplatedType(const Type& element);

    ~VariadicTemplatedType();

    // gets the type in the index specified at the given index, use the public
    // 'type' typedef in the class
    template <int which_type>
    struct GetType;

    // cast and return the element from the specified type
    template <int which>
    auto get() const;

    // the apply visitor function
    template<typename Visitor>
    auto apply_visitor(Visitor visitor);
};

#include "template_metaprogrammed_variant.ipp"
