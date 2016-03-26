#include <string> 
#include <cassert>
#include <new>
#include <iostream>
#include <tuple>
#include <type_traits>
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
template <int which_type>
struct VariadicTemplatedType<Vs...>::GetType {
    
    // check out http://en.cppreference.com/w/cpp/utility/tuple/tuple_element
    // for a possible explanation of the implementation
    using type = typename std::tuple_element<which_type, std::tuple<Vs...>>::type;
};


// templated destructor impl struct that helps in doing compile time recursion
// based on the input type, check out the comment in the destroy function
// below
template <int which_type>
struct apply_functor_conditionally_to_appropriate_type {

    // declare a templated destroy function that takes in a variant type, this
    // is independent of the variant type class
    template <typename VariantType, typename Function>
    static void apply(const VariantType& variant, Function function) {

        // simulate switch(variant.current_type_index) using compile time
        // recursion, first initiate the check here and then do the check on
        // the next integer smaller than this, so if the variant type is of
        // type <int, double, string>, then this will check if the
        // current_type_index is equal to 2 corresponding to the string and
        // then check the whether the current_type_index is equal to 1
        // corresponding to an double, and then 0 which corresponds to the
        // base case and is an int
        if (variant.current_type_index == which_type) {
            using TypeStored = 
                typename VariantType::template GetType<which_type>::type;
            function(*reinterpret_cast<const TypeStored*>(&variant.buffer));
        }
        else {
            return apply_functor_conditionally_to_appropriate_type<which_type - 1>
                ::apply(variant, function);
        }
    }
};
template <>
struct apply_functor_conditionally_to_appropriate_type<0> {

    // The ufnction for the base case
    template <typename VariantType, typename Function>
    static void apply(const VariantType& variant, Function function) {

        // assert since there is no lower index possible
        assert(variant.current_type_index == 0);
        using TypeStored = typename VariantType::template GetType<0>::type;
        function(*reinterpret_cast<const TypeStored*>(&variant.buffer));
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
        apply(*this, [](const auto& appropriate_type_var) {

            // get the right type from the input parameter without the const
            // and the ref
            using TypeWithConstRefRemoved = 
                typename std::remove_reference<
                    std::remove_const_t<decltype(appropriate_type_var)>>::type;

            // call the destructor
            appropriate_type_var.~TypeWithConstRefRemoved();
        });

    // destructor_impl<sizeof...(Vs) - 1>::destroy(*this);
}

template <typename... Vs>
template <int which_type>
auto VariadicTemplatedType<Vs...>::get() const {

    // get the type stored in the object using the GetType function
    using TypeStored = 
        typename VariadicTemplatedType::template GetType<which_type>::type;
    return *reinterpret_cast<const TypeStored*>(&buffer);
}


template <typename... Vs>
template <typename Visitor>
auto VariadicTemplatedType<Vs...>::apply_visitor(Visitor visitor) {
    return apply_functor_conditionally_to_appropriate_type<sizeof...(Vs) - 1>
        ::apply(*this, visitor);
}


