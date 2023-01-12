#ifndef ERR_H
#define ERR_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
    ERR_OK = 0,
    ERR_BAD_MALLOC,
    ERR_ASSEMBLER,
    ERR_FILE,
    ERR_VM,
} err_t;

err_t err_get(void);
void  err_set(err_t e);
void  err_format(FILE* stream);

#define ERR_FORWARD()               \
    do {                            \
        if (err_get() != ERR_OK) {  \
            goto error;             \
        }                           \
    } while (0)

#define ERR(e)          \
    do {                \
        err_set(e);     \
        goto error;     \
    } while (0)

#define ERR_IF(b, e)    \
    do {                \
        if (b) {        \
            ERR(e);     \
        }               \
    }while(0)

extern const char* err_str_map[];
extern const uint64_t err_maps_size;

#endif
