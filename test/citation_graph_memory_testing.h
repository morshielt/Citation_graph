
#define GLIBCXX_FORCE_NEW

#ifndef ZAD5_CITATION_GRAPH_MEMORY_TESTING_H
#define ZAD5_CITATION_GRAPH_MEMORY_TESTING_H
#include <cstdlib>
#include <iostream>
#include <new>

extern long long steps_left_to_throw;
extern bool test_memory;
bool prepare_for_end = false;

size_t allocs = 0;
size_t deallocs = 0;

void* operator new(std::size_t sz){

    if (prepare_for_end) {
        std::terminate();
    }

    if (test_memory) {
        //fprintf(stderr, "new called");

        --steps_left_to_throw;
        if (steps_left_to_throw <= 0) {
            throw "new exception";
        }
    }

    void *res = std::malloc(sz);

    if (res != nullptr) {
        ++allocs;
    }
    return res;
}

void operator delete(void* ptr) noexcept{
    if (ptr != nullptr) {
        ++deallocs;
    }
    if (prepare_for_end && deallocs == allocs) {
        fprintf(stderr, "after atexit: delete: memory seems to be successfully freed\n");
    }
    std::free(ptr);
}

void memCheck() {
    fprintf(stderr, "atexit [this can mismatch,"
                    " atexit is not always called after all frees], "
                    "you should see one more info if everything is ok: %lu %lu\n", allocs, deallocs);
    prepare_for_end = true;
}

#endif //ZAD5_CITATION_GRAPH_MEMORY_TESTING_H
