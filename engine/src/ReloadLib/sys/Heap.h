//
// Created by ivan on 18.3.21.
//

#ifndef RELOAD_HEAP_H
#define RELOAD_HEAP_H

#include "precompiled.h"

#include "Common.h"

class Mem {
public:
    static inline void *    Alloc(size_t size) {
#ifdef USE_ALIGNED_ALLOC
        return Mem::Alloc16(size);
#else
        return malloc(size);
#endif
    }

    static inline void      Free(void *ptr) {
        if (ptr != nullptr) {
            std::free(ptr);
        }
    }

    static void *		    ClearedAlloc(size_t size);

private:
    static void *           Alloc16(int size);
};

#endif //RELOAD_HEAP_H
