#include <new>
#include <cstdlib>
#include <dlfcn.h>
#include <mutex>
#include <iostream>
#include <map>
#include <cassert>
#include <iostream>
#include <sstream>
using std::map;
using std::mutex;
using std::unique_lock;
using std::ostream;
using std::cout;
using std::endl;
using std::ostringstream;

#ifdef DEBUG
__attribute__((unused)) static ostream& memory_output_stream {cout};
#else 
__attribute__((unused)) static ostringstream oss;
__attribute__((unused)) static ostream& memory_output_stream {oss};
#endif

// Overloads for the standard library memory functions
extern "C" {
    void* malloc(size_t size);
    void* calloc(size_t count, size_t size);
    void free(void* ptr);
}
void* operator new(size_t count); 
void* operator new[](size_t count);
void operator delete(void* ptr) _NOEXCEPT;
void operator delete[](void* ptr) _NOEXCEPT;

// A mutex for thread safety and a boolean to avoid stack overflows with a
// std::map and overloaded new and malloc functions
static std::mutex memory_lock;
static std::mutex atomic_making_lock; // to make the checking and locking atomic
static bool in_user_defined_function {false};
static unique_lock<mutex> atomically_check_and_acquire_memory_lock();

// Bookkeeping for the memory usage
int max_heap_usage {0};
static map<uintptr_t, int>& get_memory_map();
static void track_max_usage();
template <typename Func> 
static void execute_bookkeeping_code(Func function);

extern "C" {
void* malloc(size_t size) {

    // memory_output_stream << "Malloc called with size " << size << endl;
    // lock
    { auto raii_lock = atomically_check_and_acquire_memory_lock();
    
        // get the original malloc function call
        static auto original_malloc = (decltype(&malloc)) dlsym(RTLD_NEXT, 
                "malloc");

        // Get the pointer from malloc
        void* pointer = original_malloc(size);
        uintptr_t pointer_handle = reinterpret_cast<uintptr_t>(pointer);

        execute_bookkeeping_code([&]() {
            // assert that the pointer does not already exist in the memory map
            assert("memory should not exist in memory map before allocation" && 
                    get_memory_map().find(pointer_handle) == 
                    get_memory_map().end());
            
            // add to bookkeeping
            get_memory_map()[pointer_handle] = size;
            track_max_usage();
        });

        // memory_output_stream << "Malloc returned " << pointer << endl;
        return pointer;
    }
}

void* calloc(size_t count, size_t size) {
    // memory_output_stream << "calloc called with size " << size << endl;
    return ::malloc(count * size);
}

void free(void* ptr) {

    // memory_output_stream << "free called with pointer " << ptr << endl;

    // lock
    { auto raii_lock = atomically_check_and_acquire_memory_lock();
    
        // get the original free function
        static auto original_free = (decltype(&free)) dlsym(RTLD_NEXT, "free");

        uintptr_t pointer_handle = reinterpret_cast<uintptr_t>(ptr);

        execute_bookkeeping_code([&] () {
            // assert that the heap memory map already has the pointer
            assert("memory to be freed does not exist in the heap memory map" && 
                        get_memory_map().find(pointer_handle) != 
                        get_memory_map().end());

            // add to bookkeeping
            get_memory_map().erase(pointer_handle);
        });

        // free the memory
        original_free(ptr);
    }
}

}

void* operator new(size_t count) {

    // memory_output_stream << "operator new called with size " << count << endl;
    auto pointer = malloc(count);
    if (!pointer) {
        throw std::bad_alloc{};
    }

    return pointer;
}

void* operator new[](size_t count) {
    
    // memory_output_stream << "operator new[] called with size " << count << endl;

    return operator new(count);
}

void operator delete(void* ptr) _NOEXCEPT {

    // memory_output_stream << "operator delete called with ptr " << ptr << endl;
    free(ptr);
}

void operator delete[](void* ptr) _NOEXCEPT {
    // memory_output_stream << "operator delete[] called with ptr " << ptr << endl;
    free(ptr);
}

/*
 * This function updates the variable max_heap_usage to have the max value if
 * the current memory map dictates that the memory usage is greater than what
 * it was before.
 */
void track_max_usage() {

    // loop through all keys and add up the values
    int sum {0};
    for (const auto ele : get_memory_map()) { sum += ele.second; }

    // assign to max
    max_heap_usage = std::max(max_heap_usage, sum);
}


unique_lock<mutex> atomically_check_and_acquire_memory_lock() {

    // acquire lock
    unique_lock<mutex> lck {atomic_making_lock};
        
        return (in_user_defined_function) ?
            unique_lock<mutex> {} :
            unique_lock<mutex> {memory_lock};
}

template <typename Func>
void execute_bookkeeping_code(Func function) {

    // if this function has been called from a location within any of the
    // overloads defined in this file then skip the bookkeeping
    if (!in_user_defined_function) {
        in_user_defined_function = true;
            function();
        in_user_defined_function = false;
    }
}

map<uintptr_t, int>& get_memory_map() {
    { auto raii_lock = atomically_check_and_acquire_memory_lock();

        static map<uintptr_t, int>* heap_memory_map {nullptr};
        if (!heap_memory_map) {

            // this function will always be called from within the library so
            // there is no need to say whether the new is being called from
            // within.
            heap_memory_map = new map<uintptr_t, int>; 
        }
        return *heap_memory_map;
    }
}
