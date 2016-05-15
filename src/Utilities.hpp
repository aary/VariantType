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

/*
 * This utility is used to get the max of lots of integral constants at
 * compile time.  This is easier to explain with an example so this class can
 * be instantiated like the follows
 *      
 *      constexpr static const int max_size = StaticMax<int, double>::value;
 *      
 * StaticMax function taken without any thought shamelessly from 
 *      https://gist.github.com/tibordp/6909880#file-variant-cc-L106
 */
template <size_t arg1, size_t ... others>
struct StaticMax;
template <size_t arg>
struct StaticMax<arg> {
    static const size_t value = arg;
};
template <size_t arg1, size_t arg2, size_t ... others>
struct StaticMax<arg1, arg2, others...> {
    static const size_t value = (arg1 >= arg2) ? 
        (StaticMax<arg1, others...>::value) :
        (StaticMax<arg2, others...>::value);
};


/*
 * Used to get the index of a particular type in a parameter pack, so for
 * example if you the index of the type int in the parameter pack 
 *      <double, int, string>
 * is a 1, the index for double is a 0 and similarly the index for string is 2
 */
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

/*
 * Used to check whether a type is contained within a parameter pack, so for
 * example the type int is in the parameter pack <double, string, int> but the
 * type float is not
 */
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

/*
 * Get the type for the index specified.  so this is a mapping from integer to
 * a type.  This can be used like follows
 *      TypeForIndex<0, int, double, string>::type
 * and here the type for the index 0 is an int. 
 */
template <int current_index, typename... Vs>
struct TypeForIndex {};
template <typename Head, typename... Tail>
struct TypeForIndex<0, Head, Tail...> {
  using type = Head;
};
template <int current_index, typename Head, typename... Tail>
struct TypeForIndex<current_index, Head, Tail...> :
  TypeForIndex<current_index-1, Tail...>
{};

/*
 * Used to check whether all the types in the template parameter pack are of
 * the same type, so for example <int, int, int> are all of the same type but
 * <int, int, double> and <int, string, int> are not.  Behavior is well
 * defined with even one or less than one type.  So <int> and <> are true
 * since all the types in the parameter pack are the same
 */
template <typename... Types>
struct AreAllSame {
    constexpr static const bool value = (sizeof...(Types) <= 1) ? 
        (true) :
        (false);
};
template <typename ToCheck, typename... Tail>
struct AreAllSame<ToCheck, ToCheck, Tail...> : 
    std::integral_constant<bool, true && 
        AreAllSame<ToCheck, Tail...>::value> 
{};
template <typename ToCheck>
struct AreAllSame<ToCheck, ToCheck> :
    std::integral_constant<bool, true>
{};


/* ******** Utilities ********* */
