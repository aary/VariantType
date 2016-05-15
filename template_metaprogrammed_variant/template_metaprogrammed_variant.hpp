/*
 * Written by Aaryaman Sagar (rmn100@gmail.com)
 * 
 * This header file contains a templated variant type that can be used with
 * any number of type arguments.  So for example the following are legal uses
 *      VariadicTemplatedType<int, double> temp {1};
 *      VariadicTemplatedType<int, float> temp {1.0};
 */

#pragma once

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

    /*
     * Templated contructor.  There is no default constructor for this type
     * because it does not make any sense to use this type without an element
     * to assign this to.  Use of a type not included in the parameter pack
     * will cause a static_assert to fail.
     */
    template <typename Type>
    VariadicTemplatedType(const Type& element);

    /*
     * Destructor.  This calls the destructor for the appropriate type in the
     * class.  This uses the friend class
     * apply_functor_conditionally_to_appropriate_type to accomplish the
     * switching
     */
    ~VariadicTemplatedType();

    /*
     * Copy constructor.  Does not work if the other is of a different type.
     * This checking happens by default by the compiler -> win win
     */
    VariadicTemplatedType(const VariadicTemplatedType& other);

    /*
     * Move constructor.  This moves the storage from the previous one.
     * Calling the move constructor for the appropriate type if one exists
     */
    VariadicTemplatedType(VariadicTemplatedType&& other);

    /*
     * Use this function to apply a functor to the type in the class.  Any
     * operation in the functor must be applicable to any type in the variant.
     * i.e. if the functor outputs data from the variant to cout, then each
     * type in the template pack must have overloaded the operator<< to work
     * with cout. 
     */
    template<typename Visitor>
    void apply_visitor(Visitor visitor);

public:
    /*
     * The data that is going to be used as the buffer for this variant type,
     * i.e. any type included in the Vs... parameter pack can fit here.  This
     * will be the same size as a union that would contain variables of all
     * the types listed in the parameter pack.  There is no need for an
     * explicit union here in this case, but we still have to deal with the
     * downsides of POD types in an object oriented language.  placement new
     * goes a long way in mitigating the damages
     */
    constexpr static const size_t data_size = StaticMax<sizeof(Vs)...>::value;
    typename std::aligned_union<data_size, Vs...>::type buffer;
 
    /* 
     * the index in the template pack having the current type of the element
     */
#ifdef DEBUG
public:
    int current_type_index;
private:
#else
    int current_type_index;
#endif

    /* 
     * gets the type in the index specified at the given index, use the public
     * 'type' typedef in the class
     */
    template <int which_type>
    struct GetType;

    // casts the class's inner data to the type specified and returns it
    template <typename Type>
    Type& cast_and_return_data();

    // make friends with the class with all the helper functions that have not
    // been declared public
    template <int which_type>
    friend struct apply_functor_conditionally_to_appropriate_type;
};

#include "template_metaprogrammed_variant.ipp"
