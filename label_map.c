#include "label_map.h"

static inline uint32_t fnv_hash(char* s)
{
    uint32_t h = 3;
    for(; *s; ++s) {
        h *= 97;
        h ^= *s;
    }
    return h;
}

static inline bool iseq_str(char* a, char* b)
{
    return strcmp(a, b) == 0;
}

MAKE_MAP_IMPLEMENTATION(label_map, char*, uint64_t, fnv_hash, iseq_str, 4, 2, .75f, .5f, .25f)
