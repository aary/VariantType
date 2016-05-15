# VariantType

A C++ union based variant type class implemented with compile time
polymorphism.

### Building
This project relies on several features of C++14 that are incompatible with
previous versions of C++ so this must be compiled with C++14.  The header
`VariantType.hpp` contains all the relevant function declarations and the
implementations are in the other .cpp files in the /src directory.  To run the
unit tests type in ``make test``. 

Note: Another variant type implemented with some template metaprogramming is
under development and is placed in the `template_metaprogrammed_variant`
folder.  This will be further developed on the templates branch

By Aaryaman Sagar (rmn100@gmail.com) 
