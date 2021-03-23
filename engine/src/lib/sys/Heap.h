//
// Created by ivan on 18.3.21.
//

#ifndef RELOAD_HEAP_H
#define RELOAD_HEAP_H

#include "../../Common.h"

class Mem {
public:
    static inline void *    Alloc(const int size) { return Mem::Alloc16(size); }
    static inline void      Free(void *ptr) { Mem::Free16(ptr); }
    static void *		    ClearedAlloc(int size);

private:
    static void *           Alloc16(int size);
    static void             Free16(void *ptr);
};

#endif //RELOAD_C_HEAP_H
