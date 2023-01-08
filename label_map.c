#include "label_map.h"

static inline uint32_t fnv_hash(const char* s)
{
    uintptr_t h = 2166136261;
    for (; *s; s++) {
        h *= 16777619;
        h ^= *s;
    }
    return h;
}

static inline bool is_eq_str(const char*restrict a, const char*restrict b) 
{ 
    return strcmp(a, b) == 0; 
}

MAKE_MAP_IMPLEMENTATION(label_map,              // prefix
                        char*, uint64_t,        // (key, value)
                        fnv_hash, is_eq_str,    // functions 
                        4, 2,                   // init size, and growth factor
                        .75f, .5f, .25f)        // load factors
