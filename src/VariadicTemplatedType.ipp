#include <cstring>
#include <string> 
#include <cassert>
#include <new>
#include <iostream>
#include <tuple>
#include <type_traits>
#include "VariadicTemplatedType.hpp"
using std::cout;
using std::endl;
using std::string;


// Uses the placement new operator which does not create an object on the heap
// but just constructs it in place in the memory address specified, so this
// has none of the overheads of regular heap allocation using new.  This
// function also uses the IndexInTemplatePack function to query which index
// the current type is at and sets the current_type_index to be that index.
template <typename... Vs>
template <typename Type>
VariadicTemplatedType<Vs...>::VariadicTemplatedType(const Type& element) {

    static_assert(IsTypeContainedIn<Type, Vs...>::value, 
            "Cannot use this variant type with a type that is not included "
            "in the parameter pack aka the template argument list");

    // construct the element in place and set the index
    new(&this->buffer) Type{element};
    this->current_type_index = IndexInTemplatePack<Type, Vs...>::value;
}

template <typename... Vs>
VariadicTemplatedType<Vs...>::VariadicTemplatedType(
        __attribute__((unused)) const VariadicTemplatedType& other) {
}

template <typename... Vs>
template <int which_type>
struct VariadicTemplatedType<Vs...>::GetType {
    
    // check out http://en.cppreference.com/w/cpp/utility/tuple/tuple_element
    // for a possible explanation of the implementation
    using type = typename std::tuple_element<which_type, std::tuple<Vs...>>::type;
};

// templated function for general purpose functions that need to check the
// type of the object and then perform things differently based on what the
// type is.  Look at the commenting in the apply function to see how this
// works.
template <int which_type>
struct apply_functor_conditionally_to_appropriate_type {

    // declare a templated destroy function that takes in a variant type, this
    // is independent of the variant type class
    template <typename Function, typename Variant, typename... Others>
    static void apply(Function function, Variant& variant, 
            Others&... others) {

        // assert that others and this are of the same type
        static_assert(AreAllSame<Variant, Others...>::value, "All the types passed "
                "in to the parameter pack when applying a functor "
                "polymorphically should be the same");

        // simulate switch(variant.current_type_index) using compile time
        // recursion, first initiate the check here and then do the check on
        // the next integer smaller than this, so if the variant type is of
        // type <int, double, string>, then this will check if the
        // current_type_index is equal to 2 corresponding to the string and
        // then check the whether the current_type_index is equal to 1
        // corresponding to an double, and then 0 which corresponds to the
        // base case and is an int. 
        //
        // Currently no static asserts are done on the type of the other
        // parameters.  TODO I will add this later.
        if (variant.current_type_index == which_type) {

            // get the type of the object stored at the moment in the variant
            using TypeStored = 
                typename Variant::template GetType<which_type>::type;

            // call the function with all the inner data of all the variants
            // passed into the function all cast to the same type
            function(variant.template cast_and_return_data<TypeStored>(),
                    others.template cast_and_return_data<TypeStored>()...);
        }

        // compile time tail recursion?  Whatever this is is pretty cool.
        // Compiler will probably inline this since the function calls are so
        // tiny when compilation is finished and the templates have been
        // unrolled.
        else {
            return apply_functor_conditionally_to_appropriate_type<which_type - 1>
                ::apply(function, variant, others...);
        }
    }
};
template <>
struct apply_functor_conditionally_to_appropriate_type<0> {

    // The function for the base case
    template <typename Function, typename Variant, typename... Others>
    static void apply(Function function, Variant& variant, 
            Others&... others) {

        // assert since there is no lower index possible
        assert(variant.current_type_index == 0);
        using TypeStored = typename Variant::template GetType<0>::type;
        function(variant.template cast_and_return_data<TypeStored>(),
                others.template cast_and_return_data<TypeStored>()...);
    }
};

template <typename... Vs>
VariadicTemplatedType<Vs...>::~VariadicTemplatedType() {

    // call the awesome apply_functor_conditionally_to_appropriate_type
    // function, this function imitates a switch case block with compile time
    // recursion so that the code looks exactly like a switch case but the
    // code writing is done at compile time.  If that does not make sense look
    // at the apply_functor_conditionally_to_appropriate_type non base case
    // class.  If that also does not make sense then try harder.
    apply_functor_conditionally_to_appropriate_type<sizeof...(Vs) - 1>::
        apply([](const auto& appropriate_type_var) {

            // get the right type from the input parameter without the const
            // and the ref
            using TypeWithConstRefRemoved = 
                typename std::remove_reference<
                    std::remove_const_t<decltype(appropriate_type_var)>>::type;

            // call the destructor
            appropriate_type_var.~TypeWithConstRefRemoved();
        }, *this);
}

template <typename... Vs>
template <typename Type>
Type& VariadicTemplatedType<Vs...>::cast_and_return_data() {
    return *reinterpret_cast<Type*>(&buffer);
}

template <typename... Vs>
template <typename Visitor>
void VariadicTemplatedType<Vs...>::apply_visitor(Visitor visitor) {

    // call a helper from the helper class
    apply_functor_conditionally_to_appropriate_type<sizeof...(Vs) - 1>
        ::apply(visitor, *this);
}

