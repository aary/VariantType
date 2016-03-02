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

#include "VariantType.hpp"
#include <functional>
#include <string>
#include <iostream>
#include <stdexcept>

class VariantType::Impl {
public:
    ~Impl();

    friend class VariantType;

    explicit Impl(const char*);
    explicit Impl(const std::string&);
    explicit Impl(std::string&&);
    explicit Impl(double);
    explicit Impl(int);
    explicit Impl(bool);

    /*
     * Copying over a variant type is not allowed!  Moving one however is ok. 
     */
    Impl(const Impl&) = delete;
    Impl(VariantType::Impl&&);
    
    /*
     * These functions should be used to compare a VariantType::Impl object with
     * another object of type VariantType::Impl
     */
    bool operator<(const VariantType::Impl&) const;
    bool operator>(const VariantType::Impl&) const;
    bool operator==(const VariantType::Impl&) const;
    bool operator!=(const VariantType::Impl&) const;

    /*
     * These functions should be used if an object of the inner type of the
     * VariantType::Impl is to be returned.  For example doing 
     *      if (static_cast<int>(variant_type_object) < 2) 
     * will be legal.  These will cry and fail if you try to convert a type to
     * another that does not exist. 
     */
    operator const std::string&() const;
    operator double() const;
    operator int() const;
    operator bool() const;

    /*
     * Make friends
     */
    friend std::ostream& operator<<(std::ostream& os, const Impl& impl);

    /*
     * The tag enumeration for the type of the class
     */
    enum class Tag {STRING, DOUBLE, INTEGER, BOOLEAN};
    Tag tag;

    /*
     *  A union containing all the elements of the class, anonymous to avoid
     *  causing small memory leaks. 
     */ 
    union {
        std::string inner_string;
        double inner_double;
        int inner_integer;
        bool inner_boolean;
    };

    /*
     * The standard exception type for this class.  Inherits from
     * std::exception to be safe. 
     */
    class VariantTypeException : public std::logic_error {
        using std::logic_error::logic_error;
    };

private:
    /*
     * Some utility methods for the class, these check for exceptional
     * conditions and correspondingly throw exceptions. 
     */
    void check_same_type(const Impl& other, 
            const std::string& error_message) const;
    void check_if_type_is(Tag tag, const std::string& message) const;
};

/* 
 * These extensions allow the VariantType class to be used with the
 * unordered_map class from the C++ standard library.  The use of
 * multiunordered_map is not recommended. 
 */
namespace std {
    template <> struct hash<VariantType::Impl> {
        size_t operator() (const VariantType::Impl&);
    };
}

/*
 * Print the implementation to an output stream
 */
std::ostream& operator<<(std::ostream& os, const VariantType::Impl& impl);

/*
 * An assert for this library that throws exceptions
 */
#ifdef NDEBUG
#define assert_variant(condition)
#else
#define assert_variant(condition) \
    assert_variant_private(__LINE__, condition, #condition, __FILE__, __FUNCTION__);
#endif

extern void assert_variant_private(int line_number, bool condition_to_assert, 
        const char* condition, const char* filename, const char* function);
