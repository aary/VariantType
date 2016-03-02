#include "VariantType.hpp"
#include "VariantTypeImpl.hpp"
#include <string>
using std::string;

VariantType::VariantType(const char* input_string) :
    implementation{new VariantType::Impl{input_string}} {}

VariantType::VariantType(const string& input_string) :
    implementation{new VariantType::Impl{input_string}} {}

VariantType::VariantType(string&& input_string) :
    implementation{new VariantType::Impl{input_string}} {}

VariantType::VariantType(double input_double) :
    implementation{new VariantType::Impl{input_double}} {}

VariantType::VariantType(int input_integer) :
    implementation{new VariantType::Impl{input_integer}} {}

VariantType::VariantType(bool input_boolean) :
    implementation{new VariantType::Impl{input_boolean}} {}

VariantType::VariantType(VariantType&& other) {
    this->implementation.swap(other.implementation);
}

VariantType& VariantType::operator=(VariantType&& other) {
    this->implementation.swap(other.implementation);
    return *this;
}
VariantType::operator const std::string&() const {
    return static_cast<const std::string&>(*this->implementation);
}
VariantType::operator double() const {
    return static_cast<double>(*this->implementation);
}
VariantType::operator int() const {
    return static_cast<int>(*this->implementation);
}
VariantType::operator bool() const {
    return static_cast<bool>(*this->implementation);
}

bool VariantType::operator<(const VariantType& other) const {
    return *this->implementation < *other.implementation;
}

bool VariantType::operator>(const VariantType& other) const {
    return *this->implementation > *other.implementation;
}

bool VariantType::operator==(const VariantType& other) const {
    return *this->implementation == *other.implementation;
}

bool VariantType::operator!=(const VariantType& other) const {
    return *this->implementation != *other.implementation;
}

std::ostream& operator<<(std::ostream& os, const VariantType& variant_type) {
    os << *variant_type.implementation;
    return os;
}

// size_t std::hash<VariantType>::operator() (const VariantType& variant_type) {
//     return std::hash<VariantType>()(*variant_type.implementation);
// }
