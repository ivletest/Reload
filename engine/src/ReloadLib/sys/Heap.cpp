#include "Heap.h"

#include <cstdlib>

void *Mem::Alloc16(const int size) {
    if (!size) {
        return nullptr;
    }

    const int paddedSize = (size + 15) & ~15;
    return std::aligned_alloc((size_t)paddedSize, 16);
}

void *	Mem::ClearedAlloc(const size_t size) {
    void * mem = Alloc(size);
    memset(mem, 0, size);
    return mem;
}


