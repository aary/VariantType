#include <iostream>
using std::cerr;
#include <stdexcept>
using std::exception;

// run all unit tests
extern void run_tests();

int main() {

    try {
        run_tests();
    } catch(std::exception& exc) {
        cerr << exc.what();
        throw exc;
    }

    return 0;
}
