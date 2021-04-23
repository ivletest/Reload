#include "Heap.h"

#include <cstdlib>

void *Mem::Alloc16(const int size) {
    if (!size) {
        return nullptr;
    }

    const int paddedSize = (size + 15) & ~15;
#ifdef WIN32
    return _aligned_malloc((size_t)paddedSize, 16);
#else
    return std::aligned_alloc((size_t)paddedSize, 16);
#endif
}

void *	Mem::ClearedAlloc(const size_t size) {
    void * mem = Alloc(size);
    memset(mem, 0, size);
    return mem;
}


