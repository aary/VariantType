#include "VariantType.hpp"
#include "VariantTypeImpl.hpp"
#include <new>
#include <string>
#include <cassert>
#include <functional>
#include <iostream>
#include <tuple>
#include <stdexcept>
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::tuple;

/*
 * UTILITY METHODS
 */
void VariantType::Impl::check_same_type(const VariantType::Impl& other, 
        const std::string& error_message) const {
    if (this->tag != other.tag) {
        throw std::logic_error(error_message);
    }
}

void VariantType::Impl::check_if_type_is(VariantType::Impl::Tag tag, 
        const std::string& error_message) const {
    if (this->tag != tag) {
        throw std::logic_error(error_message);
    }
}

// This right here is so cool when used with lambdas.  The following is a
// compile time polymorphic function that can take in different inputs.  How
// can this magic ever work you ask?  The answer is - Polymorphic lambdas and
// variadic templates!  This has to be the coolest compile time function I
// have written. 
//
// This accepts a templated function object to which it can pass either one
// inner element or two inner elements based on the number of elements passed
// to it. 
//
// The return type of the function below is the same as the return type of the
// function object function that is passed in. 
template <typename Func, typename... Vs>
static auto switch_return_type_with(Func function, const Vs&... vs) {

    // This function can only accept either two or one argument, first I will
    // static_assert and then I will make a tuple with the unpacked
    // parameters, because the parameters need to be switched on. 
    static_assert(sizeof...(Vs) == 1 || sizeof...(Vs) == 2, "Cannot pass in "
            "any number of arguments other than 1 or 2 to "
            "switch_return_type_with");
    std::tuple<const Vs&...> tuple_of_values {std::forward_as_tuple(vs...)};

    // The compile time magic below
    switch (std::get<0>(tuple_of_values).tag) {

        case (VariantType::Impl::Tag::STRING): {
            return function(vs.inner_string...);
            break;
        }

        case (VariantType::Impl::Tag::INTEGER): {
            return function(vs.inner_integer...);
            break;
        }

        case (VariantType::Impl::Tag::DOUBLE): {
            return function(vs.inner_double...);
            break;
        }

        default: {
            return function(vs.inner_boolean...);
            break;
        }
    }
}

VariantType::Impl::Impl(const char* input_string) : 
    tag{VariantType::Impl::Tag::STRING} {
    new(&this->inner_string) string(input_string);
}

VariantType::Impl::Impl(const string& input_string) : 
    tag{VariantType::Impl::Tag::STRING} {
    new(&this->inner_string) string(input_string);
}

VariantType::Impl::Impl(string&& input_string) : 
    tag{VariantType::Impl::Tag::STRING} {
    new(&this->inner_string) string(input_string);
}

VariantType::Impl::Impl(double input_double) : 
    tag{VariantType::Impl::Tag::DOUBLE} {
    this->inner_double = input_double;
}

VariantType::Impl::Impl(int input_integer) : 
    tag{VariantType::Impl::Tag::INTEGER} {
    this->inner_integer = input_integer;
}

VariantType::Impl::Impl(bool input_boolean) : 
    tag{VariantType::Impl::Tag::BOOLEAN} {
    this->inner_boolean = input_boolean;
}


VariantType::Impl::Impl(VariantType::Impl&& other) : 
    tag{other.tag} {

    switch (this->tag) {
        case (VariantType::Impl::Tag::STRING):
            new(&this->inner_string) string(std::move(other.inner_string));
            break;
        case (VariantType::Impl::Tag::INTEGER):
            this->inner_integer = other.inner_integer;
            break;
        case (VariantType::Impl::Tag::DOUBLE):
            this->inner_double = other.inner_double;
            break;
        default:
            this->inner_boolean = other.inner_boolean;
            break;
    }
}

VariantType::Impl::~Impl() {

    // call string destructor if this is a string
    if (this->tag == VariantType::Impl::Tag::STRING) {
        this->inner_string.~string();
    }
}

bool VariantType::Impl::operator<(const VariantType::Impl& other) const {
    this->check_same_type(other, "Cannot compare with element of another type");

    return switch_return_type_with([](const auto& one, auto& two) {
        return one < two;
    }, *this, other);
}

bool VariantType::Impl::operator>(const VariantType::Impl& other) const {
    this->check_same_type(other, "Cannot compare with element of another type");

    return switch_return_type_with([](const auto& one, auto& two) {
        return one > two;
    }, *this, other);
}

bool VariantType::Impl::operator==(const VariantType::Impl& other) const {
    this->check_same_type(other, "Cannot compare with element of another type");

    return switch_return_type_with([](const auto& one, auto& two) {
        return one == two;
    }, *this, other);
}

bool VariantType::Impl::operator!=(const VariantType::Impl& other) const {
    this->check_same_type(other, "Cannot compare with element of another type");

    return switch_return_type_with([](const auto& one, auto& two) {
        return one != two;
    }, *this, other);
}

VariantType::Impl::operator const std::string&() const {
    this->check_if_type_is(VariantType::Impl::Tag::STRING, 
            "Cannot cast string object to another type");
    return this->inner_string;
}

VariantType::Impl::operator double() const {
    this->check_if_type_is(VariantType::Impl::Tag::DOUBLE, 
            "Cannot cast double object to another type");
    return this->inner_double;
}

VariantType::Impl::operator int() const {
    this->check_if_type_is(VariantType::Impl::Tag::INTEGER, 
            "Cannot cast integer object to another type");
    return this->inner_integer;
}

VariantType::Impl::operator bool() const {
    this->check_if_type_is(VariantType::Impl::Tag::BOOLEAN, 
            "Cannot cast boolean object to another type");
    return this->inner_boolean;
}

std::ostream& operator<<(std::ostream& os, const VariantType::Impl& impl) {
    switch_return_type_with([&os](const auto& inner_element) {
        os << inner_element;
    }, impl);

    return os;
}

// The specialized hash function for the Impl class
size_t std::hash<VariantType::Impl>::operator()(
        const VariantType::Impl& impl) const {

    return switch_return_type_with([](const auto& element) {

        // what?? the following is to remove all const and reference qualifiers
        // from the type of the object `element`
        using const_reference_removed_type = 
            typename std::remove_const<
                typename std::remove_reference<decltype(element)>::type>::type;

        return std::hash<const_reference_removed_type>()(element);
    }, impl);
}
