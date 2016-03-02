/*
 * Written by Aaryaman Sagar (rmn100@gmail.com) for EECS 281 @ The University
 * of Michigan - Ann Arbor
 *
 * A VariantType class to be used with the project.  This class can be tagged
 * as either being a string, bool, int, or a double floating point number.
 * Objects of this class are always meant to be constructed explicitly by
 * calling the constructor to avoid confusion with other classes. 
 */
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <iostream>

class VariantType {
public:
    
    /*
     * Constructors, these are all marked explicit to avoid implicitly
     * constructing objects of this class with a string, int, double or a
     * boolean.  Consult this link for the keyword explicit 
     *      http://en.cppreference.com/w/cpp/language/explicit
     *
     * Try to always construct an object with the {} notation instead of the
     * parentheses.  This prevents narrowing and is supposed to be the new
     * standardized method of object construction.  For example 
     *      DestructivePotato potato{argument_list}
     */
    VariantType(const char*);
    VariantType(const std::string&);
    VariantType(std::string&&);
    VariantType(double);
    VariantType(int);
    VariantType(bool);

    /*
     * Copying over a variant type is not allowed!  Moving one however is ok.
     * The copy constructor has been forbid to disallow small errors like
     * passing a value by copy. 
     */
    VariantType(const VariantType&) = delete;
    VariantType(VariantType&&);
    VariantType& operator=(VariantType&& other);
    
    /*
     * These functions should be used to compare a VariantType object with
     * another object of type VariantType.  To compare an object of this type
     * with the type it contains you can cast it first via a C-style cast or a
     * static_cast. 
     */
    bool operator<(const VariantType&) const;
    bool operator>(const VariantType&) const;
    bool operator==(const VariantType&) const;
    bool operator!=(const VariantType&) const;

    /*
     * These functions should be used if an object of the inner type of the
     * VariantType is to be returned.  For example doing 
     *      if (static_cast<int>(variant_type_object) < 2) 
     * will be legal.  These will cry and fail if you try to convert a type to
     * another that does not exist.  
     */
    explicit operator const std::string&() const;
    explicit operator double() const;
    explicit operator int() const;
    explicit operator bool() const;

    /*
     * Make friends!
     */
    template <typename TYPE>
    friend struct std::hash;
    friend std::ostream& operator<<(std::ostream& os, 
            const VariantType& variant_type);
    friend std::ostream& operator<<(std::ostream& os,
            const VariantType::Impl& impl);
    friend class Impl;

    /*
     * The implementation of this variant type
     */
    class Impl;

    /*
     * The opaque pointer idiom.  We do not want you to see how this class is
     * implemented!  Something to think about though - everything in python is
     * an Object, how do you think that works? 
     */
    std::unique_ptr<Impl> implementation;
};


/* 
 * These extensions allow the VariantType class to be used with the
 * unordered_map class from the C++ standard library.  The use of
 * multiunordered_map is not recommended. 
 */
namespace std {
    template <> struct hash<VariantType> {
        size_t operator() (const VariantType&) const;
    };
}

/*
 * Utility method so that you can print an instance of this class to cout or
 * any other ostream object for example ostringstream.  
 */
std::ostream& operator<<(std::ostream& os, const VariantType& variant_type);

