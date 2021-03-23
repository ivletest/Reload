//
// Created by ivan on 18.3.21.
//

#include "Heap.h"

#include <cstdlib>

void *Mem::Alloc16(const int size) {
    if ( !size ) {
        return nullptr;
    }

    const int paddedSize = (size + 15) & ~15;
    return std::aligned_alloc((size_t)paddedSize, 16);
}

void Mem::Free16(void *ptr) {
    if (ptr == nullptr) {
        return;
    }

    std::free(ptr);
}
void *	Mem::ClearedAlloc(const int size) {
    void * mem = Mem::Alloc(size);
    memset(mem, 0, (size_t)size);
    return mem;
}


