#pragma once

#include <string> 
#include <cassert>
#include <new>
#include <iostream>
#include <tuple>
#include <type_traits>
using std::cout;
using std::endl;
using std::string;

/* ******** Utilities ********* */
// static_max function taken without any thought shamelessly from 
//      https://gist.github.com/tibordp/6909880#file-variant-cc-L106
template <size_t arg1, size_t ... others>
struct static_max;
template <size_t arg>
struct static_max<arg> {
    static const size_t value = arg;
};
template <size_t arg1, size_t arg2, size_t ... others>
struct static_max<arg1, arg2, others...> {
    static const size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value :
    static_max<arg2, others...>::value;
};

// taken from somewhere on stackoverflow
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

// to check whether a type is included in a template parameter pack
template <typename... Vs>
struct IsTypeContainedIn { };
template <typename ToCheck, typename Head, typename... Vs>
struct IsTypeContainedIn<ToCheck, Head, Vs...> : 
    public std::integral_constant<bool, false || 
        IsTypeContainedIn<ToCheck, Vs...>::value>
{};
template <typename ToCheck, typename... Vs>
struct IsTypeContainedIn<ToCheck, ToCheck, Vs...> : 
    public std::integral_constant<bool, true>
{};

// get the type for the specified index in the parameter pack
template <int current_index, typename... Vs>
struct TypeForIndex {};
template <typename Head, typename... Tail>
struct TypeForIndex<0, Head, Tail...> {
  using type = Head;
};
template <int current_index, typename Head, typename... Tail>
struct TypeForIndex<current_index, Head, Tail...>:
  TypeForIndex<current_index-1, Tail...>
{};

/* ******** Utilities ********* */
