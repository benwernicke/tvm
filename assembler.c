#include "assembler.h"

#include <stddef.h>

typedef enum {
    ASM_ERR_OK = 0,
} asm_err_t;

static asm_err_t err = 0;

int assemble(char* target, char** src, uint64_t src_len)
{
    char* dbug = NULL;
    char* code = NULL;
    char* data = NULL;
    return 0;
}
