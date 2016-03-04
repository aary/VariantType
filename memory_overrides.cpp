#include <new>
#include <cstdlib>
#include <dlfcn.h>
#include <mutex>
#include <iostream>


// The output stream
extern std::ostream& test_output_stream;

// Overloads for the standard library memory functions
extern "C" {
    void* malloc(size_t size);
    void* calloc(size_t count, size_t size);
    void free(void* ptr);
}
void* operator new(size_t count); 
void* operator new[](size_t count);
void operator delete(void* ptr);
void operator delete[](void* ptr);

// A mutex for thread safety
static std::mutex memory_lock;

// Bookkeeping for the memory usage

static int max_heap_usage = 0;
static std::map<uintptr_t, int> heap_memory_map;
static void track_max_usage(std::map<uintptr_t, int> heap_memory_map, 
        int& max_heap_usage);

extern "C" {
void* malloc(size_t size) {

    // lock
    std::unique_lock<std::mutex> lck(memory_lock);
    
    // get the original malloc function call
    static auto original_malloc = (decltype(&malloc)) dlsym(RTLD_NEXT, "malloc");

    // Get the pointer from malloc
    void* pointer = original_malloc(size);
    uintptr_t pointer_handle = reinterpret_cast<uintptr_t>(pointer);

    // assert that the pointer does not already exist in the memory map
    assert("memory should not exist in memory map before allocation" && 
            heap_memory_map.find(pointer_handle) == heap_memory_map.end());
    
    // add to bookkeeping
    heap_memory_map[pointer_handle] = size;
    track_max_usage(heap_memory_map, max_heap_usage);

    return pointer;
}

void* calloc(size_t count, size_t size) {

    // get the original calloc
    static auto original_calloc = (decltype(&calloc)) dlsym(RTLD_NEXT, "calloc");

    // get the pointer returned by calloc
    void* pointer = original_calloc(count, size);
    uintptr_t pointer_handle = reinterpret_cast<uintptr_t>(pointer);

    // assert that the memory has not been allocated before
    assert("memory should not exist in the memory map before allocation" && 
            heap_memory_map.find(pointer_handle) == heap_memory_map.end());

    // add to bookkeeping
    heap_memory_map[pointer_handle] = size * count;
    track_max_usage(heap_memory_map, max_heap_usage);

    return pointer;
}

void free(void* ptr) {
    
    // get the original free function
    static auto original_free = (decltype(&free)) dlsym(RTLD_NEXT, "free");
    uintptr_t pointer_handle = reinterpret_cast<uintptr_t>(ptr);

    // assert that the heap memory map already has the pointer
    assert("memory to be freed does not exist in the heap memory map" && 
                heap_memory_map.find(pointer_handle) != heap_memory_map.end());

    // add to bookkeeping
    heap_memory_map.erase(pointer_handle);

    // free the memory
    original_free(ptr);
}

}

/*
 * Inputs:  A map containing pointer values and the amount of heap memory used
 *          after that point
 *
 *          The variable that keeps track of the max memory usage till this
 *          point
 *
 * This function updates the variable to have the max value if the current
 * memory map dictates that the memory usage is greater than what it was
 * before.
 */
void track_max_usage(std::map<uintptr_t, int> heap_memory_map, 
        int& max_heap_usage) {

    // loop through all keys and add up the values
    int sum {0};
    for (const auto ele : heap_memory_map) { sum += ele.second; }

    // assign to max
    max_heap_usage = std::max(max_heap_usage, sum);
}

