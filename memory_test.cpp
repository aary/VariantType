#include <sstream>
#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;
using std::ostringstream;

extern int max_heap_usage;

ostringstream oss;

int main() {
    // __attribute__((unused)) vector<int> vec {1, 2, 3};
    int* pointers = new int{0};
    oss << *pointers << endl;
    oss << "Max memory usage was " << max_heap_usage << " bytes" << endl;

    cout << oss.str();
    return 0;
}
